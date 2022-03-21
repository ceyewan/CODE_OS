#### 内存类型

内存包括栈内存（自动内存）和堆内存。

#### malloc 调用

很熟悉了

#### free调用

很熟悉了

#### 常见错误

1. 忘记分配内存

```c
char *src = "hello"; 
char *dst;		  // oops! unallocated
strcpy(dst, src); // segfault and die
// 用 strdup ，让生活更轻松
```

2. 没有分配足够的内存

```c
char *a = "hello";
char *b = (char *)malloc(sizeof(char) * 5);
strcpy(b, a);
```

3. 忘记初始化分配的内存
4. 忘记释放内存

内存泄漏问题

5. 在用完之前释放内存
6. 反复释放内存
7. 错误的调用free

一般短的程序，我们不释放内存问题不大，因为还有操作系统呢，我们的程序运行结束后，操作系统会收回所有的内存。但是如果是一个长时间运行的程序，如果不释放的话，问题就会很大了。

#### 底层操作系统的支持

malloc()和 free()它们不是系统调用，而是库调用。因此，malloc 库管理虚拟地址空间内的空间，但是它本身是建立在一些系统调用之上的，这些系统调用会进入操作系统，来请求更多内存或者将一 些内容释放回系统。

最后，你还可以通过 mmap()调用从操作系统获取内存。通过传入正确的参数，mmap() 可以在程序中创建一个匿名（anonymous）内存区域——这个区域不与任何特定文件相关联， 而是与交换空间（swap space）相关联。

#### 其他调用

calloc 和 realloc 等。

#### 作业

1．首先，编写一个名为 null.c 的简单程序，它创建一个指向整数的指针，将其设置为 NULL，然后尝试对其进行释放内存操作。把它编译成一个名为 null 的可执行文件。当你运行这个程序时会发生什么？（无事发生）

```c
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int *x = NULL;
    free(x);
    return 0;
}
```

2．接下来，编译该程序，其中包含符号信息（使用-g 标志）。这样做可以将更多信息放入可执行文件中，使调试器可以访问有关变量名称等的更多有用信息。通过输入 gdb null， 在调试器下运行该程序，然后，一旦 gdb 运行，输入 run。gdb 显示什么信息？（正常）

![image-20220321205035752](https://gitee.com/ceyewan/pic/raw/master/images/image-20220321205035752.png)

3．最后，对这个程序使用 valgrind 工具。我们将使用属于 valgrind 的 memcheck 工具 来分析发生的情况。输入以下命令来运行程序：valgrind --leak-check=yes null。当你运行它 时会发生什么？你能解释工具的输出吗？（系统应该处理好了）

![image-20220321205649068](https://gitee.com/ceyewan/pic/raw/master/images/image-20220321205649068.png)

4．编写一个使用 malloc()来分配内存的简单程序，但在退出之前忘记释放它。这个程序运行时会发生什么？你可以用 gdb 来查找它的任何问题吗？用 valgrind 呢（再次使用 --leak-check=yes 标志）？

```c
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int *x = (int *)malloc(sizeof(int));
    return 0;
}
```

gdb：无事发生

valgrind ：lost 4 bytes，内存泄漏 4 字节。

![image-20220321205939149](https://gitee.com/ceyewan/pic/raw/master/images/image-20220321205939149.png)

5．编写一个程序，使用 malloc 创建一个名为 data、大小为 100 的整数数组。然后，将 data[100]设置为 0。当你运行这个程序时会发生什么？当你使用 valgrind 运行这个程序时会 发生什么？程序是否正确？

```c
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int *data = (int *)malloc(sizeof(int) * 100);
    data[100] = 0;
    free(data);
    return 0;
}
```

![image-20220321210513628](https://gitee.com/ceyewan/pic/raw/master/images/image-20220321210513628.png)

大小为 4 的无效写入，运行是没有问题的。

6．创建一个分配整数数组的程序（如上所述），释放它们，然后尝试打印数组中某个元素的值。程序会运行吗？当你使用 valgrind 时会发生什么？

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    int *data = (int *)malloc(sizeof(int) * 100);
    memset(data, 0xff, sizeof(int) * 100);
    printf("%d\n", data[0]);
    free(data);
    printf("%d\n", data[0]);
    return 0;
}
```

![image-20220321210917883](https://gitee.com/ceyewan/pic/raw/master/images/image-20220321210917883.png)

大小为 4 的无效读取，而且读到的并不是之前值了。

7．现在传递一个有趣的值来释放（例如，在上面分配的数组中间的一个指针）。 会发生什么？你是否需要工具来找到这种类型的问题？

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    int *data = (int *)malloc(sizeof(int) * 100);
    memset(data, 0xff, sizeof(int) * 100);
    printf("%d\n", data[0]);
    free(&data[1]);
    printf("%d\n", data[0]);
    return 0;
}
```

运行时出错：（free只能处理 malloc 返回的指针）

![image-20220321211230250](https://gitee.com/ceyewan/pic/raw/master/images/image-20220321211230250.png)

8．尝试一些其他接口来分配内存。例如，创建一个简单的向量似的数据结构，以及使用 realloc()来管理向量的相关函数。使用数组来存储向量元素。当用户在向量中添加条目时， 请使用realloc()为其分配更多空间。这样的向量表现如何？它与链表相比如何？使用valgrind 来帮助你发现错误。

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    int *vector = (int *)malloc(sizeof(int) * 2);
    int VectorSize = 2;
    int p = 0;
    for (int i = 0; i < 100; i++)
    {
        vector[p++] = i;
        if (p == VectorSize)
        {
            VectorSize *= 2;
            vector = realloc(vector, sizeof(int) * VectorSize);
        }
    }
    for (int i = 0; i < 100; i++)
    {
        printf("%d\t", vector[i]);
    }
    printf("\n");
    return 0;
}
```

和链表相比，不方便释放内存，删除元素，增加元素大概类似。反正要实现邻接表我肯定是用这种方法而不是链表。是 C++ 的 vector 不香吗，是 python 的列表不香吗？

