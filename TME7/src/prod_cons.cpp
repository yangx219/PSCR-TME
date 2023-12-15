#include "Stack.h"  
#include <iostream> 
#include <unistd.h> // Pour les appels système POSIX
#include <sys/wait.h> // Pour attendre la fin des processus
#include <vector>    
#include <signal.h>  // Pour gérer les signaux

//使用共享内存和进程间通信实现生产者消费者模型
using namespace std;
using namespace pr;

void handle_sigint(int sig) {
    // Supprime le sémaphore du producteur
    sem_unlink(SEM_PRODUCER_NAME); 
    // Supprime le sémaphore du consommateur
    sem_unlink(SEM_CONSUMER_NAME); 
    exit(0); 
}
//producteur est un processus qui lit des caractères depuis l'entrée standard et les ajoute à la pile
void producteur(Stack<char> *stack) {
    cout << "Le producteur a démarré. Entrez des caractères:" << endl;
    char c;
    while (cin.get(c)) {
        if (c != '\n') {  // 忽略换行符
            stack->push(c);  // Ajoute le caractère à la pile
            cout << "Caractère produit: " << c << endl;
        }
    }
}

//consommateur est un processus qui retire des caractères de la pile et les affiche sur la sortie standard
void consommateur(Stack<char> *stack) {
    cout << "Le consommateur a démarré." << endl;
    while (true) {
        char c = stack->pop(); // Retire un caractère de la pile
        cout << "Caractère consommé: " << c << endl;
    }
}

int main() {
    signal(SIGINT, handle_sigint); // Associe le signal SIGINT au gestionnaire

    // 分配共享内存
    void *shared_memory = mmap(nullptr, sizeof(Stack<char>),
                               PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,
                               -1, 0);
    Stack<char> *s = new(shared_memory) Stack<char>(); // 在共享内存中构造栈

    // Crée un processus enfant
    pid_t pp = fork(); 
    // Termine le processus enfant du producteur
    if (pp == 0) {
        producteur(s);
        return 0; 
    }
    // Crée un autre processus enfant
    pid_t pc = fork(); 
    if (pc == 0) {
        consommateur(s);
        return 0; 
    }
     // Attend la fin du processus du producteur
    wait(nullptr);
    // Attend la fin du processus du consommateur
    wait(nullptr); 

    munmap(shared_memory, sizeof(Stack<char>)); // 释放共享内存
    return 0; 
}
