#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
const int BUF_SIZE = 512;
const char *fifo_name = "./ycitoj";
int main()
{
    const int open_mod = O_WRONLY;
    char buf[BUF_SIZE];

    if (access(fifo_name, F_OK) == -1)
    {
        int res = mkfifo(fifo_name, 0777);

        if (res != 0)
        {
            std::cout << "Open pipe failed";
            return 0;
        }
    }

    int pipe_fd = open(fifo_name, open_mod);

    if (pipe_fd != -1)
    {
        *(int *)buf = 6832;
        int res = write(pipe_fd, buf, BUF_SIZE);
        if (res == -1)
        {
            std::cout << "Write failed.";
            return 0;
        }
    }
    std::cout<<"OK"<<std::endl;
    close(pipe_fd);
    return 0;
}