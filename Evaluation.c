#include <sys/wait.h>

#include "Shell.h"
#include "Evaluation.h"
#include "Commandes_Internes.h"


int expr_simple(Expression* e, Contexte* c);
int expr_bg(Expression* e, Contexte* c);
int expr_sequence(Expression* e, Contexte* c);

typedef struct assoc {
    expr_t expr;
    int (*data) (Expression*,Contexte*);
} assoc;

assoc tab_expr[] = {{SIMPLE, expr_simple},
		    {BG, expr_bg},
		    {SEQUENCE, expr_sequence}};

int expr_not_implemented (Expression* e, Contexte* c)
{
    fprintf(stderr,"fonctionnalité non implémentée\n");
    
    return 1;
}

int expr_sequence(Expression* e, Contexte* c)
{
    get_expr(e->gauche->type)(e->gauche, c);
    return get_expr(e->droite->type)(e->droite, c);
}

int expr_bg (Expression* e, Contexte* c)
{
    c->bg=true;
    return get_expr(e->gauche->type)(e->gauche, c);
}

int expr_simple (Expression* e, Contexte* c)
{
    pid_t pid=fork();
    if(pid==0)
    {
	int (*intern)(char**)=get_intern(e->arguments[0]);
	if(intern!=NULL)
	    exit(intern(e->arguments));
	else
	{
	    execvp(e->arguments[0],e->arguments);
	    perror(e->arguments[0]);
	    exit(1);
	}
    }
    else
    {
	if (c->bg)
	    return 0;
	int status;
	waitpid(pid,&status,0);
	return WIFEXITED(status) ? WEXITSTATUS(status) : WTERMSIG(status);
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
    c->bg=false;
    return get_expr(e->type)(e,c);
}
