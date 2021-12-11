#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include "net.h"
#include "util.h"
#include "judge.h"
#include "compare.h"
#include "GUI.h"
#include "file.h"
#include <thread>
#include <vector>
#include <chrono>
#include <time.h>
int main()
{
	Util::env_init();
	Judge::JudgeServer judge_server;
	try
	{
		judge_server.run_judge();
		// judge_server.run_judge();

	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}