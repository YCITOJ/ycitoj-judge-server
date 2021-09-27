#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <queue>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../type/types.h"
// #include <thread>
#define LISTENQ 1024
#define MAX_COMM_LEN 50000
#define DEBUG
class Server
{
private:
    struct sockaddr_in hints;
public:
    pthread_t recv_thread;
    uint16_t server_port;
    int client_fd;
    int listen_fd;
    int open_listenfd(uint16_t port);
    int accept_conn();
    void server_init();
    bool wait_for_client(uint16_t port);
    bool send_res(Res res);
    void close_connection();
};

extern std::queue<JudgeTask> task_queue;
extern Server judge_server;
extern pthread_mutex_t mutex;