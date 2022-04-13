#include "headers.h"
int sudoku[9][9] = {
    { 6, 2, 4, 5, 3, 9, 1, 8, 7 },
    { 5, 1, 9, 7, 2, 8, 6, 3, 4 },
    { 8, 3, 7, 6, 1, 4, 2, 9, 5 },
    { 1, 4, 3, 8, 6, 5, 7, 2, 9 },
    { 9, 5, 8, 2, 4, 7, 3, 6, 1 },
    { 7, 6, 2, 3, 9, 1, 4, 5, 8 },
    { 3, 7, 1, 9, 5, 6, 8, 4, 2 },
    { 4, 9, 6, 1, 8, 2, 5, 7, 3 },
    { 2, 8, 5, 4, 7, 3, 9, 1, 6 }
};
void* row()
{
    int temp[10];
    for (int i = 0; i < 9; i++) {
        memset(temp, 0, sizeof(temp));
        for (int j = 0; j < 9; j++) {
            if (temp[sudoku[i][j]] == 1) {
                int* ans = (int*)malloc(4);
                *ans = 0;
                return (void*)ans;
            } else {
                temp[sudoku[i][j]] = 1;
            }
        }
    }
    int* ans = (int*)malloc(4);
    *ans = 1;
    return (void*)ans;
}

void* col()
{
    int temp[10];
    for (int i = 0; i < 9; i++) {
        memset(temp, 0, sizeof(temp));
        for (int j = 0; j < 9; j++) {
            if (temp[sudoku[j][i]] == 1) {
                int* ans = (int*)malloc(4);
                *ans = 0;
                return (void*)ans;
            } else {
                temp[sudoku[j][i]] = 1;
            }
        }
    }
    int* ans = (int*)malloc(4);
    *ans = 1;
    return (void*)ans;
}

void* sub()
{
    int temp[10];
    for (int k = 0; k < 9; k++) {
        memset(temp, 0, sizeof(temp));
        int x = (k / 3) * 3, y = (k % 3) * 3;
        for (int i = x; i < x + 3; i++) {
            for (int j = y; j < y + 3; j++) {
                if (temp[sudoku[i][j]] == 1) {
                    int* ans = (int*)malloc(4);
                    *ans = 0;
                    return (void*)ans;
                } else {
                    temp[sudoku[i][j]] = 1;
                }
            }
        }
    }
    int* ans = (int*)malloc(4);
    *ans = 1;
    return (void*)ans;
}
int main()
{
    pthread_t p[3];
    pthread_create(&p[0], NULL, row, NULL);
    pthread_create(&p[1], NULL, col, NULL);
    pthread_create(&p[2], NULL, sub, NULL);
    int *a, *b, *c;
    pthread_join(p[0], (void**)&a);
    pthread_join(p[1], (void**)&b);
    pthread_join(p[2], (void**)&c);
    printf("%d\n", *a & *b & *c);
    return 0;
}