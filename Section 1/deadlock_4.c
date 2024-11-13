#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_THREADS 5
#define MAX_ROUNDS 3

pthread_mutex_t locks[NUM_THREADS];

void* cons(void* arg) {
    int id = *(int*)arg;
    int rounds = 0;

// Attente circulaire: On se retrouve avec une situation semblable à celle du problème des philosophes (PowerPoint cours 5 p.43).
// Si chaque thread vérouille le verrou de gauche, mais attend le verrou de droite, on a une attente circulaire,
// puisque le verrou de droite du thread 1 correspond au verrou gauche du thread 2. Ainsi, chaque thread sera en attente.
    while (rounds < MAX_ROUNDS) {
        sleep(1);
        int left = id;
        int right = (id + 1) % NUM_THREADS;
        if (left < right) { // permet d'éviter l'attente circulaire, donc d'éviter l'interblocage. Solution basée sur le PowerPoint cours 5 p.47.
        // Exclusion mutuelle: Les pthread_mutex_lock/unlock permettent de s'assurer que la ressource "rounds" soit utilisée
        // par un seul thread à la fois. Une fois qu'un thread a un verrou (left et right), un autre thread ne peut pas y accéder.
            pthread_mutex_lock(&locks[left]);
            sleep(1);
            pthread_mutex_lock(&locks[right]); // Détention et attente: Le thread détient déjà locks[left] et attend locks[right]
        } else {
            pthread_mutex_lock(&locks[right]),
            sleep(1);
            pthread_mutex_lock(&locks[left]);
        }       
        sleep(1);
        rounds++;
        pthread_mutex_unlock(&locks[left]); // Pas de réquisition: Les ressources (locks[left], locks[right]) allouées sont libérées par le thread qui les utilise.
        pthread_mutex_unlock(&locks[right]);
    }

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_mutex_init(&locks[i], NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, cons, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

// La situation d'interblocage est connue, il s'agit du problème des philosophes.