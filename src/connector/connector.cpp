#include "connector.h"
std::queue<JudgeTask> task_queue;
Client judge_server;
pthread_mutex_t mutex;
void Client::client_init()
{
}

void Client::close_connection()
{
    close(stat.client_fd);
}
int Client::open_clientfd(char *host, char *port)
{
    struct addrinfo *list, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;
    hints.ai_flags |= AI_ADDRCONFIG;
    getaddrinfo(host, port, &hints, &list);
    for (p = list; p; p = p->ai_next)
    {
        if ((stat.client_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
        {
            continue;
        }
        if (connect(stat.client_fd, p->ai_addr, p->ai_addrlen) != -1)
        {
            break;
        }
        close(stat.client_fd);
    }
    freeaddrinfo(list);
    if (!p)
        return 0;
    else
    {
        // setsockopt(stat.client_fd, SOL_SOCKET, )
        printf("Connected to %s:%s\n", host, port);
        return 1;
    }
}
void *on_recv(void *app)
{
    conn_stat *stat = (conn_stat *)app;
    while (1)
    {
        char *buff = (char *)malloc(sizeof(char) * MAX_COMM_LEN);
        memset(buff, 0, sizeof(char) * MAX_COMM_LEN);
        int s;
        if ((s = recv(stat->client_fd, buff, MAX_COMM_LEN, 0)) == -1)
        {
            free(buff);
            std::cerr << "Connection closed" << std::endl;
            std::cerr << "Trying to reconnect" << std::endl;
            judge_server.close_connection();
            judge_server.open_clientfd((char *)conf.host.c_str(), (char *)conf.port.c_str());
            sleep(5);
            continue;
        };
        pthread_mutex_lock(&mutex);
#ifdef DEBUG
        printf("Receving:{content:%s, size:%d}\n", buff, sizeof(buff));
#endif
        JudgeTask jt = parse_task(buff);
        task_queue.push(jt);
        free(buff);
        pthread_mutex_unlock(&mutex);
    }
}

bool Client::connect_to_server(char *host, char *port)
{
    if (!open_clientfd(host, port))
    {
        return 0;
    }
    pthread_create(&recv_thread, NULL, on_recv, &stat);
    return 1;
}

bool Client::send_res(Res res)
{
    char *buff = (char *)malloc(sizeof(char) * MAX_COMM_LEN);
    // pthread_mutex_lock(&mutex);
    memset(buff, 0, sizeof(sizeof(char) * MAX_COMM_LEN));
#ifdef DEBUG
    printf("Sending: {judgeid: %s, memuse: %d, cpuuse: %d, score: %d, judge_res: %d}\n", res.judgeid.c_str(), res.memuse, res.cpuuse, res.score, res.jrs);
#endif
    sprintf(buff, "{\n\"judgeid\":\"%s\",\n\"memuse\":\"%d\",\n\"cpuuse\":\"%d\",\n\"score\":\"%d\"\n,\"judge_res\":\"%d\"\n}", res.judgeid.c_str(), res.memuse, res.cpuuse, res.score, res.jrs);
    send(stat.client_fd, buff, strlen(buff), 0);
    free(buff);
    // pthread_mutex_unlock(&mutex);
    return 0;
}