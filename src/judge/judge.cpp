#include "judge.h"
#include <algorithm>
#include "../compare/comp.cpp"

pthread_t judge_pool[MAXN_JUDGE_COUNT];
int avali;

struct JudgerData
{
    int usage_time;
    int usage_mem;
};

JudgerData read_from_pipe(const std::string &pipe_name)
{
    if (access(pipe_name.c_str(), F_OK) == -1)
    {
        if (mkfifo(pipe_name.c_str(), 0777) != 0)
        {
            throw std::runtime_error("create file failed!\n");
        };
    }
    int pipe_fd = open(pipe_name.c_str(), O_RDONLY);

    if (pipe_fd == -1)
        throw std::runtime_error("open file failed!\n");
    JudgerData res;
    read(pipe_fd, &res, sizeof(res));
    close(pipe_fd);
    return res;
}

void *judge(void *params)
{
    pid_t p;
    JudgeTask *jt = (JudgeTask *)params;
    Res res;
    std::vector<std::string> cases;
    pthread_detach(jt->tid);
    memset((void *)&res, 0, sizeof(res));
    res.jrs = UKE;
    res.judgeid = jt->submitid;
    pthread_mutex_lock(&mutex);
#ifdef DEBUG
    // std::cout << "DES_PATH:" << jt->des_path << std::endl;
    // std::cout << "SOURCE: " << jt->source_path << std::endl;
#endif
    std::ofstream ofs(jt->source_path, std::ios::out);
    ofs << jt->content;
    ofs.close();
    std::ifstream ifs(jt->des_path, std::ios::in);
    std::string tmp;
    while (ifs >> tmp)
    {
        cases.push_back(tmp);
    }
    ifs.close();
    pthread_mutex_unlock(&mutex);
    if (cases.size() == 0)
    {
        std::cout << "Empty cases\n";
        pthread_mutex_lock(&mutex);
        judge_server.send_res(res);
        avali++;
        delete jt;
        pthread_mutex_unlock(&mutex);
        pthread_exit(nullptr);
    }
    for (int i = 0; i < cases.size(); i++)
    {
        nxt_case(*jt, cases[i]);
        const char **argv = task_to_args(*jt);
        if ((p = fork()) == 0)
        {
            execve(conf.judger_path.c_str(), (char **)argv, NULL);
            exit(0);
        }
        try
        {
            JudgerData jd = read_from_pipe("/tmp/pipe" + res.judgeid);
            res.cpuuse = std::max(jd.usage_time, res.cpuuse);
            res.memuse = std::max(jd.usage_mem, res.memuse);
        }
        catch (std::runtime_error &e)
        {
            std::cout << e.what() << std::endl;
        }
        int stat;
        waitpid(p, &stat, 0);
        int ans = WEXITSTATUS(stat);
        switch (ans)
        {
        case 0:
            res.jrs = NORMAL;
            break;
        case 1:
            res.jrs = TLE;
            break;
        case 2:
            res.jrs = MLE;
            break;
        case 3:
            res.jrs = RE;
            break;
        case 4:
            res.jrs = UKE;
            break;
        case 5:
            res.jrs = CE;
        default:
            break;
        }
        if (res.jrs == NORMAL)
        {
            res.jrs = AC;
            if (!comp(jt->comp_path, jt->output_path))
            {
                res.jrs = WA;
                goto end_pos;
            }
        }
        else
            break;
    }
end_pos:
    remove(jt->source_path.c_str());
    remove(jt->gen_path.c_str());
    remove(jt->output_path.c_str());
    pthread_mutex_lock(&mutex);
    judge_server.send_res(res);
    avali++;
    delete jt;
    pthread_mutex_unlock(&mutex);
    pthread_exit(nullptr);
}

void judge_init()
{
    avali = conf.max_thread_cnt;
}