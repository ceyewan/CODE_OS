#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
//初始化互斥锁
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
//初始化条件变量
pthread_cond_t myCond = PTHREAD_COND_INITIALIZER;
//设置全局变量
int x = 0;
//线程执行的函数
void* waitForTrue(void* args)
{
    int res;
    //条件变量阻塞线程之前，先对互斥锁执行“加锁”操作
    res = pthread_mutex_lock(&myMutex);
    if (res != 0) {
        printf("waitForTrue 加锁失败\n");
        return NULL;
    }
    printf("------等待 x 的值为 10\n");
    if (pthread_cond_wait(&myCond, &myMutex) == 0) {
        printf("x = %d\n", x);
    }
    //最终将互斥锁解锁
    pthread_mutex_unlock(&myMutex);
    return NULL;
}
//线程执行的函数
void* doneForTrue(void* args)
{
    int res;
    while (x != 10) {
        //对互斥锁执行“加锁”操作
        res = pthread_mutex_lock(&myMutex);
        if (res == 0) {
            x++;
            printf("doneForTrue：x = %d\n", x);
            sleep(1);
            //对互斥锁“解锁”
            pthread_mutex_unlock(&myMutex);
        }
    }
    //发送“条件成立”的信号，解除 mythread1 线程的“被阻塞”状态
    res = pthread_cond_signal(&myCond);
    if (res != 0) {
        printf("解除阻塞失败\n");
    }
    return NULL;
}
int main()
{
    int res;
    pthread_t mythread1, mythread2;
    res = pthread_create(&mythread1, NULL, waitForTrue, NULL);
    if (res != 0) {
        printf("mythread1线程创建失败\n");
        return 0;
    }
    res = pthread_create(&mythread2, NULL, doneForTrue, NULL);
    if (res != 0) {
        printf("mythread2线程创建失败\n");
        return 0;
    }
    //等待 mythread1 线程执行完成
    res = pthread_join(mythread1, NULL);
    if (res != 0) {
        printf("1：等待线程失败\n");
    }
    //等待 mythread2 线程执行完成
    res = pthread_join(mythread2, NULL);
    if (res != 0) {
        printf("2：等待线程失败\n");
    }
    //销毁条件变量
    pthread_cond_destroy(&myCond);
    return 0;
}