#include<stdbool.h>
#include "./libs/lib.h"
#define TAILLE_PAGE 1024

unsigned int calculerNumeroDePage(unsigned long adresse) {
    return adresse/TAILLE_PAGE;
}

unsigned long calculerDeplacementDansLaPage(unsigned long adresse) {
     return adresse%TAILLE_PAGE;
}

unsigned long calculerAdresseComplete(unsigned int numeroDePage, unsigned long deplacementDansLaPage) {
     return (numeroDePage * TAILLE_PAGE) + deplacementDansLaPage;
}

void rechercherTLB(struct RequeteMemoire* req, struct SystemeMemoire* mem) {
	if(req.estDansTLB) {
		req.adressePhysique = calculerAdresseComplete(req.adresseVirtuelle);
		req.date = // date de mtn
		// mettre à jour les autres attributs?
	} else {
		req.adressePhysique = 0;
	}
}

void rechercherTableDesPages(struct RequeteMemoire* req, struct SystemeMemoire* mem) {
	if (calculerNumeroDePage(req.adresseVirtuelle) == mem.memoire.numeroPage) {
		req.adressePhysique = calculerAdresseComplete(req.adresseVirtuelle);
		// mettre à jour les autres attributs?
	} else {
		req.adressePhysique = 0;
	}
}

void ajouterDansMemoire(struct RequeteMemoire* req, struct SystemeMemoire* mem) {
	mem.tlb.numeroCadre[0] = 
}

void mettreAJourTLB(struct RequeteMemoire* req, struct SystemeMemoire* mem) {
	// TODO
}

// NE PAS MODIFIER
int main() {
    evaluate(
		&calculerNumeroDePage, 
		&calculerDeplacementDansLaPage, 
		&calculerAdresseComplete, 
        &rechercherTLB, 
		&rechercherTableDesPages,
		&mettreAJourTLB,
		&ajouterDansMemoire
    );
    return 0;
}
