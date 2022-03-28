#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int readline(char *arg[32], int arc)
{
    char buf[1024];
    int n = 0;
    // 一个字节一个字节的读，直到满了或者 "\n"
    while (read(0, buf + n, 1))
    {
        if (n == 1023)
        {
            fprintf(2, "error\n");
            exit(1);
        }
        if (buf[n] == '\n')
        {
            break;
        }
        n++;
    }
    buf[n] = 0;
    if (n == 0)
        return 0;
    int offset = 0;
    // 切分，将 ' ' 改成 0 （也就是 NULL）
    while (offset < n)
    {
        arg[arc++] = buf + offset;
        while (buf[offset] != ' ' && offset < n)
        {
            offset++;
        }
        while (buf[offset] == ' ' && offset < n)
        {
            buf[offset++] = 0;
        }
    }
    return arc;
}

int main(int argc, char const *argv[])
{
    // 期待参数个数大于 1
    if (argc <= 1)
    {
        fprintf(2, "error\n");
        exit(1);
    }
    // 命令
    char *command = (char *)malloc(sizeof(char) * (strlen(argv[1]) + 1));
    strcpy(command, argv[1]);
    // 参数列表
    char *arg[MAXARG];
    for (int i = 1; i < argc; ++i)
    {
        arg[i - 1] = (char *)malloc(sizeof(char) * (strlen(argv[i]) + 1));
        strcpy(arg[i - 1], argv[i]);
    }
    int arc;
    // 每行处理一次，将得到的参数写在后面，生成子进程执行
    while ((arc = readline(arg, argc - 1)) != 0)
    {
        arg[arc] = 0;
        if (fork() == 0)
        {
            exec(command, arg);
            fprintf(2, "error\n");
            exit(1);
        }
        wait(0);
    }
    exit(0);
}