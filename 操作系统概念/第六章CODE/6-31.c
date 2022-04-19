#include "headers.h"
#define MAX 100
int resource = MAX;
sem_t lock;
int count = 0;

void* producer(void* arg)
{
    for (int i = 1; i <= 100000; i++) {
        sem_wait(&lock);
        resource += i;
        sem_post(&lock);
    }
}

void* consumer(void* arg)
{
    for (int i = 100000; i > 0; i--) {
        while (1) {
            sem_wait(&lock);
            if (resource >= i) {
                resource -= i;
                sem_post(&lock);
                break;
            } else {
                sem_post(&lock);
            }
        }
    }
}

int main()
{
    sem_init(&lock, 0, 1);
    pthread_t p[4];
    pthread_create(&p[0], NULL, producer, NULL);
    pthread_create(&p[1], NULL, producer, NULL);
    pthread_create(&p[2], NULL, consumer, NULL);
    pthread_create(&p[3], NULL, consumer, NULL);
    for (int i = 0; i < 4; i++) {
        pthread_join(p[i], NULL);
    }
    printf("resource = %d\n", resource);
    printf("count = %d\n", count);
    return 0;
}