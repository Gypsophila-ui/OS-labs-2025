#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){
    int parentPipe[2],childPipe[2];
    pipe(parentPipe);// Calling pipe creates a parent pipe
    pipe(childPipe); // Calling pipe creates a child pipe

    int pid = fork(); //创建子进程
    if (pid < 0) {
        fprintf(2, "Fork failed\n");
        exit(1);
    }

    if (pid == 0) {  // 子进程
        char byte;
        
        // 关闭不需要的管道端
        close(parentPipe[1]);  // 关闭父进程写端
        close(childPipe[0]);  // 关闭子进程读端

        // 等待父进程发来的字节
        if (read(parentPipe[0], &byte, 1) != 1) {
            fprintf(2, "Failed to read from parent\n");
            exit(1);
        }
        
        // 打印接收信息
        printf("%d: received ping\n", getpid());
        
        // 向父进程发送响应字节
        if (write(childPipe[1], &byte, 1) != 1) {
            fprintf(2, "Failed to write to parent\n");
            exit(1);
        }
        
        // 清理资源
        close(parentPipe[0]);
        close(childPipe[1]);
        exit(0);
    } 
    else {  // 父进程
        char byte = 'X';  // 任意字节内容
        
        // 关闭不需要的管道端
        close(parentPipe[0]);  // 关闭父进程读端
        close(childPipe[1]);  // 关闭子进程写端

        // 向子进程发送字节
        if (write(parentPipe[1], &byte, 1) != 1) {
            fprintf(2, "Failed to write to child\n");
            exit(1);
        }
        
        // 等待子进程响应
        if (read(childPipe[0], &byte, 1) != 1) {
            fprintf(2, "Failed to read from child\n");
            exit(1);
        }
        
        // 打印接收信息
        printf("%d: received pong\n", getpid());
        
        // 清理资源并等待子进程退出
        close(parentPipe[1]);
        close(childPipe[0]);
        wait(0);  // 等待子进程结束
        exit(0);
    }
}