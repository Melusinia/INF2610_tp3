#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

// EXPLIQUER DÉTENTION ET ATTENTE, PAS DE RÉQUISITION ET ATTENTE CIRCULAIRE

#define ENTRY_CODE 7
#define N_THREADS 5

sem_t sem_critical;

int try_lock(int key){
    if(key == ENTRY_CODE){
        printf("Door unlocked\n");
        return 1;
    }
    sleep(1);
    return 0;
}

void* door(void* args){
    while(1){
        sem_wait(&sem_critical); // Exclusion mutuelle: Le sémaphore sem_critical permet d'allouée la ressource à un seul thread à la fois.
        // Dans ce cas-ci la ressource protégée par le sémaphore est le fait d'essayer de déverrouiller la porte (try_lock(key)). 
        // Donc, un seul thread à la fois peut essayer de déverrouiller la porte avec la key.
        int key = rand() % 30;
        if(try_lock(key))
            pthread_exit(NULL);
        sem_post(&sem_critical); // Exclusion mutuelle: Le sémaphore sem_critical est libéré pour permettre à un autre thread d'essayer de dévérouiller la porte.
    }
}
int main() {

    sem_init(&sem_critical, 0, 1); 
    pthread_t threads[N_THREADS];

    for(int i = 0; i < N_THREADS; i++){
        pthread_create(&threads[i], NULL, door, NULL);
    }

    for(int i = 0; i < N_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    return 0;
}