#include <iostream> // 引入标准输入输出流
#include <unistd.h> // 引入UNIX标准函数定义
#include <sys/wait.h> // 引入进程控制相关函数


//g++ -std=c++11 fork_exo2.cpp -o fork_exo2   ./fork_exo2
int main () {
    const int N = 3; // 定义常量N，表示外循环的次数
    std::cout << "main pid=" << getpid() << std::endl; // 输出主进程的PID

    int nb_fils = 0; // 子进程计数器
    for (int i=1, j=N; i<=N && j==N; i++ ) {
        if (fork() != 0) { // 父进程中fork()返回子进程的PID，不为0
            nb_fils++; // 在父进程中增加子进程计数器
            break; // 父进程跳出循环，不再创建新的子进程
        } else { // 子进程中fork()返回0
            nb_fils = 0; // 子进程的子进程计数器重置为0
        }
        // 输出当前进程的PID和父进程的PID，以及循环计数器i和j
        std::cout << "pid=" << getpid() << " ppid=" << getppid() << " i:" << i << " j:" << j << std::endl;
        for (int k=1; k<=i && j==N; k++) {
            if (fork() == 0) { // 在子进程中，fork()返回0
                nb_fils = 0; // 新子进程的子进程计数器重置为0
                j=0; // 改变j的值，以便跳出外循环
                // 输出当前进程的PID和父进程的PID，以及循环计数器k和j
                std::cout << "pid=" << getpid() << " ppid=" << getppid() << " k:" << k << " j:" << j << std::endl;
                break; // 子进程跳出循环
            } else {
                nb_fils++; // 在父进程中，如果fork()创建子进程成功，增加计数器
            }
        }
    }
    // 父进程等待所有子进程结束
    for (int i=0; i < nb_fils; i++) {
        int status;
        int pid = wait(&status); // 等待子进程结束并获取子进程的PID
        // 输出检测到子进程结束的信息
        std::cout << "detection par " << getpid() << " de fin du fils de pid=" << pid << std::endl;
    }
    // 输出进程结束的信息
    std::cout << "Fin du processus " << getpid() << std::endl;
    return 0;
}
