#include "headfile.h"
int main(){
    config_init();
    judge_server.connect_to_server((char*)conf.host.c_str(), (char*)conf.port.c_str());
    judge_init();
    bool working = 0;
    while (1){
        if (!task_queue.empty() && !avali.empty()){
            // system("clear");
            std::cout << "Avalible: " << avali.size() << std::endl;
            std::cout << "Totle Tasks: " << task_queue.size() << std::endl; 
            JudgeTask* task = new JudgeTask();
            pthread_mutex_lock(&mutex);
            *task = task_queue.front();
            task->tid = avali.front();
            task_queue.pop();
            avali.pop();
            pthread_create(task->tid, NULL, judge, (void*)task);
            pthread_detach(*task->tid);
            pthread_mutex_unlock(&mutex);
        }
        else if (task_queue.empty()){
            usleep(10000);
        }
        else {
            pthread_yield();
        }
    }
}