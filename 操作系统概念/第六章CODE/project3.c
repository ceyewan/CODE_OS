#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <assert.h>
#include <unistd.h>
#define MAX 4

int buffer[MAX];
int fast = 0; // 写入的位置
int slow = 0; // 读取的位置

// 向缓冲区写入数据
int insert_item(int value)
{
    buffer[fast] = value;
    fast = (fast + 1) % MAX;
    return 0;
}

// 从缓冲区读取数据
int remove_item(int* item)
{
    *item = buffer[slow];
    slow = (slow + 1) % MAX;
    return 0;
}

sem_t empty;
sem_t full;
pthread_mutex_t mutex;

// 生产者，首先等待缓冲区有空闲空间，然后获得缓冲区的锁
// 这里锁需要写在条件变量的里面
void* producer(void* arg)
{
    int time = rand() % 10;
    sleep(time);
    int item = rand();
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    insert_item(item);
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    printf("insert %d\n", item);
}

// 消费者，同上
void* consumer(void* arg)
{
    int time = rand() % 10;
    sleep(time);
    int item;
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    remove_item(&item);
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    printf("remove %d\n", item);
}

int main(int argc, char* argv[])
{
    assert(argc == 4);
    int sleepTime = atoi(argv[1]);
    int producerNum = atoi(argv[2]);
    int consumerNum = atoi(argv[3]);
    // assert(producerNum >= consumerNum);
    sem_init(&empty, 0, MAX); // 初始化 empty 为 MAX ，表明可以向缓冲区写入 MAX 个数字
    sem_init(&full, 0, 0); // 初始化 full 为 0，表明可以从缓冲区读取 0 个数字
    // sem_init(&mutex, 0, 1); 二值信号量，实现锁
    pthread_mutex_init(&mutex, NULL);
    pthread_t p[producerNum + consumerNum];
    for (int i = 0; i < producerNum; i++)
        pthread_create(&p[i], NULL, producer, NULL);
    for (int i = producerNum; i < producerNum + consumerNum; i++)
        pthread_create(&p[i], NULL, consumer, NULL);
    // for (int i = 0; i < producerNum + consumerNum; i++)
    //     pthread_join(p[i], NULL);
    // 根据题意，我们不需要等线程结束，只需要等 sleepTime 后主线程结束
    sleep(sleepTime);
    return 0;
}
