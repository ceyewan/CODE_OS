#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        printf("error\n");
        exit(1);
    } else if (pid == 0) {
        printf("child: %d\n", getpid());
        sleep(30);
    } else {
        printf("parent: %d\n", getpid());
        wait(NULL);
    }
    return 0;
}