#include "judge.h"
pthread_t judge_pool[MAXN_JUDGE_COUNT];
std::queue<pthread_t *> avali;
void *judge(void *params)
{
    pid_t p;
    JudgeTask *jt = (JudgeTask *)params;
    Res res;
    std::vector<std::string> cases;
    memset((void *)&res, 0, sizeof(res));
    res.judgeid = jt->submitid;
    pthread_mutex_lock(&mutex);
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
    for (int i = 0; i < cases.size(); i++)
    {
        nxt_case(*jt, cases[i]);
        const char **argv = task_to_args(*jt);
        if ((p = fork()) == 0)
        {
            execve(judger_path.c_str(), (char **)argv, __environ);
            exit(0);
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
            if ((p = fork()) == 0)
            {
                execve(comp_path.c_str(), (char **)argv, __environ);
            }
            waitpid(p, &stat, 0);
            ans = (WEXITSTATUS(stat));
            std::cout << "COMPRES: " << ans << std::endl;
            if (ans != 0)
            {
                res.jrs = WA;
                break;
            }
        }
        else 
            break;
    }
    pthread_mutex_lock(&mutex);
    judge_server.send_res(res);
    // std::cout << "~~~~~~send:" << res.judgeid << std::endl;
    avali.push(jt->tid);
    delete jt;
    pthread_mutex_unlock(&mutex);
    pthread_exit(nullptr);
}

void judge_init()
{
    for (int i = 0; i < 4; i++)
    {
        avali.push(&judge_pool[i]);
    }
}