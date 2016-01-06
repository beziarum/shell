#include <sys/wait.h>

#include "Shell.h"
#include "Evaluation.h"
#include "Commandes_Internes.h"

typedef struct assoc {
    expr_t expr;
    int (*data) (Expression*,Contexte*);
} assoc;

assoc tab_expr[] = {{VIDE,NULL}};

int expr_not_implemented (Expression* e, Contexte* c)
{
    fprintf(stderr,"fonctionnalité non implémentée\n");
  
    return 1;
}

int expr_simple (Expression* e, Contexte* c)
{
    pid_t pid=fork();
    if(pid==0)
    {
	execvp(e->arguments[0],e->arguments);
	perror(e->arguments[0]);
	exit(1);
    }
    else
    {
	waitpid(pid,NULL,0);//en cas d'erreur, on ne récupère pas le code, à améliorer du coup
	return 0;
    }
}

int (*get_expr (expr_t expr)) (Expression*, Contexte*)
{
    int taille_tab_expr = sizeof (tab_expr)/sizeof (assoc);
    for (int i=0; i<taille_tab_expr ;i++)
	if(tab_expr[i].expr == expr)
	    return tab_expr[i].data;
    return expr_not_implemented;
}

int
evaluer_expr(Expression *e)
{
    Contexte* c=malloc(sizeof(Contexte));
    return get_expr(e->type)(e,c);
}
