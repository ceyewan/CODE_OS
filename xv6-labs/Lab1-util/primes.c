#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void my_prime(int p[2])
{
    int prime;
    int next;
    close(p[1]);
    if (read(p[0], &prime, 4) != 4)
    {
        fprintf(2, "error\n");
        exit(1);
    }
    // 从管道中读取一个数字，一定是素数。因为我们采用的筛法，不是素数会被筛选出去，留下的最开头一个一定是素数
    printf("prime %d\n", prime);
    // 如果管道中还有没处理完的数据，就像父进程创建这个进程一样创建一个新的进程
    if (read(p[0], &next, 4) == 4)
    {
        // 新的管道
        int newp[2];
        pipe(newp);
        int rc = fork();
        if (rc == 0)
        {
            my_prime(newp);
        }
        else if (rc > 0)
        {
            close(newp[0]);
            // 将后续的数字从前一个管道取出，写入后一个管道，而且数字如果是当前素数的倍数，那么就不必写入了
            do
            {
                if (next % prime != 0)
                {
                    write(newp[1], &next, 4);
                }
            } while (read(p[0], &next, 4) == 4);
            close(p[0]);
            close(newp[1]);
            wait(0);
            exit(0);
        }
        else
        {
            fprintf(2, "error\n");
            exit(1);
        }
        exit(0);
    }
    exit(0);
}
int main(int argc, char const *argv[])
{
    // 期待一个参数
    if (argc != 1)
    {
        fprintf(2, "error\n");
        exit(1);
    }
    // 管道
    int p[2];
    pipe(p);
    int rc = fork();
    if (rc == 0)
    {
        my_prime(p);
        exit(0);
    }
    else if (rc > 0)
    {
        close(p[0]);
        // 向管道中传入所有的数字
        for (int i = 2; i <= 35; i++)
        {
            // 每次 4 个字节，也就是一个数字
            if (write(p[1], &i, 4) != 4)
            {
                fprintf(2, "error\n");
                exit(1);
            }
        }
        close(p[1]);
        wait(0);
        exit(0);
    }
    else
    {
        fprintf(2, "error\n");
        exit(1);
    }
}