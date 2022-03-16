### 插叙：进程API

1. 编写一个调用 fork()的程序。在调用 fork()之前，让主进程访问一个变量（例如 x） 并将其值设置为某个值（例如 100）。子进程中的变量有什么值？当子进程和父进程都改变 x 的值，变量会发生什么？

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int x = 1;
    printf("initial: x = %d\n", x);
    int rc = fork();
    if (rc < 0)
    {
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    else if (rc == 0)
    {
        printf("child x = %d\n", x);
        x++;
        printf("after change: child x = %d\n", x);
    }
    else
    {
        printf("parent x = %d\n", x);
        x++;
        printf("after change: parent x = %d\n", x);
    }
    return 0;
}
```

运行结果（子进程相当于复制了一份，两个进程之间没有影响）

![image-20220302235803849](https://gitee.com/ceyewan/pic/raw/master/images/image-20220302235803849.png)

2．编写一个打开文件的程序（使用 open()系统调用），然后调用 fork()创建一个新进程。 子进程和父进程都可以访问 open()返回的文件描述符吗？当它们并发（即同谁）写入文件时， 会发生什么？

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int fd = open("./tmp/file", O_WRONLY | O_CREAT | O_TRUNC);
    assert(fd > -1);
    int rc = fork();
    if (rc < 0)
    {
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    else if (rc == 0)
    {
        printf("child pid: rd = %d\n", fd);
        int rw = write(fd, "child pid\n", 11);
    }
    else
    {
        printf("parent pid: rd = %d\n", fd);
        int rw = write(fd, "parent pid\n", 12);
        wait(NULL);
    }
    return 0;
}
```

运行结果（我估计并发可能会出错）：

![image-20220303000920021](https://gitee.com/ceyewan/pic/raw/master/images/image-20220303000920021.png)

3．使用 fork()编写另一个程序。子进程应打印“hello”，父进程应打印“goodbye”。你 应该尝试确保子进程始终先打印。你能否不在父进程调用 wait()而做到这一点呢？

```c
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
 
int main()
{
	int rc = vfork();
	if (rc < 0) {
		fprintf(stderr, "fork failed");
		exit(1);
	} else if (rc == 0) {
		printf("hello\n");
		exit(1);
	} else {
		printf("goodbye\n");
	}
	return 0;
}
```

> 1.  fork  （）：子进程拷贝父进程的数据段，代码段
>     vfork （）：子进程与父进程共享数据段
> 2.  fork （）父子进程的执行次序不确定
>     vfork( ) 保证子进程先运行，在调用exec 或exit 之前与父进程数据是共享的,在它调用exec
>      或exit 之后父进程才可能被调度运行。
> 3.  vfork （）保证子进程先运行，在它调用exec 或exit 之后父进程才可能被调度运行。如果在
>    调用这两个函数之前子进程依赖于父进程的进一步动作，则会导致死锁。

5．现在编写一个程序，在父进程中使用 wait()，等待子进程完成。wait()返回什么？如 果你在子进程中使用 wait()会发生什么？

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int rc = fork();
    if (rc < 0)
    {
        fprintf(stderr, "fork failed");
        exit(1);
    }
    else if (rc == 0)
    {
        int wc = wait(NULL);
        printf("child pid: %d, wc = %d\n", (int)getpid(), wc);
    }
    else
    {
        int wc = wait(NULL);
        printf("parent pid: %d, wc = %d\n", (int)getpid(), wc);
    }
    return 0;
}
```

运行结果：

![image-20220303002457612](https://gitee.com/ceyewan/pic/raw/master/images/image-20220303002457612.png)6．对前一个程序稍作修改，这次使用 waitpid()而不是 wait()，什么时候 waitpid()会有用？

> ```
> pid_t waitpid(pid_t pid,int *status,int options)
> ```
>
> 1. pid>0时，只**等待进程ID等于pid的子进程**，不管其它已经有多少子进程运行结束退出了，只要指定的子进程还没有结束，waitpid就会一直等下去。
> 2. **pid=-1时，等待任何一个子进程退出，没有任何限制，此时waitpid和wait的作用一模一样。**
> 3. pid=0时，等待同一个进程组中的任何子进程，如果子进程已经加入了别的进程组，waitpid不会对它做任何理睬。
> 4. **pid<-1时，等待一个指定进程组中的任何子进程，这个进程组的ID等于pid的绝对值。**

```c

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int pi[2];
    int p = pipe(pi);
    if (p < 0)
    {
        fprintf(stderr, "pipe failed");
        exit(1);
    }
    int i = 0;
    int rc[2];
    char buf[256];
    for (i = 0; i < 2; ++i)
    {
        rc[i] = fork();
        if (rc[i] < 0)
        {
            fprintf(stderr, "fork failed");
            exit(1);
        }
        else if (rc[i] == 0)
        {
            switch (i)
            {
            case 0:
                dup2(pi[1], STDOUT_FILENO);
                puts("child input");
                break;
            case 1:
                dup2(pi[0], STDIN_FILENO);
                gets(buf);
                printf("child0 out (%s) in the child1\n", buf);
                return 2;
            }
            break;
        }
    }
    waitpid(rc[0], NULL, 0);
    waitpid(rc[1], NULL, 0);
    return 0;
}
```

