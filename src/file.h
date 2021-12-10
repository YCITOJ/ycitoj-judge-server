#pragma once
#include "net.h"
extern clock_t start;
namespace Net
{
    class File
    {
    public:
        int size;
        int cur_size;
        std::string file_path;
        std::string file_name;
        std::vector<char> data;
    public:
        File(int _size, std::string _file_path, std::string _file_name) : file_path(_file_path), size(_size), file_name(_file_name)
        {
            data.resize(size);
            cur_size = 0;
        };
        File(){cur_size = 0; size = 0;};
    public:
        void print();
    };
    class File_Pack
    {
    public:
        int trans_id;
        int pack_size;
        int insert_pos;
    public:
        File_Pack(int _trans_id, int _pack_size, int _insert_pos) : trans_id(_trans_id), pack_size(_pack_size), insert_pos(_insert_pos){};
    };
    class File_Server : public Server
    {
    public:
        int add_pack(File_Pack file_pack, std::vector<uint8_t> file_data);
        void insert_file(File file, int trans_id);
        File_Server():Server(){};
        virtual const std::map<int, File>* get_file_map();
        void clear();
    private:
        std::map<int, File> file_map;
    };
}