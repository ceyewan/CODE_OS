#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char const* argv[])
{
    // 期望参数数量为 2
    if (argc != 2) {
        fprintf(2, "error\n");
        exit(1);
    }
    // 将第二个参数转换为整数类型
    sleep(atoi(argv[1]));
    exit(0);
}