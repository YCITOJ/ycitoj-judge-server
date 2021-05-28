#include "connector.h"
std::queue<JudgeTask> task_queue;
Client judge_server;
pthread_mutex_t mutex;
void Client::client_init(){
}

void Client::close_connection(){
    close(stat.client_fd);
    pthread_cancel(recv_thread);
}
int Client::open_clientfd(char* host, char* port){
    struct addrinfo *list, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;
    hints.ai_flags |= AI_ADDRCONFIG;
    getaddrinfo(host, port, &hints, &list);
    for (p = list; p; p = p->ai_next){
        if ((stat.client_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0){
            continue;
        }
        if (connect(stat.client_fd, p->ai_addr, p->ai_addrlen) != -1){
            break;
        }
        close(stat.client_fd);
    }
    freeaddrinfo(list);
    if (!p)
        return 0;
    else 
        return 1;
}
void* on_recv(void* app){
    conn_stat* stat = (conn_stat*) app;
    while (1){
        char *buff = (char*)malloc(sizeof(char) * MAX_COMM_LEN);
        memset(buff, 0 ,sizeof(buff));
        recv(stat->client_fd, buff, MAX_COMM_LEN, 0);
        pthread_mutex_lock(&mutex);
        JudgeTask jt = parse_task(buff);
        task_queue.push(jt);
        free(buff);
        // jt.show_task();
        pthread_mutex_unlock(&mutex);
        // std::cout << "id: " << ts.submitid << " added to queue" << std::endl;
    }
}

bool Client::connect_to_server(char* host, char* port){
    if (!open_clientfd(host, port)){
        return 0;
    }
    printf("Connected to %s:%s\n", host, port);
    pthread_create(&recv_thread, NULL, on_recv, &stat);
    return 1;
}

bool Client::send_res(Res res){
    char* buff = (char*)malloc(sizeof(char) * MAX_COMM_LEN);
    // pthread_mutex_lock(&mutex);
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "{\n\"judgeid\":\"%s\",\n\"memuse\":\"%d\",\n\"cpuuse\":\"%d\",\n\"score\":\"%d\"\n,\"judge_res\":\"%d\"\n}",res.judgeid.c_str(), res.memuse, res.cpuuse, res.score, res.jrs);
    send(stat.client_fd, buff, strlen(buff), 0);
    free(buff);
    // pthread_mutex_unlock(&mutex);
    return 0;
}