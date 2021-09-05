#include "headfile.h"
int main()
{
    serv_init();
    judge_init();
    if(judge_server.wait_for_client(conf.port) == 0) exit(0);
    bool working = 0;
    while (1)
    {
        if (!task_queue.empty() && avali > 0)
        {
            JudgeTask *task = new JudgeTask();
            pthread_mutex_lock(&mutex);
            *task = task_queue.front();
            task_queue.pop();
            avali--;
            pthread_create(&task->tid, NULL, judge, (void *)task);
            pthread_mutex_unlock(&mutex);
        }
        else if (task_queue.empty())
        {
            usleep(10000);
        }
        else
        {
#ifdef __linux__
            pthread_yield();
#endif
        }
    }
}