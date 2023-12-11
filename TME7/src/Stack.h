#pragma once
#include <semaphore.h>
#include <sys/mman.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

#define STACKSIZE 100
#define SEM_PRODUCER_NAME "/sem_producer"
#define SEM_CONSUMER_NAME "/sem_consumer"

namespace pr {

template<typename T>
class Stack {
private:
    T *tab;  // Pointeur vers le tableau dans la mémoire partagée
    size_t sz;  // Taille actuelle de la pile
    sem_t *sem_producer; // Sémaphore utilisé par les producteurs
    sem_t *sem_consumer; // Sémaphore utilisé par les consommateurs

public:
    Stack();
    ~Stack();
    T pop();
    void push(T elt);
};

// Constructeur de la pile
template<typename T>
Stack<T>::Stack() : sz(0) {
    // Allocation de la mémoire partagée pour le tableau
    tab = static_cast<T *>(mmap(nullptr, STACKSIZE * sizeof(T),
                                PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,
                                -1, 0));
    // Création des sémaphores pour producteurs et consommateurs
    sem_producer = sem_open(SEM_PRODUCER_NAME, O_CREAT | O_EXCL, 0666, STACKSIZE);
    sem_consumer = sem_open(SEM_CONSUMER_NAME, O_CREAT | O_EXCL, 0666, 0);
    // Vérification si la création des sémaphores a échoué
    if (sem_producer == SEM_FAILED || sem_consumer == SEM_FAILED) {
        perror("Échec de la création des sémaphores");
        exit(EXIT_FAILURE); // Arrêt du programme si échec
    }
}

// Destructeur de la pile
template<typename T>
Stack<T>::~Stack() {
    // Libération de la mémoire partagée
    munmap(tab, STACKSIZE * sizeof(T));
    // Fermeture des sémaphores
    sem_close(sem_producer);
    sem_close(sem_consumer);
    // Suppression des sémaphores du système
    sem_unlink(SEM_PRODUCER_NAME);
    sem_unlink(SEM_CONSUMER_NAME);
}

// Méthode pour retirer un élément de la pile
template<typename T>
T Stack<T>::pop() {
    // Attente du sémaphore consommateur avant de retirer un élément
    sem_wait(sem_consumer);
    // Retrait de l'élément
    T ret = tab[--sz];
    // Signalement au sémaphore producteur qu'une place est libre
    sem_post(sem_producer);
    return ret; // Retour de l'élément retiré
}

// Méthode pour ajouter un élément à la pile
template<typename T>
void Stack<T>::push(T elt) {
    // Attente du sémaphore producteur avant d'ajouter un élément
    sem_wait(sem_producer);
    // Ajout de l'élément
    tab[sz++] = elt;
    // Signalement au sémaphore consommateur qu'un nouvel élément est disponible
    sem_post(sem_consumer);
}

} // Fin du namespace pr