#include "util.h"
#include <sstream>
#include <cstdarg>
#include <memory>
#ifndef _WIN32
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#else
#include <windows.h>
#endif
namespace Util
{
	std::string pipe_base_path = "/tmp/";
	std::shared_ptr<std::string> read_file(std::string file_path)
	{
		std::ifstream ifs(file_path);
		if (!ifs.good())
		{
			ifs.close();
			throw std::runtime_error("Failed to read: " + file_path);
		};
		std::stringstream buffer;
		buffer << ifs.rdbuf();
		std::string contents(buffer.str());
		ifs.close();
		return std::make_shared<std::string>(contents);
	}

	void write_to_file(std::string file_path, std::vector<char> write_data){
		std::ofstream ofs(file_path);
		ofs.write(write_data.data(), write_data.size());
		ofs.close();
	}

	void c_log(std::string s){
		std::cout << s << std::endl;
	}

	JSON read_json(std::string file_path)
	{
		return JSON(*read_file(file_path));
	}

	void write_code(std::string sub_id, const std::string& code, const std::string& lang){
		std::string file_pth = Judge::sub_folder + sub_id + "." + lang;
		std::ofstream ofs(file_pth);
		if (!ofs) throw std::runtime_error("Failed to write: " + file_pth);
		ofs << code;
		ofs.close();
	}

	void env_init(){
		if (access("./prob", 0) < 0)
			mkdir("./prob", 0777);
		if (access("./prob/tmp/", 0) < 0)
			mkdir("./prob/tmp/", 0777);
		if (access("./sub", 0) < 0)
			mkdir("./sub", 0777);
		if (access("./out", 0) < 0)
			mkdir("./out", 0777);
		if (access("./exec", 0) < 0)
			mkdir("./exec", 0777);
		if(access("./judger", 0) < 0)
			mkdir("./judger", 0777);
	}

	bool check_file(std::string file_path)
	{
		std::ifstream ifs(file_path.c_str());
		if (ifs.good())
		{
			ifs.close();
			return true;
		}
		else
		{
			ifs.close();
			return false;
		}
	}
	bool check_json(const JSON &json, std::string name)
	{
		if (json.get_type() != JSON::JSONTYPE::GROUP)
			throw std::runtime_error("json type error");
		if (json.get_map().count(name))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	std::string get_prob_path(std::string prob_id){
		int id = std::stoi(prob_id);
		std::string ret = Judge::prob_set_path + std::to_string(id / prob_set_size) + '/' + std::to_string(id % prob_set_size) + '/';
		return ret;
	}

	void create_pipe(const std::string &pipe_pth)
	{
#ifndef _WIN32
		if (access(pipe_pth.c_str(), F_OK) < 0)
		{
			if (mkfifo(pipe_pth.c_str(), 0777) != 0)
			{
				throw std::runtime_error("failed to create fifo");
			}
		}
#else
#endif
	}
	void read_from_pipe(int fd, std::string &s, int len)
	{
#ifndef _WIN32
		s.resize(len);
		read(fd, (void*)s.data(), len);
#else
#endif
	}

	JudgerData read_from_pipe(const std::string &pipe_name)
	{
		if (access(pipe_name.c_str(), F_OK) == -1)
		{
			if (mkfifo(pipe_name.c_str(), 0777) != 0)
			{
				throw std::runtime_error("create file failed!\n");
			};
		}
		int pipe_fd = open(pipe_name.c_str(), O_RDONLY);

		if (pipe_fd == -1)
			throw std::runtime_error("open file failed!\n");
		JudgerData res;
		read(pipe_fd, &res, sizeof(res));
		close(pipe_fd);
		return res;
	}

	int rm_dir(std::string dir_path){
		std::string ins = "rm -rf " + dir_path;
		int status = WEXITSTATUS(system(ins.c_str()));
		return status;
	}


	int unzip(std::string prob_id){
		std::string path = Judge::prob_set_path + std::to_string(std::stoi(prob_id) / prob_set_size) + '/';
		std::string tmp_path = Judge::prob_set_path + "tmp/";
		std::string prob_path = path + prob_id;
		std::string c_name = path + std::to_string(std::stoi(prob_id) % 100);
        std::string ins = "unzip -o -q -d " + path + ' ' + tmp_path + prob_id + ".zip";
		if (access(c_name.c_str(), 0) != -1){
			rm_dir(c_name.c_str());
		}
		if (WEXITSTATUS(system(ins.c_str())) != 0){
			rm_dir(path + prob_id);
			throw std::runtime_error("unzip error");
		}
		rename(prob_path.c_str(), c_name.c_str());
		return 1;
	}
}