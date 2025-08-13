#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/*判断path是否指向所需的文件名*/
int match(char *path, char *name)
{
    char *p;
    // 查找最后一个'/'后的第一个字符
    for (p = path + strlen(path); p >= path && *p != '/'; p--){} //空语句，用于定位p
    p++;
    if (strcmp(p, name) == 0) 
        return 1;
    else
        return 0;
}

void find(char *path, char *name)
{
    char buf[512], *p;//字符串储存路径，p定位当前末尾
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_FILE:
        if (match(path, name))
        {
            printf("%s\n", path);
        }
        break;

    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
        {
            printf("ls: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);//定位到最后一个位置
        // 给path后面添加 /
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            // 如果读取成功, 一直都会在while loop中
            if (de.inum == 0)
                continue;
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            // 把 de.name 拷贝到buf中
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;//添加尾零
            find(buf, name); //递归的进行查找
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("input param is %d and it is less then 2\n", argc);
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}