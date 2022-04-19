#include "headers.h"
#define MAX 100
int num[MAX];
typedef struct a {
    int start;
    int end;
} Arg;
pthread_barrier_t barrier;

void init()
{
    pthread_barrier_init(&barrier, NULL, 11);
    for (int i = 0; i < MAX; i++) {
        num[i] = rand() % MAX;
    }
}

void* sort(void* arg)
{
    int a = ((Arg*)arg)->start, b = ((Arg*)arg)->end;
    for (int i = a; i < b; i++) {
        for (int j = i + 1; j <= b; j++) {
            if (num[i] > num[j]) {
                int temp = num[i];
                num[i] = num[j];
                num[j] = temp;
            }
        }
    }
    int ret = pthread_barrier_wait(&barrier);
    return NULL;
}
int main()
{
    init();
    pthread_t p[10];
    Arg arg[10];
    for (int i = 0; i < 10; i++) {
        arg[i].start = MAX / 10 * i;
        arg[i].end = MAX / 10 * (i + 1) - 1;
        pthread_create(&p[i], NULL, sort, (void*)&arg[i]);
    }
    // …………
    pthread_barrier_wait(&barrier);
    // merge
    for (int i = 0; i < MAX; i++) {
        printf("%d ", num[i]);
        if (i % 10 == 9)
            printf("\n");
    }
    pthread_barrier_destroy(&barrier);
    return 0;
}