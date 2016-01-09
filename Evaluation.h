#ifndef _EVALUATION_H
#define _EVALUATION_H

#include "Shell.h"

#include <stdbool.h>

typedef struct Contexte {
    bool bg;
    int fdin;
    int fdout;
    int fdclose;//file descriptor qu'il faudra fermer avant de lancer la commande, -1 si aucun
    bool ispipped;
    int* tube;
} Contexte;

extern void initialiser_contexte(Contexte* c);
extern void copier_contexte(Contexte* c1, Contexte* c2);
extern int (*get_expr (expr_t e))(Expression*, Contexte*);
extern int evaluer_expr(Expression *e);

#endif
