#include <sys/wait.h>

#include "Shell.h"
#include "Evaluation.h"
#include "Commandes_Internes.h"


int expr_simple(Expression* e, Contexte* c);
int expr_bg(Expression* e, Contexte* c);
int expr_sequence(Expression* e, Contexte* c);
int expr_sequence_et(Expression* e, Contexte* c);
int expr_sequence_ou(Expression* e, Contexte* c);
int expr_sous_shell(Expression* e,Contexte* c);

typedef struct assoc {
    expr_t expr;
    int (*data) (Expression*,Contexte*);
} assoc;

assoc tab_expr[] = {{SIMPLE, expr_simple},
		    {BG, expr_bg},
		    {SEQUENCE, expr_sequence},
		    {SEQUENCE_ET, expr_sequence_et},
		    {SEQUENCE_OU, expr_sequence_ou},
		    {SOUS_SHELL, expr_sous_shell}};

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
    int (*intern)(char**)=get_intern(e->arguments[0]);
    if(intern!=NULL)
	return intern(e->arguments);
    else
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
	    if (c->bg)
		return 0;
	    int status;
	    waitpid(pid,&status,0);
	    return WIFEXITED(status) ? WEXITSTATUS(status) : WTERMSIG(status);
	}
    }
}

int expr_sequence_et (Expression* e, Contexte* c)
{
    int ret=get_expr(e->gauche->type)(e->gauche, c);
    if(ret != 0)
	return ret;
    else
	return get_expr(e->droite->type)(e->droite, c);
}

int expr_sequence_ou (Expression* e, Contexte* c)
{
    int ret=get_expr(e->gauche->type)(e->gauche, c);
    if(ret == 0)
	return ret;
    else
	return get_expr(e->droite->type)(e->droite, c);
}

int expr_sous_shell (Expression* e, Contexte* c)
{
  return get_expr(e->gauche->type)(e->gauche, c);
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
