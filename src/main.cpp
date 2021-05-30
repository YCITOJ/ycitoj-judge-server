#include "headfile.h"
namespace Test{
    std::string out(source_path + "1.out");
}
int main(){
    
#ifdef _task_test
    // char* buff = (char*)malloc(sizeof(char) * MAX_CONF_LEN);
    // getcwd(buff, MAX_CONF_LEN);
    // while(1){
    //     if (!task_queue.empty()){
            
    //     }
    // }
#endif
#ifdef _client_test
    //clent test
    // Client app;
    // app.client_init();
    // if(app.connect_to_server((char*)"192.168.43.219", (char*)"4000")){
    //     while(1){
    //         // app.send_res();
    //         sleep(3);
    //     }
    //     // sleep(3);
    //     // app.send_res();
    //     // app.Close();
    // }
    // return 0;
#endif
    judge_server.connect_to_server((char*)"192.168.43.219", (char*)"4000");
    judge_init();
    bool working = 0;
    while (1){
        if (!task_queue.empty() && !avali.empty()){
            system("clear");
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