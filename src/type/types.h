#pragma once
#include <string>
#include <iostream>
#include <ctype.h>
#include <cstring>
#define MAX_ARG_LEN 100
#define PARAM_COUNT 15
#define MAX_CODE_LEN 2000
enum ResultTag{
    NORMAL, TLE, MLE, RE, UKE, CE, AC, WA
};
struct Res
{
    ResultTag jrs;
    std::string judgeid;
    int memuse;
    int cpuuse;
    int score;
    
    /* data */
};

struct JudgeTask
{
    std::string mem_limit;
    std::string cpu_limit;
    std::string lang;
    std::string input_path;
    std::string output_path;
    std::string source_path;
    std::string submitid;
    std::string gen_path;
    std::string content;
    std::string comp_path;
    pthread_t* tid;
    int testcases;
    JudgeTask(){};
    JudgeTask(std::string _mem, std::string _cpu, std::string _lang, std::string _input, std::string _output, std::string _source, std::string _sub, std::string _gen):mem_limit(_mem), cpu_limit(_cpu), lang(_lang), input_path(_input), output_path(_output), source_path(_source), submitid(_sub), gen_path(_gen){}
    void show_task();
};


struct conn_stat
{
    int client_fd;
};

JudgeTask parse_task(char* data);

const char** task_to_args(JudgeTask &jt);
extern const std::string ans_path;
extern const std::string source_path;
extern const std::string user_out;
extern const std::string exec_out;
extern const std::string judger_path;
extern const std::string comp_path;