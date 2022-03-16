#### 线程创建

```c
#include <pthread.h>

int pthread_create(
    pthread_t *thread,
    const pthread_attr_t *attr,
    void *(*start_routine)(void *),
    void *arg);
```

- 第一个参数 thread 是指 向 pthread_t 结构类型的指针，我们将利用这个结构与该线程交互，因此需要将它传入 pthread_create()，以便将它初始化

- 第二个参数 attr 用于指定该线程可能具有的任何属性，一个属性通过单独调用 pthread_attr_init()来初始化

- 第三个参数一个函数名称（start_routine），它被传入一个类型为 void *的参数（start_routine 后面的括号表明了这 一点），并且它返回一个 void *类型的值（即一个 void 指针）

- 第四个参数 arg 就是要传递给线程开始执行的函数的参数

```c
#include<stdio.h>
#include<pthread.h>

typedef struct {
    int a;
    int b;
} myarg_t;

void *mypthread(void *arg) {
    myarg_t *m = (myarg_t *)arg;
    printf("%d %d\n", m->a, m->b);
    return NULL;
}

int main(int argc, char *argv) {
    pthread_t p;
    int rc;
    myarg_t args;
    args.a = 10;
    args.b = 20;
    rc = pthread_create(&p, NULL, mypthread, args);
    ...
}
```

#### 线程完成

```c
int pthread_join(
    pthread_t thread, 
    void **value_ptr);
```

- 第一个参数是 pthread_t 类型，用于指定要等待的线程。
- 第二个参数是一个指针，指向你希望得到的返回值。因为函数可以返回任何东西，所 以它被定义为返回一个指向 void 的指针。因为 pthread_join()函数改变了传入参数的值，所 以你需要传入一个指向该值的指针，而不只是该值本身。

```c
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>

typedef struct myarg_t
{
    int a;
    int b;
} myarg_t;

typedef struct myret_t
{
    int x;
    int y;
} myret_t;

void *mypthread(void *arg)
{
    myarg_t *m = (myarg_t *)arg;
    printf("%d %d\n", m->a, m->b);
    myret_t *r = malloc(sizeof(myret_t));
    // myret_t r; // ALLOCATED ON STACK: BAD!
    // 栈区会回收
    r->x = 1;
    r->y = 2;
    return (void *)r;
}

int main(int argc, char *argv)
{
    pthread_t p;
    int rc;
    myret_t *m;
    myarg_t args;
    args.a = 10;
    args.b = 20;
    rc = pthread_create(&p, NULL, mypthread, &args);
    // 如果成功创建线程，pthread_create() 函数返回数字 0，反之返回非零值。各个非零值都对应着不同的宏，指明创建失败的原因
    assert(rc == 0);
    pthread_join(p, (void **)&m);
    printf("returned %d %d\n", m->x, m->y);
    return 0;
}
```

#### 锁

```c
int pthread_mutex_lock(pthread_mutex_t *mutex); 
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

合理猜想锁的工作原理：

```c
pthread_mutex_t lock; // 锁变量

pthread_mutex_lock(&lock); // 上锁，一直重复，直到上锁
x = x + 1; // or whatever your critical section is
pthread_mutex_unlock(&lock); // 解锁
```

初始化锁：

```c
// 方法一
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// 方法二
// 参数attr指定了新建互斥锁的属性，为空表示默认
int rc = pthread_mutex_init(&lock, NULL);
assert(rc == 0); // always check success!
```

注销一个互斥锁：

```c
int pthread_mutex_destroy(pthread_mutex_t *mutex)
```

另外两个获取锁的调用：

```c
// 锁已被占用则失败
int pthread_mutex_trylock(pthread_mutex_t *mutex); 
// 超时获取锁后返回
int pthread_mutex_timedlock(pthread_mutex_t *mutex, struct timespec *abs_timeout);
```

#### 条件变量

当线程之间必须发生某种信号时，如果一个线程在等待另一个线程继 续执行某些操作，条件变量就很有用。避免自旋，消耗大量性能。

```c
// 使调用线程进入休眠状态，因此等待其他线程发出信号
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex); 
int pthread_cond_signal(pthread_cond_t *cond);
```

典型用法：

```c
// 初始化锁变量
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
// 初始化条件变量
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int main(int argc, char *argv)
{
    pthread_mutex_lock(&lock);
    while (ready == 0)
    {
        // 调用等待函数以便休眠，等待其他线程唤醒（ready 设置为 0）
        pthread_cond_wait(&cond, &lock);
    }
    pthread_mutex_unlock(&lock);
}
```

唤醒线程的代码：

```c
pthread_mutex_lock(&lock); 
ready = 1;
pthread_cond_signal(&cond); 
pthread_mutex_unlock(&lock);
```

> 等待调用除了使调用线程进入睡眠状态外，还会让调用者睡眠 时释放锁，不然别人怎么获得锁呢？
>
> 在被唤醒之后返回之前，pthread_cond_wait()会重新获取该锁，从而确保等待线程在等待序列开始时获取锁与结束时释放锁之间运行的任何时间，它持有锁。

#### 小结

构建多线程程序时需要注意的一些小细节：

- 保持简洁
- 让线程交互降到最少
- 初始化锁和条件变量
- 检查返回值
- 注意传给线程的参数和返回值
- 每个线程都有自己的栈
- 线程之间总是通过条件变量发送信号