#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include "file.h"
#include "utile.h"
clock_t start;
namespace Net
{
    void File::print(){
        using namespace Utile;
        c_log("file_name: " + file_name);
        c_log("file_path: " + file_path);
        c_log("file_size: " + std::to_string(size));
    }

    int File_Server::add_pack(File_Pack file_pack, std::vector<uint8_t> file_data)
    {
        File &file = file_map[file_pack.trans_id];
        memcpy(file.data.data() + file_pack.insert_pos, file_data.data(), file_pack.pack_size);
        file.cur_size += file_pack.pack_size;
        std::cout << "Trans id: " << file_pack.trans_id << " : " << file.cur_size * 100ll / file.size << "%" << std::endl;
        if (file.cur_size == file.size)
        {   
            Utile::c_log("Write to " + file.file_path + file.file_name);
            std::string dest_path = file.file_path + file.file_name + ".zip";
            Utile::write_to_file(dest_path, file.data);
            int ret = std::stoi(file.file_name);
            Utile::unzip(file.file_name);
            remove(dest_path.c_str());
            file_map.erase(file_pack.trans_id);
            return ret;
        }
        return 0;
    }

    void File_Server::insert_file(File file, int trans_id){
        if (file_map.count(trans_id)){
            file_map.erase(trans_id);
        }
        file_map.insert({trans_id, file});
    }

    const std::map<int, File>* File_Server::get_file_map(){
        return &file_map;
    }

    void File_Server::clear(){
        file_map.clear();
    }
}