#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

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
        // Pas de réquisition: La ressource (sem_critical) est allouée à un thread à la fois, puis uniquement celui-ci libère la ressource. Donc, on ne peut pas libérer
        // cette ressource ailleurs que dans le thread qui la détient.
        int key = rand() % 30;
        if(try_lock(key)) {
            sem_post(&sem_critical); // Pour régler le blocage, on libère la ressource (sem_critical) même si la porte est déverrouillée. 
                                     // Ainsi, le thread suivant peut aussi accéder à la porte.
            pthread_exit(NULL); // Détention et attente: Si le thread réussi à déverrouiller la porte, il appellera pthread_exit sans libérer la ressource (sem_critical).
        }
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

// Il ne s'agit pas d'interblocage, car il n'y a aucune attente circulaire. Toutefois, il y a un blocage infini
// en raison de la ressource qui n'était jamais libérée lorsque la porte était déverrouillée.