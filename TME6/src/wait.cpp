
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
//g++ -std=c++11 wait.cpp rsleep.cpp -o wait
//    ./wait

// V1
//等待指定的进程ID（pid）的子进程结束，attendre jusqu'à ce que le processus d'identifiant pid se termine
pid_t wait_till_pid(pid_t pid) {
    while (true) {
        pid_t p = wait(nullptr);
        if (p == -1 || p == pid) {
            return p;
        }
    }
}

// V2: signal
//增加了超时功能, attendre jusqu'à ce que le processus d'identifiant pid se termine 
//ou jusqu'à ce que le délai d'attente soit écoulé
pid_t wait_till_pid(pid_t pid, int sec) {
    static bool timeout = false;
    signal(SIGALRM,[](int) { std::cout << "received SIGALRM" << std::endl; timeout = true; });
    signal(SIGCHLD, [](int) { std::cout << "received SIGCHLD" << std::endl; });
    alarm(sec);

    sigset_t set;
    sigfillset(&set);
    sigdelset(&set, SIGALRM);
    sigdelset(&set, SIGCHLD);

    while (true) {
        std::cout << "waiting..." << std::endl;
        sigsuspend(&set);
        if (timeout) {
            std::cout << "Alarm wins" << std::endl;
            return -1;
        } else {
            pid_t p = wait(nullptr);
            std::cout << "wait answered " << p << std::endl;
            if (p == pid) {
                alarm(0);
                return p;
            }
            if (p == -1) {
                return p;
            }
        }
    }
}

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        // if more than the timeout it should die
        sleep(5);
    } else {
        signal(SIGINT, [](int) {});
        pid_t res = wait_till_pid(pid, 3);
        std::cout << "wait gave: " << res << std::endl;
    }
}