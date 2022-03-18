#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>

void *my_sum(void *arg)
{
    int *x = (int *)arg;
    int *sum = (int *)malloc(sizeof(int));
    *sum = 0;
    for (int i = 1; i <= *x; i++)
    {
        *sum += i;
    }
    return (void *)sum;
}

int main(int argc, char *argv)
{
    // 创建子进程程
    int rc = fork();
    assert(rc >= 0);
    if (rc > 0)
    {
        /* 进程 A 等待子进程执行，然后输出 Hello World. */
        rc = wait(NULL);
        printf("Process A(PID = %d) wait for process B(PID = %d).\n\n", getpid(), rc);
        printf("(PID = %d) This is process A.\n", getpid());
        printf("Hello World.\n");
    }
    else if (rc == 0)
    {
        /* 进程 B 创建一个求和的线程，然后输出求和结果。 */
        printf("(PID = %d) This is process B.\n", getpid());
        pthread_t p;
        int x = 100;
        // 创建一个线程，实现累加
        rc = pthread_create(&p, NULL, my_sum, (void *)&x);
        assert(rc == 0);
        int *ans;
        // 等待线程结束
        rc = pthread_join(p, (void **)&ans);
        assert(rc == 0);
        printf("The sum is: %d\n\n", *ans);
    }
    return 0;
}
