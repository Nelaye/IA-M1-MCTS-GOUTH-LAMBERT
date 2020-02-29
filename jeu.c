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

#include "jeuH.h"

Etat * copieEtat( Etat * src ) {

	Etat * etat = (Etat *)malloc(sizeof(Etat));
	etat->joueur = src->joueur;

	assert(&(etat->joueur) != &(src->joueur));	

	int i,j;
	for (i=0; i< LIGNE; i++){
		for ( j=0; j<COLONNE; j++){
			etat->plateau[i][j] = src->plateau[i][j];
		}
	}

	return etat;
}

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

// Etat gagne ligne 
Etat * etat_ligne() {

	Etat * etat = (Etat *)malloc(sizeof(Etat));


	int i,j;
	for (i=0; i< LIGNE; i++){
		for ( j=0; j<COLONNE; j++){
			etat->plateau[i][j] = ' ';
		}
	}

	etat->plateau[0][0] = 'X';
	etat->plateau[1][0] = 'X';
	etat->plateau[2][0] = 'X';
	etat->plateau[3][0] = 'X';
    
    etat->joueur = 0;
	return etat;
}

// Etat gagne diag haut 
Etat * etat_diag_haut() {

	Etat * etat = (Etat *)malloc(sizeof(Etat));


	int i,j;
	for (i=0; i< LIGNE; i++){
		for ( j=0; j<COLONNE; j++){
			etat->plateau[i][j] = ' ';
		}
	}

	etat->plateau[0][0] = 'X';
	etat->plateau[1][1] = 'X';
	etat->plateau[2][2] = 'X';
	etat->plateau[3][3] = 'X';
    
    etat->joueur = 0;
	return etat;
}

// Etat gagne diag bas 
Etat * etat_diag_bas() {

	Etat * etat = (Etat *)malloc(sizeof(Etat));


	int i,j;
	for (i=0; i< LIGNE; i++){
		for ( j=0; j<COLONNE; j++){
			etat->plateau[i][j] = ' ';
		}
	}

	etat->plateau[0][3] = 'X';
	etat->plateau[1][2] = 'X';
	etat->plateau[2][1] = 'X';
	etat->plateau[3][0] = 'X';
    
    etat->joueur = 0;
	return etat;
}

// Etat gagne haut 
Etat * etat_haut() {

	Etat * etat = (Etat *)malloc(sizeof(Etat));


	int i,j;
	for (i=0; i< LIGNE; i++){
		for ( j=0; j<COLONNE; j++){
			etat->plateau[i][j] = ' ';
		}
	}

	etat->plateau[0][3] = 'X';
	etat->plateau[0][2] = 'X';
	etat->plateau[0][1] = 'X';
	etat->plateau[0][0] = 'X';
    
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

	//liberer le free par rapport au k | | | k | | | | NULL  |
	//printf("il y a %d\n",*k);

	return coups;
}

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
    //printf("==B_VALEUR== :\t\t%f\n\n", n->B);

}

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
    noeud->mu = 0.;
	return noeud;
}

Noeud * ajouterEnfant(Noeud * parent, Coup * coup) {
	Noeud * enfant = nouveauNoeud (parent, coup ) ;
	parent->enfants[parent->nb_enfants] = enfant;
	parent->nb_enfants++;
	return enfant;
}

void freeNoeud ( Noeud * noeud) {
	if (noeud != NULL){
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
}

void freeCoups (Coup ** coups){
    int i = 0;
    while(coups[i] != NULL){
        free(coups[i]);
        i++;
    }

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
				while ( k < 4 && i+k < LIGNE && etat->plateau[i+k][j] == etat->plateau[i][j] ){
					//printf("while ligne\n");
					k++;
					
				}

				if ( k == 4 ){
					//printf("LIGNE");
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;
				}

				// colonnes
				k=0;
				while ( k < 4 && j+k < COLONNE && etat->plateau[i][j+k] == etat->plateau[i][j] ){
					//printf("while colonne\n");
					k++;
				}

				if ( k == 4 ){
					//printf("COLONNE");
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;
				}

				// diagonales
				k=0;
				while ( k < 4 && i+k < LIGNE && j+k < COLONNE && etat->plateau[i+k][j+k] == etat->plateau[i][j] ){
					//printf("while diag 1\n");
					k++;
				}
				if ( k == 4 ){
					//printf("DIAG HAUT");
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;
				}

				k=0;
				while ( k < 4 && i+k < LIGNE && j-k >= 0 && etat->plateau[i+k][j-k] == etat->plateau[i][j] ){
					//printf("while diag 2\n");
					k++;
				}
				if ( k == 4 ){
					//printf("DIAG BAS");
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

float B(Noeud* n){
	//printf ("\n\t==calclul du noeuf profondeur %d==\n", n->profondeur);

	float res;

	//exploitation
	float exploit 	= (n->nb_victoires)/(n->nb_simus);//mu

	//exploration
	Noeud* parent 	= n->parent;

	float ratio  	= (log(parent->nb_simus)) / (n->nb_simus);
	//printf("(%f)", ratio);
	float explo 	= C*(sqrt(ratio));

	//printf("\t%f + %f = %f\n",exploit, explo, (exploit + explo));
	if(n->profondeur%2 != 1){// ligne MIN
		res		= (-exploit) + explo;
	} else {			// ligne MAX
		res		= exploit + explo;
	}

	return res;
}

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
		float Bmax = B(n->enfants[i]);
		Noeud * fils = n->enfants[i];
 		//printf("Les B-val : %d:%f\t", i, n->enfants[i]->B);

		for(i = 1; i < k; i++){
            //printf("%d:%f\t", i, n->enfants[i]->B);
			if(B(n->enfants[i]) > Bmax){
				Bmax = B(n->enfants[i]);
				fils =  n->enfants[i];
			}
		} 
		//printf("\nRECURSION SELECTION\n");
		return selectionner(fils);//récursion
	}

}

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
		} else {
			free(coups[i]);
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

void mise_a_jour(Noeud* n, int res){

	/*if(res == ORDI_GAGNE && n->joueur == 0){
		n->nb_victoires = n->nb_victoires +1;
	}*/

	if(res == ORDI_GAGNE ){
		n->nb_victoires = n->nb_victoires +1;
	}

	/*if(res == HUMAIN_GAGNE && n->joueur == 1){
		n->nb_victoires = n->nb_victoires +1;
	}*/

	n->mu = ((n->nb_simus*n->mu) + res)/n->nb_simus+1;// µ(i) = N(i)µ(i)+res / N(i)+1
	n->nb_simus = n->nb_simus +1; //N(i)+1

	if(n->parent==NULL){  //racine on arrete (il faut mettre a jour la racine)
		return;
	}else{
		mise_a_jour(n->parent, res);
	}
}

void simuler(Noeud * n, int opti){ // retourne le résultat de la partie simulé (-1,0,1)


	Etat * e = copieEtat(n->etat);
	int fini = testFin(e);
	int k = 0;
	int i = -1;
	int profondeur = 0;
	//printf("\nsimulation\n");

	while(!fini){
		Coup ** coups = coups_possibles(e, &k);
		//printf("k= %d, i= %d\n",k,i);

		/*if(k==0){ //plus de coups possible
			fini = 1; // ?????? 1 egal egalité dans l'enum
			continue;
		}*/
		
		if( !opti ){ // sans optimisation 
			i = rand()%k;
		}else{ // avec optimisation 

			Etat * tmp = copieEtat(n->etat);
			for(int j = 0; j < k; j++){ // je parcours et je cherche une coup gagnant en 1 coup
				if( testFin( joueurCoup(tmp,coups[j]) ) == ORDI_GAGNE ){
					i = j;
					break;
				}
				free(tmp);
				tmp = copieEtat(n->etat);
			}
			if( i == -1){ // si aucun coup gagnant alors choix aléatoir
				i = rand()%k;
			}
			free(tmp);
		}
		

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

void ordijoue_mcts(Etat * etat, int tempsmax, int opti) {

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

		//Developper (selection du chemin/fils a prendre)rrors, rerun with: -v
		/*printf("**DEVELOPPEMENT**\n");
		fflush(0);*/
		Noeud * s = developper(n);

		//simuler ce chemin
		/*printf("**SIMULATION**\n");
		fflush(0);*/
		simuler(s,opti); //simule et met a jour les B valeurs
		
		toc = clock();
		temps = (int)( ((double) (toc - tic)) / CLOCKS_PER_SEC );
		iter ++;
	} while ( temps < tempsmax );

	/* fin de l'algorithme  */


	/** CHOIX DU MEILLEUR COUP **/ 


	float RATIO_MAX = (float)racine->enfants[0]->nb_victoires/(float)racine->enfants[0]->nb_simus;
	int enf = 0;
	printf("%f de %d\n", RATIO_MAX, enf);
	for(int i = 1; i < k; i++){
		float ratio = (float)racine->enfants[i]->nb_victoires/(float)racine->enfants[i]->nb_simus;
		printf("%f de %d\n", ratio, i);
		if(ratio > RATIO_MAX){
			RATIO_MAX = ratio;
			enf = i;
		}
	}

	printf("Au final on choisi : %f de %d\n", RATIO_MAX, enf);

	meilleur_coup = coups[enf];

	//printf ("ordi joue le coup numero %d à la ligne %d et colonne %d\n", enf, coups[enf]->ligne, coups[enf]->colonne);
	// Jouer le meilleur premier coup
	jouerCoup(etat, meilleur_coup );
	


	//QUESTION 1//
	int nb_simu = racine->nb_simus;
	float proba = (float)racine->nb_victoires/(float)nb_simu;
	printf("%d / %d == %f \n", nb_simu, racine->nb_victoires, proba);
	printf ("\nNombre de simu total effectue : %d\t proba de gagner : %f \n", nb_simu, proba);


	// Penser à libérer la mémoire :
	//freeCoups(coups);
	printf("free coups\n");
	freeNoeud(racine); // A CORRIGER TODO TODO
	printf("free neoud\n");
	fflush(0);


}

void sep(){
	printf("\n==================================\n");
}

void procedure_test_etat(){

	Etat * vide = etat_initial();
	afficheJeu(vide);
	FinDePartie fin = testFin(vide);
	
	printf("%d", fin);

	sep();
 

	Etat * haut = etat_haut();
	afficheJeu(haut);
	fin = testFin(haut);

	printf("%d", fin);

	sep();

	Etat * ligne = etat_ligne();
	afficheJeu(ligne);
	fin = testFin(ligne);

	printf("%d", fin);

	sep();

	Etat * d1 = etat_diag_haut();
	afficheJeu(d1);
	fin = testFin(d1);

	printf("%d", fin);

	sep();

	Etat * d2 = etat_diag_bas();
	afficheJeu(d2);
	fin = testFin(d2);

	printf("%d", fin);

	sep();

}


int main(int argc, char * args[]) {

	int opti = 0;
	if(args[0] == 'y'){
		opti = 1;
	}

	srand(time(NULL));
	FinDePartie fin = 0; //NON

	//procedure_test_etat();
	//initialisation

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
	int jouer_coup = TRUE;
	while ( fin == NON ) { // boucle de jeu

		printf("\n");
		afficheJeu(etat);
		printf("joueur : %d\n",etat->joueur );

		if ( etat->joueur == 0 ) {// tour de l'humain
			do{
				
				printf("ici\n");
				fflush(0);
				coup = demanderCoup();
				printf("là\n");
				fflush(0);
				jouer_coup = jouerCoup(etat, coup);
				free(coup);
			}while(!jouer_coup);
		}
		else {// tour de l'Ordinateur
			ordijoue_mcts(etat, SEC, opti);
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

	
	//free(etat);
	return 0;
}
