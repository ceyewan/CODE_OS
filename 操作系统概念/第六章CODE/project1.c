#include "headers.h"
#define MAX 100
struct {
    int seats; // 空闲的凳子
    pthread_mutex_t seatlock;
    sem_t ta;
} mutex;

void* consumer(void* arg)
{
    int count = 0; // 记录已经教了多少学生了，退出循环
    while (1) {
        pthread_mutex_lock(&mutex.seatlock);
        if (mutex.seats < 3) { // 查看有多少空闲凳子，如果少于 3 说明有学生在等
            mutex.seats += 1;
            pthread_mutex_unlock(&mutex.seatlock);
            teach();
            count++;
            if (count == MAX - 1)
                break;
            sem_wait(&mutex.ta); // 减少一个 post
        } else {
            pthread_mutex_unlock(&mutex.seatlock);
            sem_wait(&mutex.ta); // wait ，也就是睡觉
            printf("没有学生，助教睡觉\n");
        }
    }
}

void* producer(void* arg)
{
    while (1) {
        study(); // 学习
        pthread_mutex_lock(&mutex.seatlock);
        if (mutex.seats == 0) { // 没有空闲凳子
            pthread_mutex_unlock(&mutex.seatlock);
            printf("学生 %d 没有找到空位，返回学习\n", gettid());
        } else {
            mutex.seats -= 1;
            pthread_mutex_unlock(&mutex.seatlock);
            sem_post(&mutex.ta); // 叫醒助教（有可能本来就是醒的）
            printf("学生 %d 找到空位，叫醒助教，等待请教\n", gettid());
            break;
        }
    }
}
int main()
{
    mutex.seats = 3;
    pthread_mutex_init(&mutex.seatlock, NULL);
    sem_init(&mutex.ta, 0, 0);
    pthread_t p[MAX];
    pthread_create(&p[0], NULL, consumer, NULL);
    for (int i = 1; i < MAX; i++) {
        pthread_create(&p[i], NULL, producer, NULL);
    }
    for (int i = 0; i < MAX; i++) {
        pthread_join(p[i], NULL);
    }
    return 0;
}