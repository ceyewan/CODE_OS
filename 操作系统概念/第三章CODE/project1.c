#include "headers.h"
#define MAX_LINE 80
int main(void)
{
    char* args[MAX_LINE / 2 + 1];
    char* history[36];
    int index = 0;
    int flag = 0;
    while (1) {
        printf("osh>");
        fflush(stdin);
        fflush(stdout);
        char c;
        char temp[24];
        int i = 0, argc = 0;
        while ((c = getchar()) != '\n') {
            if (c == ' ') {
                char* t = (char*)malloc(sizeof(char) * (i + 1));
                strcpy(t, temp);
                t[i] = '\0';
                // printf("%s\n", t);
                args[argc++] = t;
                i = 0;
            } else {
                temp[i++] = c;
            }
        }
        char* t = (char*)malloc(sizeof(char) * (i + 1));
        strcpy(t, temp);
        t[i] = '\0';
        // printf("%s\n", t);
        args[argc++] = t;
        // 将指令拷贝一份到 history
        history[index] = (char*)malloc(100);
        int sign = 0;
        for (int i = 0; i < argc; i++) {
            for (int j = 0; j < strlen(args[i]); j++) {
                history[index][sign++] = args[i][j];
            }
            history[index][sign++] = ' ';
        }
        if (index == 35) {
            flag = 1;
            history[index][sign - 1] = '\0';
            index = 0;
        } else {
            history[index++][sign - 1] = '\0';
        }
        if (strcmp(args[0], "exit") == 0) {
            return 0;
        } else if (strcmp(args[0], "history") == 0) {
            if (flag == 0) {
                for (int i = index - 1; i > 0; i--) {
                    printf("%d %s\n", i, history[i - 1]);
                }
            } else {
                for (int i = 0; i < 35; i++) {
                    printf("%d %s\n", 35 - i, history[(index + 34 - i) % 36]);
                }
            }
        } else if (strcmp(args[0], "!!") == 0) {
            // to do
        } else if (args[0][0] == '!') {
            // to do
        } else if (strcmp(args[argc - 1], "&") == 0) {
            args[argc - 1] = NULL;
            int pid = fork();
            assert(pid >= 0);
            if (pid == 0) {
                execvp(args[0], args);
            }
        } else {
            args[argc] = NULL;
            int pid = fork();
            assert(pid >= 0);
            if (pid == 0) {
                execvp(args[0], args);
            } else {
                wait(NULL);
            }
        }
    }
    return 0;
}