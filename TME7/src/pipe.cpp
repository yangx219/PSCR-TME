#include <sys/types.h> 
#include <sys/wait.h>  
#include <unistd.h>    
#include <cstdlib>     
#include <cstdio>      
#include <cstring>     
#include <iostream>    
//g++ -o pipe pipe.cpp
//./pipe /bin/cat pipe.cpp \| /usr/bin/wc -l

/*第一个子进程执行 /bin/cat pipe.cpp，将 pipe.cpp 文件的内容发送到管道。
第二个子进程执行 /usr/bin/wc -l，从管道读取数据（即 pipe.cpp 的内容），并计算行数。*/

//使用管道（pipe）来实现两个命令之间的数据传输，comunicate between two commands using a pipe
// Déclare la fonction myexec qui exécutera une commande.
void myexec(int argc, char ** args) {
    std::cerr << "args :" ; // Affiche les arguments sur la sortie d'erreur standard.
    for (int i=0; i < argc ; i++) { // travers les arguments.
        if (args[i] != nullptr) { // Vérifie si l'argument n'est pas un pointeur nul.
            std::cerr << args[i] << " "; 
        } else {
            break; 
        }
    }
    std::cerr << std::endl; // Insère une nouvelle ligne.
    
    if (execv(args[0], args) == -1) { // Exécute la commande et vérifie l'échec.
        perror("execv"); 
        exit(3); 
    }
}


int main(int argc, char ** argv) {
    // Partitionne les arguments en deux ensembles pour deux commandes.
    char * args1[argc];
    char * args2[argc];
    
    memset(args1, 0, argc*sizeof(char*)); // Initialise le premier tableau d'arguments.
    memset(args2, 0, argc*sizeof(char*)); // Initialise le second tableau d'arguments.
    

    /***************pour separer les deux commandes******************/


    int arg = 1; // Commence à 1 pour ignorer le nom du programme.
    for ( ; arg < argc ; arg++) { // Parcourt les arguments.
        if (!strcmp(argv[arg], "|")) { // Vérifie la présence du symbole de pipe.
            arg++; // Incrémente pour passer le symbole de pipe.
            break; 
        } else {
            args1[arg-1] = argv[arg]; // Ajoute l'argument au premier tableau.
        }
    }


    // Continue avec le reste des arguments.
    for (int i=0; arg < argc ; i++, arg++) { 
        args2[i] = argv[arg]; // Ajoute l'argument au second tableau.
    }
    
    // Tableau pour les descripteurs de fichier de tube.
    int tubeDesc[2]; 
    // PID du processus enfant.
    pid_t pid_fils; 
    // Tente de créer un tube.
    if (pipe(tubeDesc) == -1) { 
         // Affiche le message d'erreur si le tube n'est pas créé.
        perror("pipe");
        exit(1); 
    }
    // Tente de créer un processus enfant.
    if ((pid_fils = fork()) == -1) { 
        // Affiche le message d'erreur si le fork échoue.
        perror("fork"); 
        exit(2); 
    }
    // Vérifie si dans le processus enfant.
    if (pid_fils == 0) { 
        // Redirige STDOUT vers le tube.STDOUT_FILENO est un descripteur de fichier pour la sortie standard.
        //descripteur de fichier est un entier qui identifie un fichier ouvert par un processus.
        dup2(tubeDesc[1], STDOUT_FILENO); 
        // Ferme le descripteur de fichier du tube.
        close(tubeDesc[1]); 
        close(tubeDesc[0]); 
        // Exécute la première commande.
        myexec(argc, args1); 
        // Si execv est réussi, le code suivant ne sera pas exécuté.
    }
    // Ici, nous sommes dans le processus parent.
    // Crée un second processus enfant.
    if ((pid_fils = fork()) == -1) { 
        perror("fork"); 
        exit(2); 
    }
    // Vérifie si dans le second processus enfant.
    if (pid_fils == 0) { 
        dup2(tubeDesc[0], STDIN_FILENO); // Redirige STDIN vers le tube.
        close(tubeDesc[0]); // Ferme le descripteur de fichier du tube.
        close(tubeDesc[1]); // Ferme le descripteur de fichier du tube.
        myexec(argc, args2); // Exécute la seconde commande.
    }
    // Fermeture importante des descripteurs de tube dans le processus parent.
     // Ferme le descripteur de fichier du tube.
    close(tubeDesc[0]);
    close(tubeDesc[1]);
    // Attend la fin du premier processus enfant et du second processus enfant.
    wait(0); 
    wait(0); 
    return 0; 
}

/*标准输入（STDIN_FILENO）：文件描述符为 0，通常指代键盘输入。
标准输出（STDOUT_FILENO）：文件描述符为 1，通常指代屏幕输出。
标准错误（STDERR_FILENO）：文件描述符为 2，用于错误消息的输出，通常也是屏幕*/