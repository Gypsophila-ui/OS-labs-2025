#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 筛进程函数
void sieve(int read_end) {
    int prime;
    
    // 读取第一个数字作为素数
    if (read(read_end, &prime, sizeof(prime)) <= 0) {
        close(read_end);
        exit(0);
    }
    
    printf("prime %d\n", prime);
    
    // 创建与右邻居通信的管道
    int next_pipe[2];
    pipe(next_pipe) ;
    
    int pid = fork();
    
    if (pid == 0) { // 子进程（右邻居）
        close(next_pipe[1]);  // 关闭写端
        close(read_end);      // 关闭左邻居的读端
        sieve(next_pipe[0]);  // 递归创建下一个筛进程
        exit(0);             // 永远不会执行到这里
    } 
    else { // 父进程（当前筛进程）
        close(next_pipe[0]);  // 关闭读端（不需要从右邻居读取）
        
        int num;
        while (read(read_end, &num, sizeof(num)) > 0) {
            // 过滤掉能被当前素数整除的数字
            if (num % prime != 0) {
                write(next_pipe[1], &num, sizeof(num));
            }
        }
        
        // 关闭所有文件描述符并等待子进程
        close(read_end);
        close(next_pipe[1]);
        int status;
        wait(&status);
    }
}

int main() {
    // 创建初始管道
    int first_pipe[2];
    pipe(first_pipe);
    
    int pid = fork();
    
    if (pid == 0) { // 子进程（第一个筛进程）
        close(first_pipe[1]);  // 关闭写端
        sieve(first_pipe[0]);  // 开始筛过程
        exit(0);
    } 
    else { // 父进程（数字生成器）
        close(first_pipe[0]);  // 关闭读端
        
        // 生成2到35的数字
        for (int i = 2; i <= 35; i++) {
            write(first_pipe[1], &i, sizeof(i));
        }
        
        // 关闭写端表示输入结束
        close(first_pipe[1]);
        int status;
        wait(&status);  // 等待整个管道结束
    }
    
    return 0;
}