#include "headers.h"
#define SIZE 512
typedef struct node {
    int length;
    int* num;
} Arg;

void* my_max(void* arg)
{
    int length = ((Arg*)arg)->length;
    int* num = ((Arg*)arg)->num;
    int* ret = (int*)malloc(4);
    *ret = num[0];
    for (int i = 1; i < length; i++) {
        *ret = *ret > num[i] ? *ret : num[i];
    }
    return (void*)ret;
}

void* my_min(void* arg)
{
    int length = ((Arg*)arg)->length;
    int* num = ((Arg*)arg)->num;
    int* ret = (int*)malloc(4);
    *ret = num[0];
    for (int i = 1; i < length; i++) {
        *ret = *ret < num[i] ? *ret : num[i];
    }
    return (void*)ret;
}

void* my_avg(void* arg)
{
    int length = ((Arg*)arg)->length;
    int* num = ((Arg*)arg)->num;
    int* ret = (int*)malloc(4);
    *ret = 0;
    for (int i = 0; i < length; i++) {
        *ret += num[i];
    }
    *ret = (*ret) / length;
    return (void*)ret;
}

int main(int argc, char* argv[])
{
    assert(argc > 1);
    int num[SIZE];
    for (int i = 1; i < argc; i++) {
        num[i - 1] = atoi(argv[i]);
    }
    int *max, *min, *avg;
    pthread_t p[3];
    Arg arg;
    arg.length = argc - 1;
    arg.num = num;
    pthread_create(&p[0], NULL, my_max, (void*)&arg);
    pthread_create(&p[1], NULL, my_min, (void*)&arg);
    pthread_create(&p[2], NULL, my_avg, (void*)&arg);
    pthread_join(p[0], (void**)&max);
    pthread_join(p[1], (void**)&min);
    pthread_join(p[2], (void**)&avg);
    printf("max = %d, min = %d, avg = %d\n", *max, *min, *avg);
    return 0;
}