#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MIN_PID 300
#define MAX_PID 5000
int* bitset;
int allocate_map(void);
int allocate_pid(void);
void release_pid(int pid);

int main()
{
    allocate_map();
    for (int i = 0; i < 5000; i++) {
        int pid = allocate_pid();
        printf("%d ", pid);
        if (i % 50 == 0)
            printf("\n");
    }
    for (int i = 300; i <= 5000; i++) {
        release_pid(i);
    }
    for (int i = 0; i < 5000 / 32 + 1; i++) {
        if (bitset[i] != 0) {
            printf("false\n");
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