#pragma once
#include <string>
#include "json_parser.hpp"
extern const size_t head_len;
extern const int max_body_len;
extern const int prob_set_size;
namespace Judge {
	extern std::string prob_set_path;
	extern std::string judger_path;
	extern std::string sub_folder;
	extern std::string user_out_folder;
	extern std::string exec_folder;
	extern JSON* config;
	void init();
}