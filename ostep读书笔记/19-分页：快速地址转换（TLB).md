如何加速地址转换？我们要增加所谓的地址转换旁路缓冲存储器（translation-lookaside buffer）也就是 TLB，它就是频繁发生的虚拟到物理地址转换的硬件缓存（cache）。因 此，更好的名称应该是地址转换缓存（address-translation cache）。

#### TLB 的基本算法

1. 从虚拟地址提取页号
2. 检查 TLB 是否有该 VPN 的映射
3. 如果有就 TLB 命中，从 TLB 中取出页帧号，得到物理地址。
4. 如果 TLB 未命中，访问页表，将转换写入 TLB 。然后重复尝试在 TLB 中读取 VPN 映射。

#### 示例：访问数组

很熟悉了，因为时间和空间的局限性，所以命中率还是很高的。

提示：尽可能利用缓存

#### 谁来处理 TLB 未命中

以前的硬件有复杂的指令集，造硬件的人不太相信那些搞操作系统的人。因此，硬件全权处理 TLB 未命中。为了做到这一点，硬件必须知道页表在内存中的确切位置（通过页表基址寄存器），以及页表的确切格式。发生未命中时， 硬件会“遍历”页表，找到正确的页表项，取出想要的转换映射，用它更新 TLB，并重试 该指令。

现代的体系结构，发生 TLB 未命中时，硬件系统会抛出一个异常，这会暂停当前的指令流，将特权级提升至内核模式，跳转至陷阱处理程序（trap handler），这个陷阱处理程序是操作系统的一段代码，用于处理 TLB 未命中。 这段代码在运行时，会查找页表中的转换映射，然后用特别的“特权”指令更新 TLB，并从陷阱返回。此时，硬件会重试该指令（导致 TLB 命中）。

这里从陷阱返回和之前有点不一样，之前是在后面那一句开始执行，这里是在这一句重复执行。

在运行 TLB 未命中处理代码时，操作系统需要格外小心避免引起 TLB 未命中的无限递归。（因为陷阱处理程序如果也在虚拟内存中，那么也会需要页表。因此，我们可以把这个程序直接放到物理内存中。

#### TLB 的内容

TLB 也并不一定是一对一的，也可能一对多。TLB被称为全相联的（fully-associative）缓存，硬件并行查找这些项，查看是否匹配。

> TLB的有效位!=页表的有效位
>
> 如果一个页表项（PTE）被标记为无效，就意味着该页并没有被进程申请使用，正常运行的程序不应该访问该地址。
>
> TLB 的有效位不同，只是指出 TLB 项是不是有效的地址映射。

#### 上下文切换时对 TLB 的处理

TLB 中包含的虚拟到物理的地址映射只对当前进程有效，对其他进程是没有意义的。所以 在发生进程切换时，硬件或操作系统（或二者）必须注意确保即将运行的进程不要误读了之前进程的地址映射。

可能进程1的 VPN 10 被映射到 100 ，进程2 的 VPN10 被映射到 170 。因此，还需要让 TLB 支持跨多线程的虚拟化。

方法一：直接清空。

方法二：在 TLB 中增加一个地址空间标识符（ASID)，只有当 VPN 和 ASID 同时匹配才算是命中。

当然也有可能不同的进程不同的虚拟空间映射到相同的物理空间（内存共享）

#### TLB 替换策略

TLB 满了，那么插入一个新的需要替换掉一个旧的。我们可以用 LRU 和 LFU 等。

#### 实际系统的 TLB 表项

![image-20220323165820397](https://gitee.com/ceyewan/pic/raw/master/images/image-20220323165820397.png)
![image-20220323165841004](https://gitee.com/ceyewan/pic/raw/master/images/image-20220323165841004.png)

#### 作业

1．为了计时，可能需要一个计时器，例如 gettimeofday()提供的。这种计时器的精度如何？操作要花多少时间，才能让你对它精确计时？（这有助于确定需要循环多少次，反复访问内存页，才能对它成功计时。）

```c
头文件：#include <sys/time.h>    #include <unistd.h>
timeval 结构定义为：
struct timeval{
    long tv_sec;  //秒
    long tv_usec;  //微秒
};

timezone 结构定义为：
struct timezone
{
    int tz_minuteswest;  //和Greenwich 时间差了多少分钟
    int tz_dsttime;  //日光节约时间的状态
};
struct timeval tv;
struct timezone tz;
gettimeofday (&tv, &tz);
```

那么我们用微秒那个数据，也就是说能精确到微秒。

2．写一个程序，命名为 tlb.c，大体测算一下每个页的平均访问时间。程序的输入参数有：页的数目和尝试的次数。

```c
#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <stdlib.h>
#define PAGESIZE 4096
// getconf PAGE_SIZE h
int main(int argc, char *argv[])
{
    assert(argc == 3);
    int page_num = atoi(argv[1]);
    long long max = page_num * PAGESIZE;
    char *temp = (char *)malloc(sizeof(char) * max);
    int try_num = atoi(argv[2]);
    struct timeval tv;
    struct timezone tz;
    char a;
    gettimeofday(&tv, &tz);
    long long start = tv.tv_usec;
    for (long long i = 0; i < try_num; i++)
    {
        a = temp[i * 1009 % max];
    }
    gettimeofday(&tv, &tz);
    long long end = tv.tv_usec;
    printf("平均耗时为：%f 微秒\n", (double)(end - start) / try_num);
}
```

3．用你喜欢的脚本语言（csh、Python 等）写一段脚本来运行这个程序，当访问页面从 1 增长到几千，也许每次迭代都乘 2。在不同的机器上运行这段脚本，同时收集相应数据。 需要试多少次才能获得可信的测量结果？

```python
#!/usr/bin/env python
import os

i = 1
while i < 2000 :
	print ('page num ' + str(i))
	val = os.system('./tlb ' + str(i) + ' ' + str(10000))
	i = i * 2
```

![image-20220323191448719](https://gitee.com/ceyewan/pic/raw/master/images/image-20220323191448719.png)

页面越多，平均耗时越长，因为未命中的次数增加了。

```python
#!/usr/bin/env python
import os

i = 64
while i < 200000 :
	print ('try num ' + str(i))
	val = os.system('./tlb ' + str(4) + ' ' + str(i))
	i = i * 2
```

![image-20220323191756107](https://gitee.com/ceyewan/pic/raw/master/images/image-20220323191756107.png)

页面不变，访问次数增加，平均耗时减少。因为均摊了未命中。

6．还有一个需要注意的地方，今天的计算机系统大多有多个 CPU，每个 CPU 当然有自己的 TLB 结构。为了得到准确的测量数据，我们需要只在一个 CPU 上运行程序，避免调度器把进程从一个CPU调度到另一个去运行。如何做到？（提示：在 Google 上搜索“pinning a thread”相关的信息）如果没有这样做，代码从一个 CPU 移到了另一个，会发生什么情况？

```c
// 来自：https://yangzebin001.github.io/2020/08/24/Pinning%20a%20thread%20%E2%80%94%E2%80%94%20%E5%A6%82%E4%BD%95%E8%AE%A9%E7%BA%BF%E7%A8%8B%E8%BF%90%E8%A1%8C%E5%9C%A8%E7%89%B9%E5%AE%9A%E7%9A%84CPU%E4%B8%8A/
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define handle_error_en(en, msg) \
    do                           \
    {                            \
        errno = en;              \
        perror(msg);             \
        exit(EXIT_FAILURE);      \
    } while (0)

int main(int argc, char *argv[])
{
    int s, j;
    cpu_set_t cpuset;
    pthread_t thread;

    thread = pthread_self(); // 获取当前运行的线程数据结构

    /* Set affinity mask to include CPUs 0 to 7 */
    CPU_ZERO(&cpuset); // 将cpuset结构清零
    // 将0-7号CPU全部加入cpuset中（如果只想指定特定的cpu，运行循环中那一行即可）
    // 例如将线程运行在3号cpu上：CPU_SET(3, &cpuset);
    for (j = 0; j < 8; j++)
        CPU_SET(j, &cpuset); // 设置j号cpu到cpuset中。

    // set CPU affinity of a thread
    s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (s != 0)
        handle_error_en(s, "pthread_setaffinity_np");

    /* Check the actual affinity mask assigned to the thread */
    s = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (s != 0)
        handle_error_en(s, "pthread_getaffinity_np");

    printf("Set returned by pthread_getaffinity_np() contained:\n");
    for (j = 0; j < CPU_SETSIZE; j++)
        if (CPU_ISSET(j, &cpuset))
            printf("    CPU %d\n", j);

    exit(EXIT_SUCCESS);
}
```

