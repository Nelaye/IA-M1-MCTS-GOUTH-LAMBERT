/*
	Canvas pour algorithmes de jeux à 2 joueurs

	joueur 0 : humain
	joueur 1 : ordinateur

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <string.h> //for memset
#include <limits.h>

// Paramètres du jeu
#define LARGEUR_MAX 7 		// nb max de fils pour un noeud (= nb max de coups possibles)

#define TEMPS 5		// temps de calcul pour un coup avec MCTS (en secondes)

#define LIGNE 6
#define COLONNE 7

#define TRUE 1
#define FALSE 0

#define SEC 2	//temps que mets l'algo a trouver le meilleur coup

#define C 2	//compromis entre exploitation et exploration dans le calcule de la B valeur

// macros
#define AUTRE_JOUEUR(i) (1-(i))
#define min(a, b)       ((a) < (b) ? (a) : (b))
#define max(a, b)       ((a) < (b) ? (b) : (a))

// Critères de fin de partie
typedef enum {NON, MATCHNUL, ORDI_GAGNE, HUMAIN_GAGNE } FinDePartie;

// Definition du type Etat (état/position du jeu)
typedef struct EtatSt {

	int joueur; // à qui de jouer ?

	char plateau[LIGNE][COLONNE];

} Etat;

// Definition du type Coup
typedef struct {

	int ligne;
	int colonne;

} Coup;

// Copier un état
Etat * copieEtat( Etat * src ) {

	Etat * etat = (Etat *)malloc(sizeof(Etat));
	etat->joueur = src->joueur;

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


	int i,j;
	for (i=0; i< LIGNE; i++){
		for ( j=0; j<COLONNE; j++){
			etat->plateau[i][j] = ' ';
		}
	}
    
    etat->joueur = 0;
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

	//printf("Dans le nouveauCoup\n");
	fflush(0);
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
	fflush(0);
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
Coup ** coups_possibles( Etat * etat , int *k) {

	Coup ** coups = (Coup **) malloc((1+LARGEUR_MAX) * sizeof(Coup *) );

	int i,j;
	i = 0;
	*k = 0;

	for (j=0; j < COLONNE; j++) {
		//printf("colonne %d\n",j);
		while(i<LIGNE && etat->plateau[i][j] != ' '){ // je remonte dans le plateau jusqu'a avoir un ligne où c'est libre
			i++;
			//printf("recherche d'une case libre \n");
		}
		if(i<LIGNE){ // si la colonne n'est pas remplie j'ajoute un coup
		//printf("ajout \n");
			coups[*k] = nouveauCoup(i,j);
			//printf("k=%d\n",*k);
			*k = *k +1;

		}
		i = 0;
	}

	coups[*k] = NULL;
	//printf("il y a %d\n",*k);

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
	double B;
	int profondeur;

} Noeud;

/*
 * affiche un noeud sur la sortie standard
 */
void afficher_noeud(Noeud * n){

	printf("\n=== AFFICHAGE D'UN NOEUD ===\n");
    if(n->parent==NULL) 
		printf("\t==RACINE==\n");

    printf("==joueur== :\t\t");

    (n->joueur==0)?printf("ORDI\n"): printf("HUMAN\n");

    printf("\n");
	afficheJeu(n->etat);
	printf("\n");

    if(n->parent!=NULL)//si ce n'est pas la racine, sinon SEG FAULT
        printf("==coups== :\t\tlig:%d, col:%d\n", n->coup->ligne, n->coup->colonne);


    printf("==nombre_enfant== :\t%d\n", n->nb_enfants);
    printf("==profondeur== :\t%d\n", n->profondeur);
    printf("==B_VALEUR== :\t\t%f\n\n", n->B);

}


// Créer un nouveau noeud en jouant un coup à partir d'un parent
// utiliser nouveauNoeud(NULL, NULL) pour créer la racine
Noeud * nouveauNoeud (Noeud * parent, Coup * coup ) {

	

	Noeud * noeud = (Noeud *)malloc(sizeof(Noeud));

	if ( parent != NULL && coup != NULL ) {
		//printf("parent->B : %f, coup.colonne: %d\n",parent->B,coup->colonne);
		noeud->etat = copieEtat ( parent->etat );
		jouerCoup ( noeud->etat, coup );
		noeud->coup = coup;
		noeud->joueur = AUTRE_JOUEUR(parent->joueur);
		noeud->profondeur = parent->profondeur +1;
	}else {
		//noeud initial
		noeud->etat = NULL;
		noeud->coup = NULL;
		noeud->joueur = 0;
		noeud->profondeur = 0;
	}
	noeud->parent = parent;
	noeud->nb_enfants = 0;
	//noeud->enfants = (Noeud **) malloc((1+LARGEUR_MAX) * sizeof(Noeud * ) );
	/*for(int i=0; i<LARGEUR_MAX; i++){
		noeud->enfants[i] = (Noeud *) malloc((1+LARGEUR_MAX) * sizeof(Noeud ));
	}*/
	noeud->enfants[LARGEUR_MAX] = NULL;

	// POUR MCTS:
	noeud->nb_victoires = 0;
	noeud->nb_simus = 0;
    noeud->B = 0.;
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

	while ( noeud->nb_enfants > 0 ) {
		freeNoeud(noeud->enfants[noeud->nb_enfants-1]);
		noeud->nb_enfants --;
	}
	if ( noeud->coup != NULL)
		free(noeud->coup);

	free(noeud);
}

void freeCoups (Coup ** coups){
    int i = 0;
    while(coups[i] != NULL){
        free(coups[i]);
        i++;
    }
    free(coups);

}

void freeCoupsExept (Coup ** coups, int k){
    int i = 0;
    while(coups[i] != NULL){
        if(i!=k){
            free(coups[i]);
            i++;
        }

    }
    free(coups);

}

// Test si l'état est un état terminal
// et retourne NON, MATCHNUL, ORDI_GAGNE ou HUMAIN_GAGNE
FinDePartie testFin( Etat * etat ) {


	// tester si un joueur a gagné
	int i,j,k,n = 0;



	for (i=0;i < LIGNE; i++) {
		for (j=0; j < COLONNE; j++) {
			//printf("\nje suis dans le cas i=%d et j=%d\n",i,j);
			if (etat->plateau[i][j] != ' ') {
				//printf("c'est vide\n");
				n++;	// nb coups joués

				// lignes
				k=0;

				while ( k < 4 && i+k < COLONNE && etat->plateau[i+k][j] == etat->plateau[i][j] ){
					//printf("while ligne\n");
					k++;
				}

				if ( k == 4 ){
					//printf("LIGNE");
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;
				}

				// colonnes
				k=0;
				while ( k < 4 && j+k < LIGNE && etat->plateau[i][j+k] == etat->plateau[i][j] ){
					//printf("while colonne\n");
					k++;
				}

				if ( k == 4 ){
					//printf("COLONNE");
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;
				}

				// diagonales
				k=0;
				while ( k < 4 && i+k < COLONNE && j+k < LIGNE && etat->plateau[i+k][j+k] == etat->plateau[i][j] ){
					//printf("while diag 1\n");
					k++;
				}
				if ( k == 4 ){
					//printf("DIAG 1");
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;
				}

				k=0;
				while ( k < 4 && i+k < COLONNE && j-k >= LIGNE && etat->plateau[i+k][j-k] == etat->plateau[i][j] ){
					//printf("while diag 2\n");
					k++;
				}
				if ( k == 4 ){
					//printf("DIAG 2");
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;
				}
			}
		}
	}


	// et sinon tester le match nul
	if (n == LIGNE*COLONNE)
		return MATCHNUL;

	return NON;
}


/**
 *  retourne le noeud à partir duquel developper => toujours une feuille
*/
Noeud* selectionner(Noeud * n){


    //printf("\n\tProfondeur actuelle : %d\n", n->profondeur);
	//idée de reflection peut etre prendre le compte si noeud feuille ?

	//CAS 1 : lui même n'a pas été simulé on le choisi
	if (n->nb_enfants == 0){

		//printf("\nCas : Aucun enfant mais pas feuille finale\n");
		//afficher_noeud(n);
		return n;
	}

	
	//CAS 2 : recherche du premier fils non simulé
	int k = n->nb_enfants;
	int i = 0;
	int oneNotSimu = 0;
	int indiceEnfant = 0; //prendra la valeur de l'indice de l'enfant selectionné
	int isNotSimu[n->nb_enfants];// tableau contenant le nombre de simulation pour chaque enfant
	//memset(isNotSimu, 0, n->nb_enfants); //obliger de faire ça car n->nb_enfants n'est pas connu à l'avance par gcc

	while(i < k){ //peut être mettre un while à la place while(notsimu && i<k)
		isNotSimu[i] =0;
        if(n->enfants[i]->nb_simus==0){
			oneNotSimu = 1;
			isNotSimu[i] = 1;
		}
		i++;
	}
	
	if(oneNotSimu){ // un des fils n'a jamais été simulé
		//printf("\nCas : un fils n'a jamais été simulé\n");
        //selectionne un fils non developpé aléatoirement
        int r = 0;
        do{
            r = rand()%n->nb_enfants;
            indiceEnfant = r;
            //printf("le random %d :isNotSimu[]= %d\n", r, isNotSimu[r]);
        }while(isNotSimu[r]==0);

		//afficher_noeud(n->enfants[indiceEnfant]);
		return n->enfants[indiceEnfant];

	}else{ // CAS 3 : tous les fils ont été simulé, on va pouvoir descendre d'un étage par le fils ayant la plus grande B_valeur
		//printf("\nCas : tous les fils simulés\n");
        int i = 0;
		int Bmax = n->enfants[i]->B;
		Noeud * fils = n->enfants[i];
 		//printf("Les B-val : %d:%f\t", i, n->enfants[i]->B);

		for(i = 1; i < k; i++){
            //printf("%d:%f\t", i, n->enfants[i]->B);
			if(n->enfants[i]->B > Bmax){
				Bmax = n->enfants[i]->B;
				fils =  n->enfants[i];
			}
		} 
		//printf("\nRECURSION SELECTION\n");
		return selectionner(fils);//récursion
	}

}

/**
 * Développe les noeuds d'une feuille en instanciant ses fils
 * et retourne un noeud fils random pour le simuler jusqu'a une fin de partie
 */
Noeud* developper(Noeud * n){

	//c'est une feuille on developpe pas
    //afficher_noeud(n);
	if(testFin(n->etat)!=NON){//il va partir pour mettre a jour lereste de l'arbre
		return n;
	}

	int k = 0; // nombre de coups possible
	Etat * e = copieEtat(n->etat);
	Coup** coups = coups_possibles(e, &k);

	for(int i = 0; i < k ; i ++){//parcours de ces coups jouable
		if(coups[i] != NULL && jouerCoup(e, coups[i])){ //si le coup est jouable
			Noeud* enfant = ajouterEnfant(n, coups[i]);
			//printf("enfant %d\n",i);
			//afficher_noeud(enfant);
		}
		Etat * e = copieEtat(n->etat);
	}

	//printf("\nnombre de coup k = %d\n",k);
	//si on arrive a une feuille dont l'état est final
	/*
	if(k==0){ //normalement impossible ...
		printf("WTF\n");
		return n;
	}
		*/
	int r = rand()%k;
	//printf("R = %d\n",r);
    free(e);
    //freeCoups(coups); //surtout pas free les coups sinon on perd la référence du coups dans le noeud enfant
    //et cela cause un SEG FAULT
	return n->enfants[r];
}

/**
 * calcule la B valeur du noeud n et le met à jour dans la structure
*/

void B(Noeud* n){
	//printf ("\n\t==calclul du noeuf profondeur %d==\n", n->profondeur);
	//exploitation

	float exploit 	= (n->nb_victoires)/(n->nb_simus);

	//exploration
	Noeud* parent 	= n->parent;

	float ratio  	= (log(parent->nb_simus)) / (n->nb_simus);
	//printf("(%f)", ratio);
	float explo 	= C*(sqrt(ratio));

	//printf("\t%f + %f = %f\n",exploit, explo, (exploit + explo));
	if(n->profondeur%2 == 1){// ligne MIN
		n->B 		= (-exploit) + explo;
	} else {			// ligne MAX
		n->B		= exploit + explo;
	}

}

void mise_a_jour(Noeud* n, int res){

	n->nb_simus = n->nb_simus +1;
	if(res == ORDI_GAGNE ){
		n->nb_victoires = n->nb_victoires +5;
	}

	if(n->parent==NULL){  //racine on arrete (il faut mettre a jour la racine)
		return;
	}else{
		mise_a_jour(n->parent, res);
		B(n); //met a jour la B valeur du noeud
	}
}


void simuler(Noeud * n){ // retourne le résultat de la partie simulé (-1,0,1)


	Etat * e = copieEtat(n->etat);
	int fini = testFin(e);
	int k = 0;
	int profondeur = 0;
	//printf("\nsimulation\n");
	while(!fini){
		Coup ** coups = coups_possibles(e, &k);
		/*if(k==0){ //plus de coups possible
			fini = 1; // ?????? 1 egal egalité dans l'enum
			continue;
		}*/
		
		int i = rand()%k;
		//printf("k= %d, i= %d\n",k,i);

		jouerCoup(e , coups[i]);
		//afficheJeu(e);

		fini = testFin(e); // note à moi même : ne pas simuler si dejà une feuille
		//printf("%d",fini);
		//printf("Fini %d\n",fini);
		//printf("Coup numéro : %d\n", ++profondeur);

		freeCoups(coups);
	}
	free (e);

	mise_a_jour(n, fini);
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
	int k = 0;//nb enfant
	coups = coups_possibles(racine->etat, &k);
	/*printf("%d\n",k);
	for(int c=0; c<k; c++){
		printf("ligne: %d, colonne: %d\n",coups[c]->ligne,coups[c]->colonne);
	}*/
	Noeud * enfant;
	int i = 0;
	while ( coups[i] != NULL) {
		//printf("ligne: %d, colonne: %d\n",coups[i]->ligne,coups[i]->colonne);
		enfant = ajouterEnfant(racine, coups[i]);
		//simuler(enfant);
		i++;
	}

	int iter = 0;

	do {
		//printf("\n%d\n",iter);
		//Sélectionner
		/*printf("**SELECTION**\n");
		fflush(0);*/
		Noeud * n = selectionner(racine);

		//Developper (selection du chemin/fils a prendre)
		/*printf("**DEVELOPPEMENT**\n");
		fflush(0);*/
		Noeud * s = developper(n);

		//simuler ce chemin
		/*printf("**SIMULATION**\n");
		fflush(0);*/
		simuler(s); //simule et met a jour les B valeurs
		
		toc = clock();
		temps = (int)( ((double) (toc - tic)) / CLOCKS_PER_SEC );
		iter ++;
	} while ( temps < tempsmax );

	/* fin de l'algorithme  */
	float BMAX = -9999; //-inf
	int enf = 0;
	for(int i = 0; i < k; i++){
		if(racine->enfants[i]->B > BMAX){
			BMAX = racine->enfants[i]->B;
			enf = i;
		}
	}

	meilleur_coup = coups[enf];

	printf ("ordi joue le coup numero %d à la ligne %d et colonne %d\n", enf, coups[enf]->ligne, coups[enf]->colonne);
	// Jouer le meilleur premier coup
	jouerCoup(etat, meilleur_coup );
	
	printf("coup joué\n");

	// Penser à libérer la mémoire :
	//freeCoups(coups);
	printf("free coups\n");
	//freeNoeud(racine); // A CORRIGER TODO TODO
	printf("free neoud\n");
	fflush(0);
}



int main(void) {
	srand(time(NULL));
	FinDePartie fin;

	// initialisation

	Etat * etat = etat_initial();

	/*Noeud * children = developper(selectionner(n));
	printf("j'ai choisi :\n");
	afficher_noeud(children);
	
	simuler(children);*/
	/*ordijoue_mcts(n->etat, 1); //3s de temps max
	free( etat );
	free( n );
*/


	// Choisir qui commence :
	/*printf("Qui commence (0 : humain, 1 : ordinateur) ? ");
	scanf("%d", &(etat->joueur) );
	*/

	Coup * coup;
	while ( fin == NON ) { // boucle de jeu

		printf("\n");
		afficheJeu(etat);
		printf("joueur : %d\n",etat->joueur );

		if ( etat->joueur != 0 ) {// tour de l'humain
			do{
				printf("ici\n");
				fflush(0);
				coup = demanderCoup();
				printf("là\n");
				fflush(0);
			}while( !jouerCoup(etat, coup) );
		}
		else {// tour de l'Ordinateur
			ordijoue_mcts(etat, SEC);
		}
		fin = testFin( etat );
		//free(coup);
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
