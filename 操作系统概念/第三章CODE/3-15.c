#include "headers.h"

int main(int argc, char* argv[])
{
    assert(argc == 2);
    const int SIZE = 4096;
    const char* name = "OS";
    int pid = fork();
    assert(pid >= 0);
    if (pid == 0) {
        printf("child: %d\n", getpid());
        int n = atoi(argv[1]);
        int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
        ftruncate(shm_fd, SIZE);
        void* ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
        while (n > 1) {
            char str[10];
            sprintf(str, "%d", n);
            sprintf(ptr, "%s", str);
            ptr += strlen(str);
            sprintf(ptr, "%s", ",");
            ptr += 1;
            if (n % 2 == 0) {
                n = n / 2;
            } else {
                n = 3 * n + 1;
            }
        }
        sprintf(ptr, "%d\n", n);
    } else {
        wait(NULL);
        printf("parent: %d\n", getpid());
        int shm_fd = shm_open(name, O_RDONLY, 0666);
        // ftruncate(shm_fd, SIZE);
        void* ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
        printf("%s", (char*)ptr);
        shm_unlink(name);
    }
    return 0;
}