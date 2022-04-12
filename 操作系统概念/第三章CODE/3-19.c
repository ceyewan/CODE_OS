#include "headers.h"
#define SIZE 512

int main()
{
    int fd1[2], fd2[2];
    char buffer[SIZE];
    int rt = pipe(fd1);
    assert(rt != -1);
    rt = pipe(fd2);
    assert(rt != -1);
    pid_t pid = fork();
    assert(pid >= 0);
    if (pid == 0) {
        close(fd1[1]);
        read(fd1[0], buffer, SIZE);
        close(fd1[0]);
        for (int i = 0; i < strlen(buffer); i++) {
            if (buffer[i] >= 'a' && buffer[i] <= 'z') {
                buffer[i] -= 32;
            } else if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
                buffer[i] += 32;
            }
        }
        close(fd2[0]);
        write(fd2[1], buffer, strlen(buffer) + 1);
        close(fd2[1]);
    } else {
        close(fd1[0]);
        write(fd1[1], "Hello World", 12);
        close(fd1[1]);
        wait(NULL);
        read(fd2[0], buffer, SIZE);
        printf("%s\n", buffer);
        close(fd2[0]);
    }
    return 0;
}