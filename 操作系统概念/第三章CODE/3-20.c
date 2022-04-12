#include "headers.h"
#define SIZE 512

int main(int argc, char* argv[])
{
    assert(argc == 3);
    char* file1 = argv[1];
    char* file2 = argv[2];
    char buffer[SIZE];
    memset(buffer, 0, sizeof(buffer));
    int fd[2];
    int rt = pipe(fd);
    int pid = fork();
    assert(pid >= 0);
    if (pid == 0) {
        printf("child start\n");
        close(fd[1]);
        read(fd[0], buffer, SIZE);
        close(fd[0]);
        int f = open(file2, O_WRONLY | O_CREAT);
        write(f, buffer, strlen(buffer));
        close(f);
        printf("child end\n");
    } else {
        printf("parent start\n");
        close(fd[0]);
        int f = open(file1, O_RDONLY);
        read(f, buffer, SIZE);
        close(f);
        write(fd[1], buffer, strlen(buffer) + 1);
        close(fd[1]);
        wait(NULL);
        printf("parent end\n");
    }
    return 0;
}