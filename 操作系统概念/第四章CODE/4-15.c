#include "headers.h"
#define MIN_PID 300
#define MAX_PID 5000
int* bitset;
int allocate_map(void);
int allocate_pid(void);
void release_pid(int pid);

void* func()
{
    int pid = allocate_pid();
    int temp = rand() % 10;
    sleep(temp);
    release_pid(temp);
}

int main()
{
    allocate_map();
    pthread_t p[100];
    struct timeval start, end;
    gettimeofday(&start, NULL);
    for (int i = 0; i < 100; i++) {
        pthread_create(&p[i], NULL, func, NULL);
    }
    for (int i = 0; i < 100; i++) {
        pthread_join(p[i], NULL);
    }
    gettimeofday(&end, NULL);
    printf("%d s\n", end.tv_sec - start.tv_sec);
    for (int i = 0; i < 5000 / 32 + 1; i++) {
        if (bitset[i] != 0) {
            printf("false\n");
            return 0;
        }
    }
    printf("true\n");
    return 0;
}
int allocate_map()
{
    bitset = (int*)malloc(sizeof(int) * (MAX_PID / 32 + 1));
    if (bitset == NULL)
        return -1;
    memset(bitset, 0, sizeof(int) * (MAX_PID / 32 + 1));
    return 1;
}

int allocate_pid()
{
    for (int i = MIN_PID; i <= MAX_PID; i++) {
        int a = i / 32, b = i % 32;
        if (((bitset[a] >> b) & 1) == 0) {
            bitset[a] |= 1 << b;
            return i;
        }
    }
    return -1;
}

void release_pid(int i)
{
    int a = i / 32, b = i % 32;
    if (((bitset[a] >> b) & 1) == 1) {
        bitset[a] ^= 1 << b;
    }
}