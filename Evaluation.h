#ifndef _EVALUATION_H
#define _EVALUATION_H

#include "Shell.h"

#include <stdbool.h>

typedef struct Contexte {
    bool bg;
    int fdin;
    int fdout;
  int fderr;
} Contexte;

extern void initialiser_contexte(Contexte* c);
extern void copier_contexte(Contexte* c1, Contexte* c2);
extern int (*get_expr (expr_t e))(Expression*, Contexte*);
extern int evaluer_expr(Expression *e);

#endif
