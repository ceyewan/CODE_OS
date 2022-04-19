#include "headers.h"
#define LOOPS 10000
int count = 0;
pthread_mutex_t lock;

void* func()
{
    srand((unsigned int)time(NULL));
    for (int i = 0; i < LOOPS; i++) {
        double a = (double)rand() / RAND_MAX * 2 - 1;
        double b = (double)rand() / RAND_MAX * 2 - 1;
        if (a * a + b * b <= 1) {
            pthread_mutex_lock(&lock);
            count += 1;
            pthread_mutex_unlock(&lock);
        }
    }
}
int main(int argc, char* argv[])
{
    assert(argc == 2);
    int n = atoi(argv[1]);
    assert(n <= 1000);
    pthread_mutex_init(&lock, NULL);
    pthread_t p[1000];
    for (int i = 0; i < n; i++) {
        pthread_create(&p[i], NULL, func, NULL);
    }
    for (int i = 0; i < n; i++) {
        pthread_join(p[i], NULL);
    }
    printf("PI = %.6f\n", (double)count / (n * LOOPS) * 4);
    return 0;
}