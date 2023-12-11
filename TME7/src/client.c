#include "chat_common.h"
#include <semaphore.h>
#include <fcntl.h> // for O_CREAT
#include <sys/mman.h> // for shm_open, mmap, munmap
#include <signal.h> // for sigaction
#include <unistd.h> // for sleep

    int shouldexit = 0;
    struct myshm *shm_client, *shm_pere;
    void *reader(void* arg){ while(!shouldexit){
    sem_wait(&(shm_client->sem));
    /* Si il y a eu au moins une écriture */
    if(shm_client->read != shm_client->write || shm_client->nb == MAX_MESS){ /* On récupère le premier message et on incrémente le nombre lu */ struct message message = shm_client->messages[shm_client->read]; shm_client->read = (shm_client->read + 1) % MAX_MESS;
        printf("%s", message.content);
        shm_client->nb--;
    }
    sem_post(&(shm_client->sem));
    }
    pthread_exit(NULL);
    return NULL; }
    void *writer(void* arg){ while(!shouldexit){
    struct message msg;
    msg.type = 1;
    fgets(msg.content, TAILLE_MESS, stdin);
    /* Evite d’envoyer un message avec Ctrl-C dedans */
    if(shouldexit) break;
    sem_wait(&(shm_pere->sem));
    while(shm_pere->nb == MAX_MESS){ sem_post(&(shm_pere->sem)); sleep(1); sem_wait(&(shm_pere->sem));
    }
    shm_pere->messages[shm_pere->write] = msg;
    shm_pere->write = (shm_pere->write + 1) % MAX_MESS;
    shm_pere->nb++;
    sem_post(&(shm_pere->sem));
    }
    pthread_exit(NULL);
    return NULL; }
    void exithandler(int sig){ shouldexit = sig; /* i.e. !=0 */
    }
    int main(int argc, char *argv[]){
    char *shm_client_name, *shm_pere_name; int shm_client_id, shm_pere_id;
    pthread_t tids[2];
    struct message msg;
    struct sigaction action;
    if(argc<=2){
    fprintf(stderr, "Usage: %s id_client id_server\n", argv[0]); exit(EXIT_FAILURE);
    }
    printf("Connexion à %s sous l'identité %s\n", argv[2], argv[1]);
    /* Met un handler pour arrêter le programme de façon clean avec Ctrl-C */
    action.sa_handler = exithandler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, 0);
    shm_client_name = (argv[1]);
    shm_pere_name = (argv[2]);
    /* Crée le segment en lecture écriture */
    if ((shm_client_id = shm_open(shm_client_name, O_RDWR | O_CREAT, 0666)) == -1) { perror("shm_open shm_client");
    exit(errno);
    }
    if ((shm_pere_id = shm_open(shm_pere_name, O_RDWR | O_CREAT, 0666)) == -1) {
    perror("shm_open shm_pere");
    exit(errno);
    }
    /* Allouer au segment une taille de NB_MESS messages */
    if(ftruncate(shm_client_id, sizeof(struct myshm)) == -1) { perror("ftruncate shm_client");
    exit(errno);
    }
    /* Mappe le segment en read-write partagé */
    if((shm_pere = mmap(NULL, sizeof(struct myshm), PROT_READ | PROT_WRITE, MAP_SHARED, shm_pere_id, 0)) == MAP_FAILED){
    perror("mmap shm_pere");
    exit(errno);
    }
    if((shm_client = mmap(NULL, sizeof(struct myshm), PROT_READ | PROT_WRITE, MAP_SHARED, shm_client_id, 0)) == MAP_FAILED){
    perror("mmap shm_pere");
    exit(errno);
    }
    shm_client->read = 0;
    shm_client->write = 0;
    shm_client->nb = 0;
    /* Initialisation du sémaphore (mutex) */
    sem_t *sem_client = sem_open("/sem_client_name", O_CREAT, 0644, 1);
    if (sem_client == SEM_FAILED) { 
        perror("sem_open shm_client");
        exit(EXIT_FAILURE); // or exit(errno);
    }
    
    /* Préparer un message de type connexion */
    msg.type = 0;
    strcpy(msg.content, argv[1]);
    // Attendre que le sémaphore soit libre avant de modifier le segment de mémoire
    sem_wait(&(shm_pere->sem));

    // Ajouter le message dans la file de messages et mettre à jour l'index d'écriture et le nombre de messages
    shm_pere->messages[shm_pere->write] = msg;
    shm_pere->write = (shm_pere->write + 1) % MAX_MESS;
    shm_pere->nb++;

    // Libérer le sémaphore après modification
    sem_post(&(shm_pere->sem));

    // Création des threads pour lire et écrire et attente de leur terminaison
    pthread_create(&(tids[0]), NULL, reader, NULL);
    pthread_create(&(tids[1]), NULL, writer, NULL);
    pthread_join(tids[0], NULL);
    pthread_join(tids[1], NULL);

    printf("Déconnexion...\n");

    // Préparer un message de type déconnexion
    msg.type = 2;
    sem_wait(&(shm_pere->sem));

    // Ajouter le message de déconnexion dans la file
    shm_pere->messages[shm_pere->write] = msg;
    shm_pere->write = (shm_pere->write + 1) % MAX_MESS;
    shm_pere->nb++;

    // Libérer le sémaphore après ajout du message de déconnexion
    sem_post(&(shm_pere->sem));

    // Fermer le sémaphore du client et libérer la mémoire partagée
    sem_close(&(shm_client->sem));
    munmap(shm_client, sizeof(struct myshm));
    shm_unlink(shm_client_name);

    sem_close(sem_client); // Close the semaphore
    sem_unlink("/sem_client_name"); // Unlink the semaphore if it's no longer needed

    return EXIT_SUCCESS;
}