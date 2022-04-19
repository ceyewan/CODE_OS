#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/syscall.h>

pid_t gettid(void)
{
    return syscall(SYS_gettid);
}

void teach()
{
    printf("teach a student\n");
    // sleep(1);
}

void study()
{
    printf("student %d is study\n", gettid());
    // sleep(1);
}

void thinking(int x)
{
    printf("哲学家 %d 在思考\n", x);
}

void eating(int x)
{
    printf("哲学家 %d 在就餐\n", x);
}