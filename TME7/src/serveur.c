#include "chat_common.h"
#include <semaphore.h>
#include <fcntl.h> // for O_CREAT
#include <sys/mman.h> // for shm_open, mmap, munmap
#include <signal.h> // for sigaction
#include <unistd.h> // for sleep
#include <sys/stat.h>

//gcc -o serveur serveur.c -lpthread
//./serveur my_shared_memory

// Gestionnaire de signal pour la terminaison du serveur------Ctrl+C
int shouldexit = 0;
void exithandler(int sig) {
    // Assigner le signal reçu à l'indicateur de sortie
    shouldexit = sig; /* i.e !=0 */
}

// Structure pour représenter un utilisateur
struct user {
    char *name; // Nom de l'utilisateur
    // Segment de mémoire partagée pour l'utilisateur
    struct myshm *shm;
} *users[MAX_USERS];// Tableau de tous les utilisateurs

int main(int argc, char *argv[]) {
    // Nom du segment de mémoire partagée
    char *shmname;
    int shm_id, i;
    // Pointeur vers le segment de mémoire partagée du serveu
    struct myshm *shm_pere;
    // Structure pour définir une action de signal
    struct sigaction action;

    // Vérifiez si l'ID du serveur est passé en argument
    if (argc <= 1) {
        fprintf(stderr, "Usage: %s id_server\n", argv[0]); 
        exit(EXIT_FAILURE);
    }

    // Configurer le gestionnaire de signal pour Ctrl-C
    // Fonction à appeler lors de la réception de SIGINT
    action.sa_handler = exithandler; 
    // Options du gestionnaire de signal
    action.sa_flags = 0; 
    // Initialiser l'ensemble des signaux bloqués
    sigemptyset(&action.sa_mask); 
    // Appliquer l'action pour le signal SIGINT
    sigaction(SIGINT, &action, 0); 

    // Créer l'identifiant du segment de mémoire partagée
    shmname = argv[1];

    // Créer le segment de mémoire partagée avec des droits en lecture et écriture
    if ((shm_id = shm_open(shmname, O_RDWR | O_CREAT, 0666)) == -1) {
        perror("shm_open shm_pere");
        exit(EXIT_FAILURE);
    }

    // Allouer la taille pour NB_MESS messages dans le segment de mémoire partagée
    if (ftruncate(shm_id, sizeof(struct myshm)) == -1) {
        perror("ftruncate shm_pere");
        exit(EXIT_FAILURE);
    }

    // Mapper le segment de mémoire pour le serveur avec des droits de lecture et écriture
    if ((shm_pere = mmap(NULL, sizeof(struct myshm), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0)) == MAP_FAILED) {
        perror("mmap shm_pere");
        exit(EXIT_FAILURE);
    }
    
    // Initialiser les indices de lecture et écriture et le nombre de messages à zéro
    shm_pere->read = 0;
    shm_pere->read = 0;
    shm_pere->write = 0;
    shm_pere->nb = 0;

    // Initialiser le sémaphore (mutex) pour le segment de mémoire partagée
    sem_t *sem = sem_open("/mysemaphore", O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }
    

    /* Initializer le tableau des utilisateurs */
    for (i = 0; i < MAX_USERS; i++) {
        users[i] = NULL;
    }

    // pour chaque message reçu
    while (!shouldexit) {
         // Attendre que le sémaphore soit disponible pour lire le segment de mémoire partagée
        sem_wait(&(shm_pere->sem));

        // Vérifier s'il y a de nouveaux messages à traiter
        if (shm_pere->read != shm_pere->write || shm_pere->nb == MAX_MESS) {

            // Récupérer le premier message à lire
            struct message message = shm_pere->messages[shm_pere->read];

            // Traiter le message selon son type
            switch (message.type) {
                // Case pour la connexion d'un nouvel utilisateur
                case 0: {
                    /* Connection */
                    char *username;
                    int shm_id_user;
                    i = 0;

                    /* trouver un emplacement libre dans le tableau des utilisateurs */
                    while (i < MAX_USERS && users[i] != NULL) i++;
                    if (i == MAX_USERS) {
                        perror("impossible d'ajouter l'user"); 
                        exit(EXIT_FAILURE);
                    }
                    // Allouer de la mémoire pour l'utilisateur
                    users[i] = malloc(sizeof(struct user));
                    if (users[i] == NULL) {
                        perror("impossible d'ajouter l'user"); 
                        exit(EXIT_FAILURE);
                    }
                    //
                    /* copier le nom de l'utilisateur */
                    users[i]->name = malloc((strlen(message.content) + 1) * sizeof(char));
                    strcpy(users[i]->name, message.content);
                    users[i]->name[strlen(message.content)] = '\0';

                    printf("Connexion de %s\n", users[i]->name);

                    /* Créer le segment de mémoire partagée pour l'utilisateur */
                    username = message.content;
                    if ((shm_id_user = shm_open(username, O_RDWR | O_CREAT, 0666)) == -1) {
                        perror("shm_open shm_fils");
                        exit(EXIT_FAILURE);
                    }

                    /* Allouer la taille pour NB_MESS messages dans le segment de mémoire partagée */
                    if ((users[i]->shm = mmap(NULL, sizeof(struct myshm), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id_user, 0)) == MAP_FAILED) {
                        perror("mmap shm_fils");
                        exit(EXIT_FAILURE);
                    }
                    break;
                }
                case 1: {
                    /* Message sending */
                    int temp = 0;
                    printf("Réception message serveur : %s\n", message.content);
                    for (i = 0; i < MAX_USERS; i++) {
                        if (users[i] != NULL) {
                            struct message msg;
                            struct myshm *shm = users[i]->shm;
                            temp++;

                            /* Copy the message to the child's queue */
                            msg.type = 1;
                            strcpy(msg.content, message.content);

                            sem_wait(&(shm->sem));

                            while (shm->nb == MAX_MESS) {
                                sem_post(&(shm->sem));
                                sleep(1);
                                sem_wait(&(shm->sem));
                            }

                            /* TODO: verify that it's not full */
                            printf("\t\tSending to %s\n", users[i]->name);

                            shm->messages[shm->write] = msg;
                            shm->write = (shm->write + 1) % MAX_MESS;
                            shm->nb++;

                            sem_post(&(shm->sem));
                        }
                    }

                    if (temp == 0) {
                        printf("Nobody found!\n");
                    }
                    break;
                }
                case 2: {
                    /* Disconnection */
                    printf("Déconnexion de %s\n", message.content);
                    i = 0;

                    /* Retrieve the user corresponding to the disconnecting user */
                    while (i < MAX_USERS && (users[i] == NULL || strcmp(users[i]->name, message.content) != 0)) i++;
                    if (i == MAX_USERS) {
                        perror("Something went wrong! L'user n'existe pas!");
                        exit(EXIT_FAILURE);
                    }

                    /* Free memory space */
                    free(users[i]->name);
                    munmap(users[i]->shm, sizeof(struct myshm));
                    free(users[i]);
                    users[i] = NULL;

                    break;
                }
            }
            shm_pere->read = (shm_pere->read + 1) % MAX_MESS;
            shm_pere->nb--;
        }
        sem_post(&(shm_pere->sem));
    }

    sem_close(&(shm_pere->sem));

    munmap(shm_pere, sizeof(struct myshm));
    shm_unlink(shmname);

    //close the semaphore
    sem_close(sem);
    // Unlink the semaphore if it's no longer needed
    sem_unlink("/mysemaphore");

    return EXIT_SUCCESS;
}
