#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

//EXPLIQUER : PAS DE RÉQUISITION et dire l'interblocage par son nom...?

int main() {
    int fd[2];

    pipe(fd); // Le tube créé ici est la ressource partagée, deux processus ne peuvent pas écrire/lire en même temps dans le même tube.

    if(!fork()){ // Détention et attente: Le premier enfant peut lire dans le tube, mais il demande une seconde ressource, soit que le parent écrive dans le tube avant.
        close(fd[1]); // Exclusion mutuelle: cette ligne empêche le premier enfant d'écrire dans le tube. La lecture dans le tube est permise.
        char* string[6];
        read(fd[0], string, 6);
        exit(0);
    }

    if(!fork()){
        close(fd[1]); // Exclusion mutuelle: Le 2e processus fils ferme le tube en écriture et en lecture, puisqu'il n'a pas besoin de cette ressource.
        close(fd[0]);
        execlp("ls", "ls", "-l", NULL);
        exit(0);
    }
// Le parent détient l'accès au tube en écriture, mais il demande une autre ressource, soit que ses enfants aient terminé.
    // while(wait(NULL) > 0);
    char string[6] = "Salut!";
    write(fd[1], &string, strlen(string)); // Le parent écrit dans le tube.
    printf("Process completed!\n");
    close(fd[0]);
    close(fd[1]);
    while(wait(NULL) > 0); // Pour régler l'interblocage, on peut s'assurer que le parent écrive dans le tube avant d'attendre la fin de ses enfants, 
    // en déplaçant cette ligne ici. Cela permet d'empêcher l'attente circulaire et donc, d'empêcher l'interblocage vu qu'une des 4 conditions n'est pas satisfaite.
    return 0;
}

// Attente circulaire: Puisque le 1er enfant attent une écriture dans le tube pour pouvoir lire et qu'aucune écriture n'est faite, alors le 1er enfant
// ne se terminera pas. D'un autre côté, l'écriture n'est pas faite, car le parent (qui doit écrire) attend que ses enfants aient terminé avant d'écrire.
// Ainsi, les deux processus attendent après l'autre et on se retrouve avec une attente circulaire.