#ifndef __JEU_H__
#define __JEU_H__

// Paramètres du jeu
#define LARGEUR_MAX 7 		// nb max de fils pour un noeud (= nb max de coups possibles)

#define TEMPS 30		// temps de calcul pour un coup avec MCTS (en secondes)

#define LIGNE 6
#define COLONNE 7

#define TRUE 1
#define FALSE 0

#define SEC 10	//temps que mets l'algo a trouver le meilleur coup

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
	float mu;
	
	int profondeur;

} Noeud;

// Copier un état
Etat * copieEtat( Etat * src );

// Etat initial
Etat * etat_initial();

void afficheJeu(Etat * etat);

// Nouveau coup
// TODO: adapter la liste de paramètres au jeu
Coup * nouveauCoup( int i, int j );

// Demander à l'humain quel coup jouer
Coup * demanderCoup ();

// Modifier l'état en jouant un coup
// retourne 0 si le coup n'est pas possible
int jouerCoup( Etat * etat, Coup * coup );

// Retourne une liste de coups possibles à partir d'un etat
// (tableau de pointeurs de coups se terminant par NULL)
Coup ** coups_possibles( Etat * etat , int *k);

/*
 * affiche un noeud sur la sortie standard
 */
void afficher_noeud(Noeud * n);

// Créer un nouveau noeud en jouant un coup à partir d'un parent
// utiliser nouveauNoeud(NULL, NULL) pour créer la racine
Noeud * nouveauNoeud (Noeud * parent, Coup * coup );

// Ajouter un enfant à un parent en jouant un coup
// retourne le pointeur sur l'enfant ajouté
Noeud * ajouterEnfant(Noeud * parent, Coup * coup);

void freeNoeud ( Noeud * noeud);


void freeCoups (Coup ** coups);

void freeCoupsExept (Coup ** coups, int k);


// Test si l'état est un état terminal
// et retourne NON, MATCHNUL, ORDI_GAGNE ou HUMAIN_GAGNE
FinDePartie testFin( Etat * etat );

/**
 * calcule la B valeur du noeud n et le met à jour dans la structure
*/
float B(Noeud* n);

/**
 *  retourne le noeud à partir duquel developper => toujours une feuille
*/
Noeud* selectionner(Noeud * n);

/**
 * Développe les noeuds d'une feuille en instanciant ses fils
 * et retourne un noeud fils random pour le simuler jusqu'a une fin de partie
 */
Noeud* developper(Noeud * n);

void mise_a_jour(Noeud* n, int res);

void simuler(Noeud * n, int opti);

/**
* Calcule et joue un coup de l'ordinateur avec MCTS-UCT
* en tempsmax secondes
*/
void ordijoue_mcts(Etat * etat, int tempsmax,  int opti); 

void sep();


#endif
