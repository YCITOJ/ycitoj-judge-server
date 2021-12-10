#include "definitions.h"
#include "json_parser.hpp"
#include "utile.h"
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
		//std::cout << "AAA" << std::endl;
		if (ifs.good()) {
			ifs.close();
			std::shared_ptr<std::string> str = Utile::read_file("./settings/default.json");
			//std::cout << "AAA" << std::endl;
			config = new JSON(*str);
			// std::cout << config->to_string() << std::endl;
		}
		else {
			//std::cout << "AAA" << std::endl;
			throw std::runtime_error("Unable to find default.json");
		}

	}
}