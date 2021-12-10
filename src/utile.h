#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <memory>
#include "judge.h"
#include "message.h"
#include "json_parser.hpp"
#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif
namespace Utile
{
	struct JudgerData
	{
		int usage_time;
		int usage_mem;
		char info[512 - sizeof(int) * 2];
		JudgerData(){
			usage_mem = 0;
			usage_time = 0;
			info[0] = 0;
		}
	};
	enum JudgeStat{
		AC, TLE, MLE, RE, UKE, CE, WA
	};
	extern std::string pipe_base_path;
	int get_cpu_count();
	std::shared_ptr<std::string> read_file(std::string file_path);
	void write_to_file(std::string file_path, std::vector<char> write_data);
	void c_log(std::string s);
	bool check_file(std::string file_path);
	bool check_json(const JSON &json, std::string name);
	void write_code(std::string sub_id, const std::string& code, const std::string& lang);
	JSON read_json(std::string file_path);
	void read_from_pipe(int fd, std::string &s, int len);
	std::string get_prob_path(std::string prob_id);
	JudgerData read_from_pipe(const std::string &pipe_name);
	void create_pipe(const std::string &pipe_pth);
	int unzip(std::string prob_id);
	template <typename T, typename... Ts>
	void build_pack(char **pth, T arg, Ts... args_left)
	{
		char *tmp = (char *)malloc(arg.size() + 1);
		strcpy(tmp, arg.c_str());
		*pth = tmp;
		if constexpr (sizeof...(args_left) > 0)
		{
			build_pack(pth + 1, args_left...);
		}
		else
		{
			*(pth + 1) = NULL;
		}
	}


	template <typename... Ts>
	int exec(std::string exec_pth, JSON &msg, std::string pipe_path, Ts... args)
	{
		char *argv[sizeof...(args) + 3];
		// std::cout << pipe_pth << std::endl;
		build_pack(&argv[0], exec_pth, args...);
#ifndef _WIN32
		int pid;
		create_pipe(pipe_path);
		if ((pid = fork()) < 0)
		{
			throw std::runtime_error("failed to create child process");
		}
		else if (pid == 0)
		{
			if (execve(exec_pth.c_str(), argv, nullptr) < 0)
			{
				std::cerr << "failed to launch " << exec_pth.c_str() << std::endl;
				exit(0);
			};
		}
		else
		{
			int status;
			JudgerData data;
			data = read_from_pipe(pipe_path);
			waitpid(pid, &status, 0);
			status = WEXITSTATUS(status);
			msg.add_pair("mem_usage", JSON::val(data.usage_mem));
			msg.add_pair("time_usage", JSON::val(data.usage_time));
			if (status == CE) msg.add_pair("compiler_msg", JSON::val(std::string(data.info)));
			for (int i = 0; i < sizeof...(args) + 2; i++)
			{
				free(argv[i]);
			}
			return status;
		}
#endif
		return 1;
	}

}