#include <string>
#include <fstream>
std::string get_file_content(const std::string& filename)
{
	using namespace std;
	ifstream ifs(filename);
	if (!ifs)
	{
		throw std::runtime_error("invalid file");
	}
	std::string file_content((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
	return file_content;
}

bool comp(std::string file, std::string file2){
    std::string tmp1 = get_file_content(file);
    std::string tmp2 = get_file_content(file2);
    while (tmp1.size() && isspace(tmp1.back())){
        tmp1.pop_back();
    }
    while (tmp2.size() && isspace(tmp2.back())){
        tmp2.pop_back();
    }
    return tmp1 == tmp2;
}