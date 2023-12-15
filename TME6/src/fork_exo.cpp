#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
//g++ -std=c++11 fork_exo.cpp -o fork_exo
//    ./fork_exo
int main() {
    const int N = 3;
    std::cout << "main pid=" << getpid() << std::endl;

    int nbfils = 0;
    for (int i = 1, j = N; i <= N && j == N; i++) {
        if (fork() != 0) {// 在父进程中执行
            nbfils++;
            break;
        } else {
            nbfils = 0;// 在子进程中执行。子进程将 nbfils 重置为0
        }

    std::cout << "pid=" << getpid() << " ppid=" << getppid()
              				<< " i:j " << i<<":" << j << std::endl;

    for (int k = 1; k <= i && j == N; k++) {
        if (fork() == 0) {
            nbfils = 0;
            j = 0;
            std::cout << "pid=" << getpid() << " ppid=" << getppid()
                      << " k:j " << k << " :" << j << std::endl;
        } else {
            nbfils++;
        }
    }
	}

    for (int i = 0; i < nbfils; ++i) {
        int status;
        int pid = wait(&status);
        std::cout << "detection par " << getpid() << " de fin du fils de pid=" << pid << std::endl;
    }

    std::cout << "Fin du processus " << getpid() << std::endl;

    return 0;
	
}
