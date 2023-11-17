#include <sys/types.h> // 包含数据类型，如pid_t
#include <sys/wait.h> // 包含进程控制的函数，如wait()
#include <unistd.h> // 提供对POSIX操作系统API的访问
#include <time.h> // 提供标准时间库函数
#include <cstdio> // 提供标准输入输出库函数
#include <cstdlib> // 提供标准库函数，如exit()
#include <csignal> // 提供信号处理库函数
#include "rsleep.h" // 可能是提供随机休眠功能的自定义库

//g++ -o combat combat.cpp rsleep.cpp
// 生命值，全局变量，每个进程初始有3点生命值
int PV = 3;

// 信号处理函数，用于攻击阶段
void handler(int sig) {
    if (sig == SIGUSR1) {
        PV--; // 收到信号时减少生命值
        printf("Attaque recue par %d; PV restants %d\n", getpid(), PV);
        if (PV == 0) { // 生命值为0时，进程结束
            printf("Plus de vie pour %d; mort du processus.\n", getpid());
            exit(1); // 进程退出，返回1表示死亡
        }
    }
}

// 攻击函数，向对手发送SIGUSR1信号
void attaque(pid_t adversaire) {
    signal(SIGUSR1, handler); // 设置信号处理函数
    if (kill(adversaire, SIGUSR1) < 0) { // 发送信号给对手
        printf("Detection de Mort de l'adversaire de pid=%d \n", adversaire);
        exit(0); // 如果发送信号失败，表示对手已经死亡，当前进程也退出
    }
    randsleep(); // 攻击后随机休眠
}

// 防御函数，忽略SIGUSR1信号
void defense() {
    signal(SIGUSR1, SIG_IGN); // 忽略SIGUSR1信号
    randsleep(); // 防御时随机休眠
}

// 战斗函数，不断循环执行防御和攻击
void combat(pid_t adversaire) {
    while (1) { // 无限循环
        defense(); // 首先进入防御状态
        attaque(adversaire); // 然后进入攻击状态
    }
}

int main() {
    pid_t pere = getpid(); // 获取当前进程的PID
    pid_t pid = fork(); // 创建子进程
    srand(pid); // 以pid作为随机数种子
    if (pid == 0) { // 子进程
        combat(pere); // 子进程开始战斗，以父进程为对手
    } else { // 父进程
        combat(pid); // 父进程开始战斗，以子进程为对手
    }
    return 0; // 程序正常退出
}
