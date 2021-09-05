#pragma once
#include <unistd.h>
#include <pthread.h>
#include <fstream>
#include <vector>
#include <sys/wait.h>
#include "../type/types.h"
#include "../connector/connector.h"
#define MAXN_JUDGE_COUNT 8
extern pthread_t judge_pool[MAXN_JUDGE_COUNT];
extern int avali;

class Judger{
public:
    void start_judge();
private:
    pthread_t thread_pool[MAXN_JUDGE_COUNT];
    std::queue<pthread_t*> avali;

};

void* judge(void* params);
void judge_init();