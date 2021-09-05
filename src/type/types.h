#pragma once
#include <string>
#include <iostream>
#include <ctype.h>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define MAX_ARG_LEN 100
#define PARAM_COUNT 15
#define MAX_CODE_LEN 2000
#define MAX_HOST_LEN 80
#define MAX_PORT_LEN 10
enum ResultTag
{
    NORMAL,
    TLE,
    MLE,
    RE,
    UKE,
    CE,
    AC,
    WA
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

struct Config
{
    ///mnt/c/Users/huawei/OneDrive/YCITOJ/judge-server
    std::string serv_path;
    std::string comp_path;
    std::string prob_path;
    std::string user_out;
    std::string exec_out;
    std::string judger_path;
    std::string source_path;
    std::string host; 
    std::string port;
    int max_thread_cnt;
    Config(){
        serv_path = std::string("../");
        comp_path = std::string("../comp/comp.sh");
        user_out = serv_path + std::string("gen/out/");
        exec_out = serv_path + std::string("gen/exec/");
        judger_path = serv_path + std::string("judge-core/judger_linux");
        source_path = serv_path + std::string("sub/");
    }
};
/*
#DEFAULT
///mnt/c/Users/huawei/OneDrive/YCITOJ/judge-server
const std::string serv_path = std::string("../");
const std::string comp_path = std::string("../comp/comp.sh");
const std::string ans_path = serv_path + std::string("prob/");
const std::string user_out = serv_path + std::string("gen/out/");
const std::string exec_out = serv_path + std::string("gen/exec/");
const std::string judger_path = serv_path + std::string("judge-core/judger_linux");
const std::string source_path = serv_path + std::string("sub/");
*/
struct JudgeTask
{
    std::string mem_limit;
    std::string cpu_limit;
    std::string lang;
    std::string input_path;
    std::string output_path;
    std::string source_path;
    std::string submitid;
    std::string gen_folder_path;
    std::string gen_path;
    std::string content;
    std::string comp_path;
    std::string test_case;
    std::string des_path;
    int prob_id;
    pthread_t tid;
    int testcases;
    JudgeTask(){};
    JudgeTask(std::string _mem, std::string _cpu, std::string _lang, std::string _input, std::string _output, std::string _source, std::string _sub, std::string _gen) : mem_limit(_mem), cpu_limit(_cpu), lang(_lang), input_path(_input), output_path(_output), source_path(_source), submitid(_sub), gen_folder_path(_gen) {}
    void show_task();
};

extern Config conf;
JudgeTask parse_task(char *data);
const char **task_to_args(JudgeTask &jt);
void nxt_case(JudgeTask &jt, std::string cur_case);
void serv_init();