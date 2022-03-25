#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void my_find(char const* path, char const* name)
{
    int fd;
    char buf[512], *p;
    struct dirent de;
    struct stat st;
    // 无法打开
    if ((fd = open(path, 0)) == 0) {
        fprintf(2, "error\n");
        exit(1);
    }
    // 将文件信息写入 st
    if (fstat(fd, &st) < 0) {
        fprintf(2, "error\n");
        close(fd);
        exit(1);
    }
    switch (st.type) {
    // 传入的参数不是文件夹，肯定是不行的
    case T_FILE:
        fprintf(2, "error\n");
        exit(1);
    case T_DIR:
        // 当前的路径太深了
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
            printf("error\n");
            exit(1);
        }
        // 复制一个新路径
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        // 遍历当前路径的每个文件（夹）
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            // 不能是 "." 和 ".."
            if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            // 复制文件（夹）名，如果是将字符串 20-30
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0) {
                printf("error\n");
                exit(1);
            }
            switch (st.type) {
            // 新形成的路径是文件
            case T_FILE:
                if (strcmp(de.name, name) == 0) {
                    printf("%s\n", buf);
                }
                break;
            case T_DIR:
                my_find(buf, name);
                break;
            }
        }
    }
}
int main(int argc, char const* argv[])
{
    // 期望参数是 3 个
    if (argc != 3) {
        fprintf(2, "Usage: find dir file\n");
        exit(1);
    }
    my_find(argv[1], argv[2]);
    exit(0);
}