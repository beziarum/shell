#ifndef _EVALUATION_H
#define _EVALUATION_H

#include "Shell.h"

typedef struct Contexte {
} Contexte;

extern int (*get_expr (expr_t e))(Expression*, Contexte*);
extern int evaluer_expr(Expression *e);

#endif
