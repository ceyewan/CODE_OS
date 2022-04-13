#include "headers.h"

void* func()
{
    srand((unsigned int)time(NULL));
    int* ret = (int*)malloc(4);
    *ret = 0;
    for (int i = 0; i < 1000; i++) {
        double a = (double)rand() / RAND_MAX * 2 - 1;
        double b = (double)rand() / RAND_MAX * 2 - 1;
        if (a * a + b * b <= 1) {
            *ret += 1;
        }
    }
    return (void*)ret;
}
int main(int argc, char* argv[])
{
    assert(argc == 2);
    int n = atoi(argv[1]);
    pthread_t p[100];
    for (int i = 0; i < n; i++) {
        pthread_create(&p[i], NULL, func, NULL);
    }
    int sum = 0, *count;
    for (int i = 0; i < n; i++) {
        pthread_join(p[i], (void**)&count);
        sum += *count;
    }
    printf("PI = %.6f\n", (double)sum / (n * 1000) * 4);
    return 0;
}