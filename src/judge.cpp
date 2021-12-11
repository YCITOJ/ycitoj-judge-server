#include "judge.h"
#include "util.h"
#include "compare.h"
#include <algorithm>
#include <thread>
#include <fstream>
namespace Judge
{
	Case::Case(const std::string &case_id, const std::string &prob_path)
	{
		ans_in = prob_path + "cases/" + case_id + ".in";
		ans_out = prob_path + "cases/" + case_id + ".out";
	}

	Task::Task(std::string _prob_id, std::string sub_id, std::string _lang, const std::string &_code)
	{
		int id = std::stoi(_prob_id);
		prob_id = _prob_id;
		submit_id = sub_id;
		lang = _lang;
		prob_path = prob_set_path + std::to_string(id / prob_set_size) + '/' + std::to_string(id % prob_set_size) + '/';
		user_source = sub_folder + sub_id + "." + lang;
		user_out = user_out_folder + sub_id + ".out";
		exec_path = exec_folder + sub_id;
		std::string info_path = prob_path + "cases" + '/' + "config.json";
		code = _code;
	}

	bool Task::init(){
		Util::write_code(submit_id, code, lang);
		std::string info_path = prob_path + "cases" + '/' + "config.json";
		if (access(info_path.c_str(), 0) < 0){
			return 0;
		}
		JSON conf = Util::read_json(info_path);
		std::vector<JSON> case_list = conf["test_cases"].get_list();
		mem_limit = conf["problem_info"]["memory_limit"].get_int();
		time_limit = conf["problem_info"]["time_limit"].get_int();
		pipe_path = Util::pipe_base_path + "pipe" + submit_id;
		for (auto i : case_list)
		{
			cases.push_back(Case(i.get_str(), prob_path));
		}
		return 1;
	}

	void Task::clear()
	{
		remove(user_out.c_str());
		remove(user_source.c_str());
		remove(exec_path.c_str());
		remove((Util::pipe_base_path + "pipe" + submit_id).c_str());
	}

	JudgeServer::JudgeServer() : File_Server()
	{
		try
		{
			max_threads = conf["max_threads"].get_int();
			avali_threads = max_threads;
			prob_set_path = conf["prob_set_path"].get_str();
			if (prob_set_path.back() != '/')
				prob_set_path += '/';

			sub_folder = conf["sub_folder"].get_str();
			if (sub_folder.back() != '/')
				sub_folder += '/';

			user_out_folder = conf["user_out_folder"].get_str();
			if (user_out_folder.back() != '/')
				user_out_folder += '/';

			Util::pipe_base_path = conf["pipe_base_path"].get_str();
			if (Util::pipe_base_path.back() != '/')
				Util::pipe_base_path += '/';

			exec_folder = conf["exec_folder"].get_str();
			if (exec_folder.back() != '/')
				exec_folder += '/';
			judger_path = conf["judger_path"].get_str();
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			return;
		}
	}

	void JudgeServer::handle_task(JSON msg)
	{
		if (Util::check_json(msg, "judge"))
		{
			JSON judge_msg = msg["judge"];
			Task judge_task = Task(judge_msg["prob_id"].get_str(), judge_msg["sub_id"].get_str(), judge_msg["lang"].get_str(), judge_msg["code"].get_str());
			int prob_id = std::stoi(judge_msg["prob_id"].get_str());
			if (check_version(judge_msg["version"].get_int(), judge_msg["prob_id"].get_str()))
			{
				task_queue.push_back(judge_task);
			}
			else {
				auto it = freezed_tasks.find(prob_id);
				if (it != freezed_tasks.end()){
					it->second.push_back(judge_task);
				}
				else{
					std::vector<Task> ins = {judge_task};
					freezed_tasks.insert({prob_id, ins});
					pull_prob(std::to_string(prob_id));
				}
			}
		}
		else if (Util::check_json(msg, "query"))
		{
			JSON query_msg = msg["query"];
			query_info(query_msg.get_str());
		}
		else if (Util::check_json(msg, "file"))
		{
			std::string file_path = Judge::prob_set_path + "tmp/";
			Net::File file(msg["file"]["size"].get_int(), file_path, msg["file"]["file_name"].get_str());
			insert_file(file, msg["file"]["trans_id"].get_int());
		}
		else if (Util::check_json(msg, "file_pack"))
		{
			JSON pack_info = msg["file_pack"];
			Net::File_Pack file_pack(pack_info["trans_id"].get_int(), pack_info["pack_size"].get_int(), pack_info["insert_pos"].get_int());
			int fid;
			if ((fid = add_pack(file_pack, pack_info["data"].get_raw()))){
				auto active_task = freezed_tasks.find(fid);
				if (active_task != freezed_tasks.end()){
					for (auto tasks : active_task->second){
						task_queue.push_back(tasks);
					}
					freezed_tasks.erase(active_task);
				}
			}
		}
		else
		{
			std::cout << "REV: \n"
					  << msg.to_string() << std::endl;
			throw std::runtime_error("Invalid request");
		}
	}

	void JudgeServer::pull_prob(std::string prob_id){
		JSON sendjson = JSON("{}");
		sendjson.add_pair("type", JSON::val("pull_cases"));
		sendjson.add_pair("prob_id", JSON::val(std::stoi(prob_id)));
		Message send_message = Message(sendjson.to_string());
		send(send_message);
	}

	void JudgeServer::query_info(std::string op)
	{
		JSON data("{}");
		if (op == "pressure")
		{
			data.add_pair("pressure", JSON::val(std::to_string(get_pressure())));
		}
		else if (op == "echo")
		{
			data.add_pair("echo", JSON::val("echo"));
		}
		Message msg(data.to_string());
		send(msg);
	}

	bool JudgeServer::check_version(int ver, std::string prob_id)
	{
		std::string prob_folder_path = Util::get_prob_path(prob_id);
		if (access(prob_folder_path.c_str(), 0) == -1) return false;
		int old_version = read_prob_info(prob_id)["problem_info"]["version"].get_int();
		return old_version == ver;
	}

	JSON JudgeServer::read_prob_info(std::string prob_id)
	{
		std::string case_path = Util::get_prob_path(prob_id) + "cases/config.json";
		return Util::read_json(case_path);
	}

	int JudgeServer::get_pressure() const
	{
		return (((max_threads - avali_threads) * 100) / max_threads);
	}

	void JudgeServer::judge_task(Task task)
	{
		task.init();
		// if(!task.init()){
		// 	JSON info = JSON("{}");
		// 	info.add_pair("type", JSON::val("error"));
		// 	info.add_pair("msg", JSON::val("Caselist not exist"));
		// 	info.add_pair("sub_id", JSON::val(task.submit_id));
		// 	info.add_pair("prob_id", JSON::val(task.prob_id));
		// 	Message send_msg = Message(info.to_string());
		// 	send(send_msg);
		// 	return;
		// }
		JSON msg("{}");
		msg.add_pair("type", JSON::val("submission_result"));
		msg.add_pair("subid", JSON::val(task.submit_id));
		int verdict = Util::AC;
		bool err = 0;
		int max_time_usage = 0;
		int max_mem_usage = 0;
		std::vector<JSON> v;
		for (auto i : task.cases)
		{
			JSON case_json("{}");
			int res = Util::exec(judger_path, case_json, task.pipe_path,
								  std::to_string(task.mem_limit), std::to_string(task.time_limit), task.lang, i.ans_in, task.user_out, task.user_source, task.submit_id,
								  exec_folder);
			std::string check_msg;
			max_time_usage = std::max(max_time_usage, (int)case_json["time_usage"].get_int());
			max_mem_usage = std::max(max_mem_usage, (int)case_json["mem_usage"].get_int());
			if (res != 0)
			{
				verdict = res;
				case_json.add_pair("judge_res", JSON::val(res));
				v.push_back(case_json);
				break;
			}
			if (!Comparator::compare(task.user_out, i.ans_out, check_msg))
			{
				case_json.add_pair("check_msg", JSON::val(check_msg));
				case_json.add_pair("judge_res", JSON::val(Util::WA));
				v.push_back(case_json);
				verdict = Util::WA;
				break;
			}
			else
			{
				case_json.add_pair("check_msg", JSON::val(check_msg));
				case_json.add_pair("judge_res", JSON::val(Util::AC));
				v.push_back(case_json);
			}
		}
		msg.add_pair("check_log", JSON::array(v));
		msg.add_pair("verdict", JSON::val(verdict));
		msg.add_pair("max_time_use", JSON::val(max_time_usage));
		msg.add_pair("max_mem_use", JSON::val(max_mem_usage));
		Message send_msg(msg.to_string());
		task.clear();
		// std::cout << msg.to_string() << std::endl;
		// send_msg.show();
		send(send_msg);
		return;
	}

	void JudgeServer::run_judge()
	{
		run_in_main = 1;
		while (true)
		{
			if (!ready_to_connect)
			{
				task_queue.clear();
				clear();
				while (avali_threads != max_threads)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
				}
				// std::cout << "ready: " << avali_threads << std::endl;
				ready_to_connect = 1;
			}
			if (!is_connected)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				continue;
			}
			if (!empty())
			{
				handle_task(JSON(get_msg()));
			}
			if (avali_threads > 0 && !task_queue.empty())
			{
				avali_threads--;
				auto cur_task = task_queue.pop_front();
				std::thread judge_thread([cur_task, this]()
										 {
											try{
											 	judge_task(cur_task);
											 	avali_threads++;
											}
											catch(std::exception& e){
												std::cout << e.what() << std::endl;
												task_queue.clear();
											} });
				judge_thread.detach();
			}
			else if (avali_threads == max_threads && empty())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
			}
		}
	}

	void JudgeServer::async_run_judge()
	{
		run_thread = std::thread([this]()
								 {
									 try
									 {
										run_judge();
									 }
									 catch (std::exception &e)
									 {
										 std::cout << e.what() << std::endl;
									 } });
		run_in_main = false;
	}
}
/*
			// std::cout << task.mem_limit << std::endl;
			// std::cout << task.time_limit << std::endl;
			// std::cout << task.lang << std::endl;
			// std::cout << task.user_out << std::endl;
			// std::cout << task.user_source << std::endl;
			// std::cout << task.submit_id << std::endl;
			// std::cout << exec_folder << std::endl;
			// int res = Util::exec(std::string("/home/muffin/Judge_ServerV2/thread_test/a.out"), case_json, std::string("1"));
*/