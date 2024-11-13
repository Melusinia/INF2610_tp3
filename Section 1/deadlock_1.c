#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define BUFFER_SIZE 5
#define N_THREADS 1
#define N_THREADS_2 10

int buffer[BUFFER_SIZE];
int ip;
int ic;

sem_t sem_initial;
sem_t sem_busy;
sem_t sem_critical;

int flag = 0;

// Pas de réquisition: Il n'y a pas vraiment de réquisition, car les producteurs et les consommateurs attendent après l'autre
// seulement lorsque le buffer est plein ou vide. Ainsi, il ne s'agit pas d'une attente infinie et donc il n'y a pas de réquisition.

void* producer(void* arg) {
    while (1) {
        sem_wait(&sem_initial); // Détention et attente: Le thread peut détenir sem_initial, puis demander sem_critical par la suite.
        sem_wait(&sem_critical); // Exclusion mutuelle: Le sémaphore sem_critical permet de s'assurer que le buffer est utilisé par un seul thread à la fois.
        buffer[ip] = rand() % 9 + 1;
        ip = (ip + 1) % BUFFER_SIZE;
        sem_post(&sem_critical); // Exclusion mutuelle: sem_critical est libéré.
        sem_post(&sem_busy); 
        if (flag)
            break;
    }
    pthread_exit(NULL);
}

// Il y a une possible attente circulaire, puisque les threads producer et consumer s'attendent mutuellement.
// Le producteur va attendre qu'il y a de l'espace libre dans le buffer (sem_initial).
// Le consommateur va attendre que le buffer comporte quelque chose à consommer.
// Ainsi, les threads s'attendent en boucle ce qui cause une attente circulaire.

void* consumer(void* arg) {
    while (1) {
        sem_wait(&sem_busy); // Détention et attente: Un peu comme pour producer, le thread consumer peut détenir sem_busy, puis demander sem_critical par la suite.
        sem_wait(&sem_critical); // Exclusion mutuelle: Le sémaphore sem_critical permet de s'assurer que le buffer est utilisé par un seul thread à la fois.
        int x = buffer[ic];
        ic = (ic + 1) % BUFFER_SIZE;
        sem_post(&sem_critical);
        sem_post(&sem_initial);
        if(x == 0){
            break;
        }
    }
    pthread_exit(NULL);
}

void changeFlag(){
    flag = 1;
    printf("Flag changed\n");
}

int main() {
    pthread_t prod_thread[N_THREADS], cons_thread[N_THREADS_2];

    sem_init(&sem_initial, 0, BUFFER_SIZE);
    sem_init(&sem_busy, 0, 0);           
    sem_init(&sem_critical, 0, 1); 
    signal(SIGALRM, &changeFlag);      

    for(int i = 0; i < N_THREADS; i++){
        pthread_create(&prod_thread[i], NULL, producer, NULL);
    }

    for(int i = 0; i < N_THREADS_2; i++){
        pthread_create(&cons_thread[i], NULL, consumer, NULL);
    }

    alarm(1);

    for(int i = 0; i < N_THREADS; i++){
        pthread_join(prod_thread[i], NULL);
    }

    for(int i = 0; i < N_THREADS_2; i++){
        // Nous avons ajouté sem_critical pour l'accès au buffer (ressource partagée), sem_initial pour vérifier qu'il y a
        // de l'espace dans le buffer. Ainsi, on évite d'écrire par-dessus les chiffres déjà dans le buffer. On copie le
        // comportement du producteur.
        sem_wait(&sem_initial);
        sem_wait(&sem_critical);
        buffer[ip] = 0;
        ip = (ip + 1) % BUFFER_SIZE;
        sem_post(&sem_critical);
        sem_post(&sem_busy);
        // Le consommateur va consommer un zero et va sortir du thread.
    }

    for(int i = 0; i < N_THREADS_2; i++){
        pthread_join(cons_thread[i], NULL);
    }

    sem_destroy(&sem_initial);
    sem_destroy(&sem_busy);
    sem_destroy(&sem_critical);

    return 0;
}

// Il s'agit d'une situation d'interblocage connue, soit le problème des producteurs/consommateurs.