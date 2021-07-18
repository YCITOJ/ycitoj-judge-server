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
#include "../type/types.h"
// #include <thread>
#define LISTENQ 1024
#define MAX_COMM_LEN 5000
#define DEBUG
struct conn_stat
{
    int client_fd;
    bool connected;
};

class Client
{
private:
    struct addrinfo hints;

public:
    conn_stat stat;
    pthread_t recv_thread;
    int open_clientfd(char *host, char *port);
    void client_init();
    bool connect_to_server(char *host, char *port);
    bool send_res(Res res);
    void close_connection();
};

extern std::queue<JudgeTask> task_queue;
extern Client judge_server;
extern pthread_mutex_t mutex;