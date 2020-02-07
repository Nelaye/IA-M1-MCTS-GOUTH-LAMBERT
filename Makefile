jeu : jeu.o
		gcc jeu.o -o jeu -lm
		
jeu.o : jeu.c jeuH.h
		gcc -c -Wall jeu.c
		
clean :rm*.o