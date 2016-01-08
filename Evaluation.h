#ifndef _EVALUATION_H
#define _EVALUATION_H

#include "Shell.h"

#include <stdbool.h>

typedef struct Contexte {
    bool bg;
  int ftdin;
} Contexte;

extern int initaliser_contexte(Contexte* c);
extern int (*get_expr (expr_t e))(Expression*, Contexte*);
extern int evaluer_expr(Expression *e);

#endif
