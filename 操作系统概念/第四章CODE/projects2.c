#include "headers.h"
#define LENGTH 10
typedef struct {
    int start, end, length;
} Arg;
int num[LENGTH] = { 6, 2, 4, 5, 3, 9, 1, 8, 7, 0 };

void* sort(void* arg)
{
    int start = ((Arg*)arg)->start, end = ((Arg*)arg)->end;
    for (int i = start; i < end; i++) {
        for (int j = i + 1; j <= end; j++) {
            if (num[i] > num[j]) {
                int temp = num[i];
                num[i] = num[j];
                num[j] = temp;
            }
        }
    }
    return NULL;
}

void* merge(void* arg)
{
    int start1 = ((Arg*)arg)->start, start2 = ((Arg*)arg)->end, length = ((Arg*)arg)->length;
    int* temp = (int*)malloc(sizeof(int) * length);
    int index = 0, i = 0, j = start2;
    while (i < start2 && j < length) {
        if (num[i] <= num[j]) {
            temp[index++] = num[i++];
        } else {
            temp[index++] = num[j++];
        }
    }
    while (i < start2) {
        temp[index++] = num[i++];
    }
    while (j < length) {
        temp[index++] = num[j++];
    }
    for (int i = 0; i < LENGTH; i++) {
        num[i] = temp[i];
    }
    free(temp);
    return NULL;
}
int main()
{
    int mid = LENGTH / 2;
    pthread_t p[3];
    Arg arg1;
    arg1.start = 0, arg1.end = mid - 1, arg1.length = mid;
    pthread_create(&p[0], NULL, sort, (void*)&arg1);
    Arg arg2;
    arg2.start = mid, arg2.end = LENGTH - 1, arg2.length = LENGTH - mid;
    pthread_create(&p[1], NULL, sort, (void*)&arg2);
    pthread_join(p[0], NULL);
    pthread_join(p[1], NULL);
    Arg arg3;
    arg3.start = 0, arg3.end = mid, arg3.length = LENGTH;
    pthread_create(&p[2], NULL, merge, (void*)&arg3);
    pthread_join(p[2], NULL);
    for (int i = 0; i < LENGTH; i++) {
        printf("%d ", num[i]);
    }
    printf("\n");
    return 0;
}