#include "GUI.h"
#include "net.h"
using namespace std;
void watch(const Judge::JudgeServer& s){
    using namespace Utile;
    while (1){
        std::string stat;
        if (s.stat == Net::RUNNING){
            stat = "RUNNING";
        }
        else if (s.stat == Net::WAITING) {
            stat = "WAITING";
        }
        else {
            stat = "SLEEPING";
        }
        system("clear");
        printf("*************************************************\n");
        printf(" Net:\n");
        printf(" Connection: %s\n", s.is_connected ? "Connected" : "Disconnected");
        printf(" Remote IP: %s\n", s.is_connected ? s.remote_ip.c_str() : "-");
        printf(" Server:\n");
        printf(" Pressure: %d\n", s.get_pressure());
        printf("\n");
        printf("*************************************************\n");
        // auto file_map = s.file_map;
        // for (auto t : file_map){
        //     auto file = t.second;
        //     c_log("Trans id: " + to_string(t.first) + " " + to_string((file.cur_size * 100ll) / file.size) + "%");
        // }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
