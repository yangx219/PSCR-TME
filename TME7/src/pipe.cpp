#include <sys/types.h> 
#include <sys/wait.h>  
#include <unistd.h>    
#include <cstdlib>     
#include <cstdio>      
#include <cstring>     
#include <iostream>    
//g++ -o pipe pipe.cpp
//./pipe /bin/cat pipe.cpp \| /usr/bin/wc -l

// Déclare la fonction myexec qui exécutera une commande.
void myexec(int argc, char ** args) {
    std::cerr << "args :" ; // Affiche les arguments sur la sortie d'erreur standard.
    for (int i=0; i < argc ; i++) { // Boucle à travers les arguments.
        if (args[i] != nullptr) { // Vérifie si l'argument n'est pas un pointeur nul.
            std::cerr << args[i] << " "; // Affiche l'argument.
        } else {
            break; // Quitte la boucle si un pointeur nul est trouvé.
        }
    }
    std::cerr << std::endl; // Insère une nouvelle ligne.
    
    if (execv(args[0], args) == -1) { // Exécute la commande et vérifie l'échec.
        perror("execv"); // Affiche le message d'erreur.
        exit(3); // Termine le processus avec le code 3.
    }
}

// La fonction principale.
int main(int argc, char ** argv) {
    // Partitionne les arguments en deux ensembles pour deux commandes.
    char * args1[argc];
    char * args2[argc];
    
    memset(args1, 0, argc*sizeof(char*)); // Initialise le premier tableau d'arguments.
    memset(args2, 0, argc*sizeof(char*)); // Initialise le second tableau d'arguments.
    
    int arg = 1; // Commence à 1 pour ignorer le nom du programme.
    for ( ; arg < argc ; arg++) { // Parcourt les arguments.
        if (!strcmp(argv[arg], "|")) { // Vérifie la présence du symbole de pipe.
            arg++; // Incrémente pour passer le symbole de pipe.
            break; // Sort de la boucle.
        } else {
            args1[arg-1] = argv[arg]; // Ajoute l'argument au premier tableau.
        }
    }
    for (int i=0; arg < argc ; i++, arg++) { // Continue avec le reste des arguments.
        args2[i] = argv[arg]; // Ajoute l'argument au second tableau.
    }
    
    int tubeDesc[2]; // Tableau pour les descripteurs de fichier de tube.
    pid_t pid_fils; // PID du processus enfant.
    if (pipe(tubeDesc) == -1) { // Tente de créer un tube.
        perror("pipe"); // Affiche le message d'erreur si le tube n'est pas créé.
        exit(1); // Sortie avec le code d'erreur 1.
    }
    if ((pid_fils = fork()) == -1) { // Tente de créer un processus enfant.
        perror("fork"); // Affiche le message d'erreur si le fork échoue.
        exit(2); // Sortie avec le code d'erreur 2.
    }
    if (pid_fils == 0) { // Vérifie si dans le processus enfant.
        dup2(tubeDesc[1], STDOUT_FILENO); // Redirige STDOUT vers le tube.
        close(tubeDesc[1]); // Ferme le descripteur de fichier du tube.
        close(tubeDesc[0]); // Ferme le descripteur de fichier du tube.
        myexec(argc, args1); // Exécute la première commande.
        // Si execv est réussi, le code suivant ne sera pas exécuté.
    }
    // Ici, nous sommes dans le processus parent.
    if ((pid_fils = fork()) == -1) { // Crée un second processus enfant.
        perror("fork"); // Affiche le message d'erreur si le fork échoue.
        exit(2); // Sortie avec le code d'erreur 2.
    }
    if (pid_fils == 0) { // Vérifie si dans le second processus enfant.
        dup2(tubeDesc[0], STDIN_FILENO); // Redirige STDIN vers le tube.
        close(tubeDesc[0]); // Ferme le descripteur de fichier du tube.
        close(tubeDesc[1]); // Ferme le descripteur de fichier du tube.
        myexec(argc, args2); // Exécute la seconde commande.
    }
    // Fermeture importante des descripteurs de tube dans le processus parent.
    close(tubeDesc[0]); // Ferme le descripteur de fichier du tube.
    close(tubeDesc[1]); // Ferme le descripteur de fichier du tube.
    wait(0); // Attend la fin du premier processus enfant.
    wait(0); // Attend la fin du second processus enfant.
    return 0; // Fin du programme avec succès.
}
