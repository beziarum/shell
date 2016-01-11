#ifndef _EVALUATION_H
#define _EVALUATION_H

#include "Shell.h"

#include <stdbool.h>

/*
 *si Contient le contexte demandé pour la commande à lancer
 */
typedef struct Contexte {
    bool bg; //si elle doit etre en arrière plan
    int fdin;//si l'entrée standart de la commande doit être changé
    int fdout;//idem pour la sortie standard
    int fderr;//idem pour la sortie d'erreur
    int fdclose;//file descriptor qu'il faudra fermer avant de lancer la commande, -1 si aucun
    int* tube;//contient un tube à fermer par le père si besoin
} Contexte;

extern void initialiser_contexte(Contexte* c);
extern void copier_contexte(Contexte* c1, Contexte* c2);
extern int (*get_expr (expr_t e))(Expression*, Contexte*);
extern int evaluer_expr(Expression *e);
extern pid_t get_last_pid();
#endif
