#pragma once
#include <string>
#include <fstream>
#include <atomic>
#include <map>
#include "net.h"
#include "utile.h"
#include "file.h"

namespace Judge {
	class Case {
	public:
		Case(const std::string& case_id, const std::string& prob_path);
	public:
		std::string ans_in;
		std::string ans_out;
	};

	class Task {
	public:
		Task(std::string prob_id, std::string sub_id, std::string _lang, const std::string& code);
		Task() {};	
	public:
		void clear();
		bool init();
	public:
		std::string prob_id;
		std::string lang;
		std::string compiler_msg;
		std::string submit_id;
		std::string prob_path;
		std::string user_source;
		std::string user_out;
		std::string exec_path;
		std::string pipe_path;
		std::string code;
		int time_limit;
		int mem_limit;
		std::vector<Case> cases;
	};

	class JudgeServer :public Net::File_Server {
	public:
		JudgeServer();
		~JudgeServer() {};
	public:
		void run_judge();
		void async_run_judge();
		int get_pressure() const;
	public:
		int max_threads;
		std::atomic<int> avali_threads;
		TSQueue<Task> task_queue;
		std::map<int, std::vector<Task>> freezed_tasks;
	private:
		void query_info(std::string op);
		bool check_version(int ver, std::string prob_id);
		JSON read_prob_info(std::string prob_id);
		void pull_prob(std::string prob_id);
		void handle_task(JSON msg);
		void judge_task(Task task);
	private:
		bool run_in_main;
		std::thread run_thread;
	};
}