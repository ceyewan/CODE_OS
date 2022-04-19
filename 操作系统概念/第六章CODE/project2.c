#include "headers.h"
#define MAX 5
int forks[MAX];
pthread_mutex_t lock[MAX];
pthread_cond_t c[MAX];

void pickup_forks(int x)
{
    int a = (x + 1) % MAX, b = (x + MAX - 1) % MAX;
    pthread_mutex_lock(&lock[a]);
    while (forks[a] == 1) {
        pthread_cond_wait(&c[a], &lock[a]);
    }
    forks[a] = 1;
    pthread_mutex_unlock(&lock[a]);
    pthread_mutex_lock(&lock[b]);
    while (forks[b] == 1) {
        pthread_cond_wait(&c[b], &lock[b]);
    }
    forks[a] = 1;
    pthread_mutex_unlock(&lock[b]);
}

void return_forks(int x)
{
    int a = (x + 1) % MAX, b = (x + MAX - 1) % MAX;
    pthread_mutex_lock(&lock[a]);
    forks[a] = 0;
    pthread_cond_signal(&c[a]);
    pthread_mutex_unlock(&lock[a]);
    pthread_mutex_lock(&lock[b]);
    forks[b] = 0;
    pthread_cond_signal(&c[b]);
    pthread_mutex_unlock(&lock[b]);
}
void* philosopher(void* arg)
{
    int a = *(int*)arg;
    thinking(a);
    pickup_forks(a);
    eating(a);
    return_forks(a);
}
int main()
{
    memset(forks, 0, sizeof(forks));
    for (int i = 0; i < MAX; i++) {
        pthread_mutex_init(&lock[i], NULL);
        pthread_cond_init(&c[i], NULL);
    }
    pthread_t p[MAX];
    int arg[MAX];
    for (int i = 0; i < MAX; i++) {
        arg[i] = i;
        pthread_create(&p[i], NULL, philosopher, (void*)&arg[i]);
    }
    for (int i = 0; i < MAX; i++) {
        pthread_join(p[i], NULL);
    }
    return 0;
}