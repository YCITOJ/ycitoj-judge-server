#include "definitions.h"
#include "json_parser.hpp"
#include "util.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <memory>
#include <sstream>
const size_t head_len = sizeof(int);
const int prob_set_size = 100;
namespace Judge {
	std::string prob_set_path;
	std::string judger_path;
	std::string sub_folder;
	std::string user_out_folder;
	std::string exec_folder;
	const int judger_arg_count = 9;
	JSON* config;
	void init() {
		std::ifstream ifs("./settings/default.json");
		if (ifs.good()) {
			ifs.close();
			std::shared_ptr<std::string> str = Util::read_file("./settings/default.json");
			config = new JSON(*str);
		}
		else {
			throw std::runtime_error("Unable to find default.json");
		}

	}
}