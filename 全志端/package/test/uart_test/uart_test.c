#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
// #include <select.h>



int main(int argc, char const *argv[])
{
    fd_set rd_fdset;
    int fd = 0;
    int ret;

    fd = open("/dev/ttyS0", O_RDWR);
    if (fd == -1)
        printf("open error\n");
    // FD_ZERO(&rd_fdset);
    // FD_SET(fd, &rd_fdset);
    // struct timeval tv = {1, 0};
    // ret = select(fd+1, &rd_fdset, NULL, NULL, &tv);
    // if (ret < 0) {
    //     printf("select ret: %d\n", ret);
    // }
    // printf("Hello NeZha\n");
    char str[] = "Hello NeZha\n";
    write(fd, str, strlen(str));
    close(fd);
    return 0;
}
