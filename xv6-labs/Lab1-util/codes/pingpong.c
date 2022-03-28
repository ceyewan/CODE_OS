#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
    // 期望参数的数量是 1
    if (argc != 1)
    {
        fprintf(2, "error\n");
        exit(1);
    }
    // 管道，p[0] 为读管道，p[1] 为写管道
    int p[2];
    pipe(p);
    int rc = fork();
    if (rc == 0)
    {
        // 子进程先从管道中读取一个字节
        char buf[2];
        if (read(p[0], buf, 1) != 1)
        {
            fprintf(2, "error\n");
            exit(1);
        }
        // 及时关闭管道
        close(p[0]);
        printf("%d: received ping\n", getpid());
        // 子进程先管道中写入一个字节
        if (write(p[1], buf, 1) != 1)
        {
            fprintf(2, "error\n");
            exit(1);
        }
        // 关闭管道
        close(p[1]);
        exit(0);
    }
    else if (rc > 0)
    {
        char buf[2] = "a";
        // 向管道中写入一个字节
        if (write(p[1], buf, 1) != 1)
        {
            fprintf(2, "error\n");
            exit(1);
        }
        close(p[1]);
        // 等到子进程结束，这样后面读到的一定是子进程写入的而不是刚刚自己写入的
        wait(0);
        // 从管道中读取一个字节
        if (read(p[0], buf, 1) != 1)
        {
            fprintf(2, "error\n");
            exit(1);
        }
        // 关闭管道
        close(p[0]);
        printf("%d: received pong\n", getpid());
        exit(0);
    }
    else
    {
        fprintf(2, "error\n");
        exit(1);
    }
}