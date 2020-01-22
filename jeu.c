/*
	Canvas pour algorithmes de jeux à 2 joueurs

	joueur 0 : humain
	joueur 1 : ordinateur

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Paramètres du jeu
#define LARGEUR_MAX 7 		// nb max de fils pour un noeud (= nb max de coups possibles)

#define TEMPS 5		// temps de calcul pour un coup avec MCTS (en secondes)

#define LIGNE 6
#define COLONNE 7


// macros
#define AUTRE_JOUEUR(i) (1-(i))
#define min(a, b)       ((a) < (b) ? (a) : (b))
#define max(a, b)       ((a) < (b) ? (b) : (a))

// Critères de fin de partie
typedef enum {NON, MATCHNUL, ORDI_GAGNE, HUMAIN_GAGNE } FinDePartie;

// Definition du type Etat (état/position du jeu)
typedef struct EtatSt {

	int joueur; // à qui de jouer ?

	// TODO: à compléter par la définition de l'état du jeu

	char plateau[LIGNE][COLONNE];

} Etat;

// Definition du type Coup
typedef struct {
	// TODO: à compléter par la définition d'un coup

	/* par exemple, pour morpion: */
	int ligne;
	int colonne;

} Coup;

// Copier un état
Etat * copieEtat( Etat * src ) {

	Etat * etat = (Etat *)malloc(sizeof(Etat));
	etat->joueur = src->joueur;

	// TODO: à compléter avec la copie de l'état src dans etat

	/* par exemple : */
	int i,j;
	for (i=0; i< LIGNE; i++){
		for ( j=0; j<COLONNE; j++){
			etat->plateau[i][j] = src->plateau[i][j];
		}
	}

	return etat;
}

// Etat initial
Etat * etat_initial() {

	Etat * etat = (Etat *)malloc(sizeof(Etat));

	// TODO: à compléter avec la création de l'état initial

	/* par exemple : */
	int i,j;
	for (i=0; i< LIGNE; i++){
		for ( j=0; j<COLONNE; j++){
			etat->plateau[i][j] = ' ';
		}
	}

	return etat;
}

void afficheJeu(Etat * etat) {

	// TODO: à compléter

	/* par exemple : */
	int i,j;
	printf("   |");
	for ( j = 0; j < COLONNE; j++)//colonnes
		printf(" %d |", j);
	printf("\n--------------------------------\n");

	for(i=LIGNE-1; i >=0; i--) {//lignes
		printf(" %d |", i);
		for ( j = 0; j < COLONNE; j++)//longueur ligne
			printf(" %c |", etat->plateau[i][j]);
		printf("\n--------------------------------\n");
	}

}

// Nouveau coup
// TODO: adapter la liste de paramètres au jeu
Coup * nouveauCoup( int i, int j ) {

	Coup * coup = (Coup *)malloc(sizeof(Coup));

	// TODO: à compléter avec la création d'un nouveau coup

	/* par exemple : */
	coup->ligne = i;
	coup->colonne = j;
	//printf("je place en %d, %d", i,j);

	return coup;
}

// Demander à l'humain quel coup jouer
Coup * demanderCoup () {

	// TODO...

	/* par exemple : */
	int i,j;
	printf(" quelle colonne ? ") ;
	scanf("%d",&j);
	i=0;


	return nouveauCoup(i,j);
}

// Modifier l'état en jouant un coup
// retourne 0 si le coup n'est pas possible
int jouerCoup( Etat * etat, Coup * coup ) {

	// TODO: à compléter
	/* par exemple : */

	if(etat->plateau[LIGNE-1][coup->colonne] == ' '){// si de la place dans la colonne
		int c = coup->ligne;

		if ( etat->plateau[coup->ligne][coup->colonne] != ' ' ){ // si la ligne pas dispo
			do{ //  je remonte jusqu'a trouver une place libre
				c++;
			}while(etat->plateau[c][coup->colonne] !=' ');
		}

		etat->plateau[c][coup->colonne] = etat->joueur ? 'O' : 'X';
		etat->joueur = AUTRE_JOUEUR(etat->joueur);

		return 1;

	}else{
		return 0;
	}

}

// Retourne une liste de coups possibles à partir d'un etat
// (tableau de pointeurs de coups se terminant par NULL)
Coup ** coups_possibles( Etat * etat ) {

	Coup ** coups = (Coup **) malloc((1+LARGEUR_MAX) * sizeof(Coup *) );

	int k = 0;

	// TODO: à compléter

	/* par exemple */
	int i,j;
	for(i=0; i < LIGNE; i++) {
		for (j=0; j < COLONNE; j++) {
			if ( etat->plateau[i][j] == ' ' ) {
				coups[k] = nouveauCoup(i,j);
				k++;
			}
		}
	}
	/* fin de l'exemple */

	coups[k] = NULL;

	return coups;
}


// Definition du type Noeud
typedef struct NoeudSt {

	int joueur; // joueur qui a joué pour arriver ici
	Coup * coup;   // coup joué par ce joueur pour arriver ici

	Etat * etat; // etat du jeu

	struct NoeudSt * parent;
	struct NoeudSt * enfants[LARGEUR_MAX]; // liste d'enfants : chaque enfant correspond à un coup possible
	int nb_enfants;	// nb d'enfants présents dans la liste

	// POUR MCTS:
	int nb_victoires;
	int nb_simus;

} Noeud;

// Créer un nouveau noeud en jouant un coup à partir d'un parent
// utiliser nouveauNoeud(NULL, NULL) pour créer la racine
Noeud * nouveauNoeud (Noeud * parent, Coup * coup ) {

	Noeud * noeud = (Noeud *)malloc(sizeof(Noeud));

	if ( parent != NULL && coup != NULL ) {
		noeud->etat = copieEtat ( parent->etat );
		jouerCoup ( noeud->etat, coup );
		noeud->coup = coup;
		noeud->joueur = AUTRE_JOUEUR(parent->joueur);
	}else {
		noeud->etat = NULL;
		noeud->coup = NULL;
		noeud->joueur = 0;
	}
	noeud->parent = parent;
	noeud->nb_enfants = 0;

	// POUR MCTS:
	noeud->nb_victoires = 0;
	noeud->nb_simus = 0;

	return noeud;
}

// Ajouter un enfant à un parent en jouant un coup
// retourne le pointeur sur l'enfant ajouté
Noeud * ajouterEnfant(Noeud * parent, Coup * coup) {
	Noeud * enfant = nouveauNoeud (parent, coup ) ;
	parent->enfants[parent->nb_enfants] = enfant;
	parent->nb_enfants++;
	return enfant;
}

void freeNoeud ( Noeud * noeud) {
	if ( noeud->etat != NULL)
		free (noeud->etat);

	do {
		freeNoeud(noeud->enfants[noeud->nb_enfants-1]);
		noeud->nb_enfants --;
	}while ( noeud->nb_enfants > 0 );
	if ( noeud->coup != NULL)
		free(noeud->coup);

	free(noeud);
}

// Test si l'état est un état terminal
// et retourne NON, MATCHNUL, ORDI_GAGNE ou HUMAIN_GAGNE
FinDePartie testFin( Etat * etat ) {

	// TODO...

	/* par exemple	*/

	// tester si un joueur a gagné
	int i,j,k,n = 0;


	for (i=0;i < LIGNE; i++) {
		for (j=0; j < COLONNE; j++) {
			printf("\nje suis dans le cas i=%d et j=%d\n",i,j);
			if ( etat->plateau[i][j] != ' ') {
				printf("c'est vide\n");

				n++;	// nb coups joués

				// lignes
				k=0;

				while ( k < 4 && i+k < COLONNE && etat->plateau[i+k][j] == etat->plateau[i][j] ){
					printf("while ligne\n");
					k++;
				}

				if ( k == 4 ){
					printf("LIGNE");
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;
				}

				// colonnes
				k=0;
				while ( k < 4 && j+k < LIGNE && etat->plateau[i][j+k] == etat->plateau[i][j] ){
					printf("while colonne\n");
					k++;
				}

				if ( k == 4 ){
					printf("COLONNE");
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;
				}

				// diagonales
				k=0;
				while ( k < 4 && i+k < COLONNE && j+k < LIGNE && etat->plateau[i+k][j+k] == etat->plateau[i][j] ){
					printf("while diag 1\n");
					k++;
				}
				if ( k == 4 ){
					printf("DIAG 1");
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;
				}

				k=0;
				while ( k < 4 && i+k < COLONNE && j-k >= LIGNE && etat->plateau[i+k][j-k] == etat->plateau[i][j] ){
					printf("while diag 2\n");
					k++;
				}
				if ( k == 4 ){
					printf("DIAG 2");
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;
				}
			}
		}
	}


	// et sinon tester le match nul
	if ( n == LIGNE*COLONNE )
		return MATCHNUL;

	return NON;
}

/**
* Calcule et joue un coup de l'ordinateur avec MCTS-UCT
* en tempsmax secondes
*/
void ordijoue_mcts(Etat * etat, int tempsmax) {

	clock_t tic, toc;
	tic = clock();
	int temps;

	Coup ** coups;
	Coup * meilleur_coup ;

	// Créer l'arbre de recherche
	Noeud * racine = nouveauNoeud(NULL, NULL);
	racine->etat = copieEtat(etat);

	// créer les premiers noeuds:
	coups = coups_possibles(racine->etat);

	int k = 0;
	Noeud * enfant;
	while ( coups[k] != NULL) {
		enfant = ajouterEnfant(racine, coups[k]);
		k++;
	}


	meilleur_coup = coups[ rand()%k ]; // choix aléatoire A SUPPRIMER

	/*  TODO :
		- supprimer la sélection aléatoire du meilleur coup ci-dessus
		- implémenter l'algorithme MCTS-UCT pour déterminer le meilleur coup ci-dessous
		
	int iter = 0;

	do {

		//Sélectionner
		Noeud * n = selectionner(racine);
		//Developper (selection du chemin/fils a prendre)
		Noeud * s = developper(n);
		//simuler ce chemin
		int res = simuler(s);
		//maj de tout (B val, N, etc)


		// à compléter par l'algorithme MCTS-UCT...


		toc = clock();
		temps = (int)( ((double) (toc - tic)) / CLOCKS_PER_SEC );
		iter ++;
	} while ( temps < tempsmax );

	/* fin de l'algorithme  */

	// Jouer le meilleur premier coup
	jouerCoup(etat, meilleur_coup );

	// Penser à libérer la mémoire :
	freeNoeud(racine);
	free (coups);
}

Noeud selectionner(Noeud * n){

	Coup ** coups;
	coups = coups_possibles[n->etat];

	int nb_coups_possible=0;
	while(coups[nb_coups_possible] != NULL){
		nb_coups_possible++;
	}

	if(n->nb_enfants < nb_coups_possible){ //  tous les fils ne possède pas de B valeur
		//faire fonction qui prend un fils tableau
		//return ce fils

	}else{ // on va pouvoir descendre d'un étage par le fils ayant le plus grand Bvaleur
		int i = 0;
		int Bmax = n->enfant[i]->B;
		Noeud * fils = n->enfant[i];
		for(i=1; i<nb_coups_possible;i++){
			if(n->enfant[i]->B > Bmax){
				Bmax = n->enfant[i]->B;
				fils =  n->enfant[i];
			}
		}
		return selectionner(fils);
	}

}

Noeud developer(Noeud * n){ //retourne le noeud à partir duquel simuler

}

int simuler(Noeud * n){ // retourne le résultat de la partie simulé (-1,0,1)
	Coup ** coups = coups_possibles[n->etat];
	Noeud * depart = nouveauNoeud(NULL, NULL);
	depart->etat = copieEtat(n->etat);
	int fini = 0
	while( !fini ){
		//rand sur i
		jouerCoup(depart->etat,coups[i]);
		fini = testFin(depart->etat); // note à moi même : ne pas simuler si dejà une feuille
	}

	// mettre à jour les valeurs



}



int main(void) {

	Coup * coup;
	FinDePartie fin;

	// initialisation
	Etat * etat = etat_initial();

	// Choisir qui commence :
	printf("Qui commence (0 : humain, 1 : ordinateur) ? ");
	scanf("%d", &(etat->joueur) );

	while ( fin == NON ) { // boucle de jeu

		printf("\n");
		afficheJeu(etat);


		if ( etat->joueur == 0 ) {// tour de l'humain
			do{
				coup = demanderCoup();
			}while( !jouerCoup(etat, coup) );

		}
		else {// tour de l'Ordinateur
			do{
				coup = demanderCoup();
			}while( !jouerCoup(etat, coup) );
			//ordijoue_mcts( etat, TEMPS );
		}

		fin = testFin( etat );

	}

	printf("\n");
	afficheJeu(etat);

	if ( fin == ORDI_GAGNE )
		printf( "** L'ordinateur a gagné **\n");
	else if ( fin == MATCHNUL )
		printf(" Match nul !  \n");
	else
		printf( "** BRAVO, l'ordinateur a perdu  **\n");

	return 0;
}
