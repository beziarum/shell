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
int expr_redirection_a(Expression* e, Contexte* c);
int expr_vide(Expression* e, Contexte* c);
int expr_pipe(Expression* e, Contexte* c);
int expr_redirection_er(Expression* e, Contexte* c);


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
		    {REDIRECTION_A, expr_redirection_a},
		    {SOUS_SHELL, expr_sous_shell},
		    {VIDE, expr_vide},
		    {REDIRECTION_E, expr_redirection_er},
		    {PIPE, expr_pipe}};


int expr_not_implemented (Expression* e, Contexte* c)
{
    fprintf(stderr,"fonctionnalité non implémentée\n");
    
    return 1;
}

int expr_vide(Expression* e, Contexte* c){
  return EXIT_SUCCESS;
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
    if(intern!=NULL && c->bg!=true)
    {
	appliquer_contexte(c,true);
	int ret=intern(e->arguments);
	appliquer_contexte(c,false);
	if(c->ispipped)
	    exit(ret);
	else
	    return ret;
	    
    }
    else
    {
	pid_t pid;
	if(!c->ispipped)
	    pid=fork();
	if(pid==0 || c->ispipped)
	{
	    appliquer_contexte(c,false);
	    if(intern != NULL)
	    {
		exit(intern(e->arguments));
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
	    if(c->tube!=NULL)
	    {
		close(c->tube[0]);
		close(c->tube[1]);
	    }
	    if (c->bg)
		return 0;
	    int status;
	    waitpid(pid,&status,0);
	    return WIFEXITED(status) ? WEXITSTATUS(status) : WTERMSIG(status);
	}
    }
}	

int expr_pipe(Expression* e, Contexte* c)
{
    Contexte* c2=malloc(sizeof(Contexte));
    copier_contexte(c,c2);

    int* tube=c2->tube=malloc(2*sizeof(int));
    pipe(tube);

    c->bg=true;
    c->fdout=tube[1];
    c->fdclose=tube[0];
    
    c2->fdin=tube[0];
    c2->fdclose=tube[1];

    get_expr(e->gauche->type)(e->gauche,c);
    return get_expr(e->droite->type)(e->droite,c2);
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

int expr_redirection_a(Expression* e, Contexte* c)
{
  c->fdout = open(e->arguments[0],O_WRONLY|O_CREAT|O_APPEND,0660);
  return get_expr(e->gauche->type)(e->gauche, c);
}
  
int expr_redirection_er(Expression* e, Contexte* c)
{
  c->fderr = open(e->arguments[0],O_WRONLY|O_CREAT|O_TRUNC,0660);
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

void initialiser_contexte(Contexte* c)
{
  c->bg=false;
  c->fdin=STDIN_FILENO;
  c->fdout=STDOUT_FILENO;
  c->fderr=STDERR_FILENO;
  c->fdclose=-1;
  c->ispipped=false;
  c->tube=NULL;
}

/*
 *copie c1 dans c2
 */
void copier_contexte(Contexte* c1, Contexte* c2)
{
    c2->bg=c1->bg;
    c2->fdin=c1->fdin;
    c2->fdout=c1->fdout;
    c2->fderr=c1->fderr;
	c2->fdclose=c1->fdclose;
    c2->ispipped=c1->ispipped;
    c2->tube=c1->tube;
}

void appliquer_contexte(Contexte* c,bool save)
{
    swapfd(&(c->fdin),STDIN_FILENO,save);
    swapfd(&(c->fdout),STDOUT_FILENO,save);
    swapfd(&(c->fderr),STDERR_FILENO,save);
    if(c->fdclose != -1)
    	close(c->fdclose);
}

void swapfd(int* fdnew, int fdorigin, bool save)
{
    if(*fdnew != fdorigin)
    {
	int tmp;
	if(save)
	    tmp=dup(fdorigin);
	dup2(*fdnew,fdorigin);
	if(save)
	    *fdnew=tmp;
	else
	    close(*fdnew);
    }
}

int
evaluer_expr(Expression *e)
{
    Contexte* c=malloc(sizeof(Contexte));
    initialiser_contexte(c);
    return get_expr(e->type)(e,c);
}
