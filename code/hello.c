#include <stdio.h>
#include <assert.h>
#include <pthread.h>

void *print(void *arg)
{
    printf("Hello World\n");
    return NULL;
}
int main()
{
    pthread_t p;
    int rc = pthread_create(&p, NULL, print, NULL);
    assert(rc == 0);
    pthread_join(p, NULL);
    assert(rc == 0);
    return 0;
}