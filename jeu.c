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

// Paramètres du jeu
#define LARGEUR_MAX 7 		// nb max de fils pour un noeud (= nb max de coups possibles)

#define TEMPS 5		// temps de calcul pour un coup avec MCTS (en secondes)

#define LIGNE 6
#define COLONNE 7

#define C 23	//compromis entre exploitation et exploration dans le calcule de la B valeur

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
    if(n->parent==NULL) printf("\t==RACINE==\n");
    printf("==joueur== :\t\t");
    (n->joueur==0)?printf("ORDI\n"): printf("HUMAN\n");
    printf("\n");
	afficheJeu(n->etat);
	printf("\n");
    if(n->parent!=NULL)//si ce n'est pas la racine, sinon SEG FAULT
        printf("==coups== :\t\tlig:%d, col:%d\n", n->coup->ligne, n->coup->colonne);


    printf("==nombre_enfant== :\t%d\n", n->nb_enfants);
    printf("==profondeur== :\t%d\n", n->profondeur);
    printf("==B_VALEUR== :\t\t%f\n", n->B);

}


// Créer un nouveau noeud en jouant un coup à partir d'un parent
// utiliser nouveauNoeud(NULL, NULL) pour créer la racine
Noeud * nouveauNoeud (Noeud * parent, Coup * coup ) {

	Noeud * noeud = (Noeud *)malloc(sizeof(Noeud));

	if ( parent != NULL && coup != NULL ) {
		noeud->etat = copieEtat ( parent->etat );
		jouerCoup ( noeud->etat, coup );
		noeud->coup = coup;
		noeud->joueur = AUTRE_JOUEUR(parent->joueur);
		noeud->profondeur = parent->profondeur +1;
	}else {
		noeud->etat = NULL;
		noeud->coup = NULL;
		noeud->joueur = 0;
		noeud->profondeur = 0;
	}
	noeud->parent = parent;
	noeud->nb_enfants = 0;

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

	do {
		freeNoeud(noeud->enfants[noeud->nb_enfants-1]);
		noeud->nb_enfants --;
	}while ( noeud->nb_enfants > 0 );
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

    srand(time(NULL));
    printf("\n\tLOOKING AT DEAP : %d\n\n", n->profondeur);
	//idée de reflection peut etre prendre le compte si noeud feuille ?

	//CAS 1 : lui même n'a pas été simulé on le choisi
	if (n->nb_enfants == 0 && !testFin(n->etat)){

		printf("\n____selection 1____\n");
		afficher_noeud(n);
		return n;
	}

	
	//CAS 2 : recherche du premier fils non simulé
	int k = n->nb_enfants;
	int i = 0;
	int simu = 0;
	int enfant = 0; //prendra la valeur de l'indice de l'enfant selectionné
	int enf[n->nb_enfants];
	memset(enf, 0, n->nb_enfants); //obliger de faire ça car n->nb_enfants n'est pas connu à l'avance par gcc
	while(i < k){ //peut être mettre un while à la place while(notsimu && i<k)
		enf[i] = 0;
        if(n->enfants[i]->nb_simus==0){
			simu = 1;
			enf[i] = 1;
		}
		i++;
	}
	
	if(simu){ // un des fils n'a jamais été simulé
		printf("\n____selection 2____\n");
        //selectionne un fils non developpé aléatoirement
        int r = rand()%n->nb_enfants;
        while(enf[r]==0){
            r = rand()%n->nb_enfants;
            enfant = r;
            printf("r:%d \t enf:%d\n", r, enf[r]);
        }
        n = n->enfants[r];
		afficher_noeud(n);
		return n->enfants[enfant];

	}else{ // CAS 3 : tous les fils ont été simulé, on va pouvoir descendre d'un étage par le fils ayant la plus grande B_valeur
		printf("\n____selection 3____\n");
        int i = 0;
		int Bmax = n->enfants[i]->B;
		Noeud * fils = n->enfants[i];

		for(i = 1; i < k; i++){
            printf("%d:%f\t", i, n->enfants[i]->B);
			if(n->enfants[i]->B > Bmax){
				Bmax = n->enfants[i]->B;
				fils =  n->enfants[i];
			}
		} printf("\n");
		return selectionner(fils);//récursion
	}

}

/**
 * Développe les noeuds d'une feuille en instanciant ses fils
 * et retourne un noeud fils random pour le simuler jusqu'a une fin de partie
 */
Noeud* developper(Noeud * n){

	printf("\n ___developper___ \n");
	//c'est une feuille on developpe pas
    //afficher_noeud(n);
	if(testFin(n->etat)!=NON){  // ATTENTION : il faut peut être gérer le cas != NON justement, si victoire alors le renvoyer et terminer
		printf("HERE");
		return n;
	}

	int k = 0; // nombre de coups possible
	Etat * e = copieEtat(n->etat);
	Coup** coups = coups_possibles(e, &k);

	for(int i = 0; i < k ; i ++){//parcours de ces coups jouable
		if(coups[i] != NULL && jouerCoup(e, coups[i])){ //si le coup est jouable
			Noeud* enfant = ajouterEnfant(n, coups[i]); // on récupère mais pourquoi ?
		}
		Etat * e = copieEtat(n->etat);
	}

	//si on arrive a une feuille dont l'état est final
	if(k==0){ //normalement impossible ...
		printf("WTF\n");
		return n;
	}

	srand(time(NULL));
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
		n->B 		= -(exploit + explo);
	} else {			// ligne MAX
		n->B		= exploit + explo;
	}

}

void mise_a_jour(Noeud* n, int res){

	n->nb_simus = n->nb_simus +1;
	if(res == 1 ){
		n->nb_victoires = n->nb_victoires +1;
	}

	if(n->parent==NULL){  //racine on arrete (il faut mettre a jour la racine)
		return;
	}else{
		B(n); //met a jour la B valeur du noeud
		mise_a_jour(n->parent, res);
	}
}


void simuler(Noeud * n){ // retourne le résultat de la partie simulé (-1,0,1)


	Etat * e = copieEtat(n->etat);
	int fini = testFin(e);
	int k = 0;

	while(!fini){
		Coup ** coups = coups_possibles(e, &k);
		if(k==0){ //plus de coups possible
			fini = 1; // ?????? 1 egal egalité dans l'enum
			continue;
		}
		srand(time(NULL));
		int i = rand()%k;

		jouerCoup(e , coups[i]);
		//afficheJeu(e);

		fini = testFin(e); // note à moi même : ne pas simuler si dejà une feuille
		//printf("%d\n",fini);
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
	int k = 0;
	coups = coups_possibles(racine->etat, &k);

	Noeud * enfant;
	while ( coups[k] != NULL) {
		enfant = ajouterEnfant(racine, coups[k]);
		k++;
	}

	int iter = 0;

	do {

		//Sélectionner
		Noeud * n = selectionner(racine);
		//Developper (selection du chemin/fils a prendre)
		Noeud * s = developper(n);
		//simuler ce chemin
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

	printf ("je joue le meilleur coup numero %d\n", enf);
	// Jouer le meilleur premier coup
	jouerCoup(etat, meilleur_coup );

	// Penser à libérer la mémoire :
	freeNoeud(racine);
	freeCoups(coups);
}



int main(void) {

	FinDePartie fin;

	// initialisation

	Noeud * n = nouveauNoeud(NULL,NULL);
	Etat * etat = etat_initial();
	n->etat = etat;

	ordijoue_mcts(n->etat, 1); //3s de temps max
	free( etat );
	free( n );


/*
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
*/

	return 0;
}
