#include "types.h"
///mnt/c/Users/huawei/OneDrive/YCITOJ/judge-server
const std::string serv_path = std::string("../");
const std::string comp_path = std::string("../comp/comp.sh");
const std::string ans_path = serv_path + std::string("prob/");
const std::string user_out = serv_path + std::string("gen/out/");
const std::string exec_out = serv_path + std::string("gen/exec/");
const std::string judger_path = serv_path + std::string("judge-core/judger");
const std::string source_path = serv_path + std::string("sub/");

void JudgeTask::show_task(){
    using namespace std;
    cout << submitid << endl;
    
}

int parse_int(char* &data){
    int ret = 0;
    while (*data == ' ') data++;
    while(isdigit(*data)){
        ret = ret * 10 + (*data) - '0';
        data++;
    }
    return ret;
}

std::string parse_string(char* &data){
    std::string str;
    char tmp[MAX_ARG_LEN];
    int cnt = 0;
    while(*data == ' ') data++;  
    while(*data != ' ' && *data != 0){
        tmp[cnt++] = *data;
        data++;
    }
    tmp[cnt] = '\0';
    str = std::string(tmp);
    return str;
}

std::string parse_code(char* data){
    std::string str;
    char tmp[MAX_CODE_LEN];
    int cnt = 0;
    while(*data == ' ') data++;
    while(*data != '\0'){
        tmp[cnt++] = *data;
        data++;
    }
    tmp[cnt] = '\0';
    str = std::string(tmp);
    return str;
}
std::string que_path(std::string que_id){
    return  ans_path + que_id + std::string("/");
}

JudgeTask parse_task(char* data){
    JudgeTask ret;
    ret.mem_limit = parse_string(data);
    ret.cpu_limit = parse_string(data);
    ret.lang = parse_string(data);
    ret.prob_id = parse_string(data);
    ret.submitid = parse_string(data);
    ret.content = parse_code(data);
    // ret.input_path =  que_path(que_id) + que_id + std::string(".in");
    ret.output_path = user_out + ret.submitid + std::string(".out");
    ret.source_path = source_path + ret.submitid + std::string(".") + ret.lang;
    ret.gen_folder_path = exec_out;
    ret.gen_path = exec_out + ret.submitid;
    ret.comp_path = que_path(ret.prob_id) + ret.prob_id + std::string(".out");
    ret.des_path = que_path(ret.prob_id) + std::string("test_cases.txt");
    return ret;
}   



const char** task_to_args(JudgeTask &jt){
    const char** argv = (const char**)malloc(sizeof(char*) * PARAM_COUNT);
    std::memset(argv, 0, sizeof(argv));
    argv[0] = "Judger";
    argv[1] = jt.mem_limit.c_str();
    argv[2] = jt.cpu_limit.c_str();
    argv[3] = jt.lang.c_str();
    argv[4] = jt.input_path.c_str();
    argv[5] = jt.output_path.c_str();
    argv[6] = jt.source_path.c_str();
    argv[7] = jt.submitid.c_str();
    argv[8] = jt.gen_folder_path.c_str();
    argv[9] = jt.comp_path.c_str();
    argv[10] = NULL;
    return argv;
}

void nxt_case(JudgeTask &jt, std::string cur_case){
    jt.input_path = que_path(jt.prob_id) + cur_case + std::string(".in");
    jt.comp_path = que_path(jt.prob_id) + cur_case + std::string(".out"); 
}