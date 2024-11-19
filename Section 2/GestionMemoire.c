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
	for (int i = 0; i < TAILLE_TLB; i++) {
		if(mem->tlb->entreeValide[i] == 1 && mem->tlb->numeroPage[i] == calculerNumeroDePage(req->adresseVirtuelle)) {
			req->adressePhysique = calculerAdresseComplete(mem->tlb->numeroCadre[i], calculerDeplacementDansLaPage(req->adresseVirtuelle));
			mem->tlb->dernierAcces[i] = req->date;
			req->estDansTLB = 1;
			return;
		}
	}
	req->adressePhysique = 0;
	req->estDansTLB = 0;
}

void rechercherTableDesPages(struct RequeteMemoire* req, struct SystemeMemoire* mem) {
	if(mem->tp->entreeValide[calculerNumeroDePage(req->adresseVirtuelle)]) {
		req->adressePhysique = calculerAdresseComplete(mem->tp->numeroCadre[calculerNumeroDePage(req->adresseVirtuelle)], calculerDeplacementDansLaPage(req->adresseVirtuelle));
		req->estDansTablePages = 1;
		return;
	}
	req->adressePhysique = 0;
	req->estDansTablePages = 0;
}

void ajouterDansMemoire(struct RequeteMemoire* req, struct SystemeMemoire* mem) {
	for(int i = 0; i < TAILLE_MEMOIRE; i++) {
		if (!mem->memoire->utilisee[i]) {
			mem->memoire->utilisee[i] = 1;
			mem->memoire->numeroPage[i] = calculerNumeroDePage(req->adresseVirtuelle);
			mem->memoire->dateCreation[i] = req->date;
			mem->memoire->dernierAcces[i] = req->date;

			mem->tp->numeroCadre[calculerNumeroDePage(req->adresseVirtuelle)] = i;
	 		mem->tp->entreeValide[calculerNumeroDePage(req->adresseVirtuelle)] = 1;
			req->adressePhysique = calculerAdresseComplete(i, calculerDeplacementDansLaPage(req->adresseVirtuelle));
		}
	}
	
	
}

void mettreAJourTLB(struct RequeteMemoire* req, struct SystemeMemoire* mem) {
	unsigned long dateAncienne = mem->tlb->dateCreation[0];
	int iAncien = 0;
	for(int i = 0; i < TAILLE_TLB; i++) {
		if(!mem->tlb->entreeValide[i]) {
			iAncien = i;
			break;
		}
		if(mem->tlb->dateCreation[i] < dateAncienne) {
			dateAncienne = mem->tlb->dateCreation[i];
			iAncien = i;
		}
	}
	mem->tlb->numeroPage[iAncien] = calculerNumeroDePage(req->adresseVirtuelle);
	mem->tlb->numeroCadre[iAncien] = mem->tp->numeroCadre[calculerNumeroDePage(req->adresseVirtuelle)];
	mem->tlb->entreeValide[iAncien] = 1;
	mem->tlb->dernierAcces[iAncien] = req->date;
	mem->tlb->dateCreation[iAncien] = req->date;

	req->adressePhysique = calculerAdresseComplete(mem->tlb->numeroCadre[iAncien], calculerDeplacementDansLaPage(req->adresseVirtuelle));
	req->estDansTLB = 1;
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
