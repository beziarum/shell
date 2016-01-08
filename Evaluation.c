#include <sys/wait.h>
#include <fcntl.h>
#include "Shell.h"
#include "Evaluation.h"
#include "Commandes_Internes.h"


int expr_simple(Expression* e, Contexte* c);
int expr_bg(Expression* e, Contexte* c);
int expr_sequence(Expression* e, Contexte* c);
int expr_sequence_et(Expression* e, Contexte* c);
int expr_sequence_ou(Expression* e, Contexte* c);
int expr_sous_shell(Expression* e,Contexte* c);
int expr_redirection_o(Expression* e,Contexte* c);
int expr_redirection_in(Expression* e,Contexte* c);

void appliquer_contexte(Contexte* c, bool save);

typedef struct assoc {
    expr_t expr;
    int (*data) (Expression*,Contexte*);
} assoc;

assoc tab_expr[] = {{SIMPLE, expr_simple},
		    {BG, expr_bg},
		    {SEQUENCE, expr_sequence},
		    {SEQUENCE_ET, expr_sequence_et},
		    {SEQUENCE_OU, expr_sequence_ou},
		    {REDIRECTION_I, expr_redirection_in},
		    {REDIRECTION_O, expr_redirection_o},
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

void swapfd(int fd1, int fd2)
{
    int tmpfd=fd1;
}

int expr_simple (Expression* e, Contexte* c)
{
    int (*intern)(char**)=get_intern(e->arguments[0]);
    if(intern!=NULL && c->bg!=true)
    {
	appliquer_contexte(c,true);
	int ret=intern(e->arguments);
	appliquer_contexte(c,false);
	return ret;
    }
    else
    {
	pid_t pid=fork();
	if(pid==0)
	{
	    appliquer_contexte(c,false);
	    if(intern != NULL)
	    {
		return intern(e->arguments);
	    }
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
}	

int expr_redirection_o (Expression* e, Contexte* c)
{
    c->fdout=open(e->arguments[0],O_WRONLY|O_CREAT|O_TRUNC,0660);
    return get_expr(e->gauche->type)(e->gauche,c);
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

int expr_redirection_in(Expression* e, Contexte* c)
{
    c->fdin = open(e->arguments[0],O_RDONLY);
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

void initaliser_contexte(Contexte* c)
{
    c->bg=false;
    c->fdin=STDIN_FILENO;
    c->fdout=STDOUT_FILENO;
}

void appliquer_contexte(Contexte* c,bool save)
{
    if(c->fdin != STDIN_FILENO)
    {
	
	int tmp;
	if(save)
	    tmp=dup(STDIN_FILENO);
	dup2(c->fdin,STDIN_FILENO);
	if(save)
	    c->fdin=tmp;
	else
	    close(c->fdin);
	
    }
    if(c->fdout != STDOUT_FILENO)
    {
	int tmp;
	if(save)
	    tmp=dup(STDOUT_FILENO);
	dup2(c->fdout,STDOUT_FILENO);
	if(save)
	    c->fdin=tmp;
	else
	    close(c->fdout);
    }
}

int
evaluer_expr(Expression *e)
{
    Contexte* c=malloc(sizeof(Contexte));
    initaliser_contexte(c);
    return get_expr(e->type)(e,c);
}
