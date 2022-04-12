#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    assert(argc == 2);
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        printf("error\n");
        exit(1);
    } else if (pid == 0) {
        printf("child: %d\n", getpid());
        int n = atoi(argv[1]);
        while (n > 1) {
            printf("%d, ", n);
            if (n % 2 == 0) {
                n = n / 2;
            } else {
                n = 3 * n + 1;
            }
        }
        printf("%d\n", n);
    } else {
        wait(NULL);
        printf("parent: %d\n", getpid());
    }
    return 0;
}