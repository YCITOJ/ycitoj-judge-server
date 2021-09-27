#include "connector.h"
std::queue<JudgeTask> task_queue;
Server judge_server;
pthread_mutex_t mutex;
void Server::server_init()
{
}

void Server::close_connection()
{
    close(client_fd);
}
int Server::open_listenfd(uint16_t port)
{
    server_port = 0;
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cerr << "Failed to create listen_fd" << std::endl;
        return -1;
    }
    memset(&hints, 0, sizeof(hints));
    hints.sin_family = AF_INET;
    hints.sin_addr.s_addr = htonl(INADDR_ANY);
    hints.sin_port |= htons(port);
    int on = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (bind(listen_fd, (sockaddr *)&hints, sizeof(hints)) == -1)
    {
        std::cerr << "Failed to bind listen FD" << std::endl;
        return -1;
    }

    if (listen(listen_fd, 1) < 0)
    {
        std::cerr << "Failed to start listen" << std::endl;
        return -1;
    }
    server_port = port;
    return 0;
}

int Server::accept_conn()
{
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    std::cout << "listening on " << server_port << std::endl;
    if ((client_fd = accept(listen_fd, (sockaddr *)&addr, &addr_len)) <= 0)
    {
        std::cerr << "Failed To Accept Connection" << std::endl;
        return -1;
    }
    std::cout << "Connection established to " << inet_ntoa(addr.sin_addr) << std::endl;
    return 0;
}

void *on_recv(void *fd)
{
    int client_fd = *((int *)fd);
    while (1)
    {
        char *buff = (char *)malloc(sizeof(char) * MAX_COMM_LEN);
        memset(buff, 0, sizeof(char) * MAX_COMM_LEN);
        int s;
        if ((s = recv(client_fd, buff, MAX_COMM_LEN, 0)) == 0 || s == -1)
        {
            std::cout << "invalid data" << std::endl;
            exit(0);
        };
        pthread_mutex_lock(&mutex);
#ifdef DEBUG
        printf("Receving:{content:%s, size:%d}\n", buff, s);
#endif
        JudgeTask jt = parse_task(buff);
        task_queue.push(jt);
        free(buff);
        pthread_mutex_unlock(&mutex);
    }
}

bool Server::wait_for_client(uint16_t port)
{
    if (open_listenfd(port) == -1)
    {
        return 0;
    }

    if (accept_conn() == -1)
    {
        return 0;
    }
    pthread_create(&recv_thread, NULL, on_recv, &client_fd);
    return 1;
}

bool Server::send_res(Res res)
{
    char *buff = (char *)malloc(sizeof(char) * MAX_COMM_LEN);
    // pthread_mutex_lock(&mutex);
    memset(buff, 0, sizeof(sizeof(char) * MAX_COMM_LEN));
#ifdef DEBUG
    printf("Sending: {judgeid: %s, memuse: %d, cpuuse: %d, score: %d, judge_res: %d}\n", res.judgeid.c_str(), res.memuse, res.cpuuse, res.score, res.jrs);
#endif
    sprintf(buff, "{\n\"judgeid\":\"%s\",\n\"memuse\":\"%d\",\n\"cpuuse\":\"%d\",\n\"score\":\"%d\"\n,\"judge_res\":\"%d\"\n}", res.judgeid.c_str(), res.memuse, res.cpuuse, res.score, res.jrs);
    send(client_fd, buff, strlen(buff), 0);
    free(buff);
    // pthread_mutex_unlock(&mutex);
    return 0;
}