#include "compare.h"
#include <string>
#include <cstring>
namespace Comparator
{
    bool compare(std::string out, std::string ans, std::string &msg)
    {
        std::ifstream user_out(out.c_str());
        std::ifstream ans_out(ans.c_str());
        std::string user_line;
        std::string ans_line;
        if (!ans_out.good()){
            msg = "user file not found";
            return 0;
        }
        if (!ans_out.good()){
            msg = "ans file not found";
            return 0;
        }
        int cnt = 1;
        while (getline(ans_out, ans_line))
        {
            if (!getline(user_out, user_line))
            {
                msg = "Line " + std::to_string(cnt) + " differ";
                return 0;
            }
            while (user_line.back() == '\r' || user_line.back() == '\n' || user_line.back() == ' ')
                user_line.pop_back();
            while (ans_line.back() == '\r' || ans_line.back() == '\n' || ans_line.back() == ' ')
                ans_line.pop_back();
            if (user_line.size() != ans_line.size() || memcmp(user_line.data(), ans_line.data(), ans_line.size()))
            {
                msg = "Line " + std::to_string(cnt) + " differ";
                return 0;
            }
            cnt++;
        }
        if (getline(user_out, user_line)){
            msg = "Line " + std::to_string(cnt) + " differ";
            return 0;
        }
        else {
            msg = "check " + std::to_string(cnt - 1) + " lines " + "accept";
        }
        return 1;
    }
}
