#include <sys/wait.h>
#include <fcntl.h>
#include "Shell.h"
#include "Evaluation.h"
#include "Commandes_Internes.h"

//Ces fonction implémentant le comportement à suivre pour chaques
//types d'expressions
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

//Cette fonction interviennent sur le comportement du programme
//en foction du contexte, swapfd est utilisé dans
//appliquer_contexte
void appliquer_contexte(Contexte* c, bool save);
void swapfd(int* fdnew, int fdorigin, bool save);

typedef struct assoc {
    expr_t expr;
    int (*data) (Expression*,Contexte*);
} assoc;

pid_t lpid=0;

/* Tableau permettant d'associer un type d'expréssion avec la fonction qui la traite */
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

/* 
 * Fonction permettant de ne pas provoquer d'erreur lorsque qu'on essaye d'exécuter une commande non implémentée 
 */
int expr_not_implemented (Expression* e, Contexte* c)
{
    fprintf(stderr,"fonctionnalité non implémentée\n");
    
    return 1;
}

/* 
 * Fonction traitant l'expression vide. Rien ne se passe. 
 */
int expr_vide(Expression* e, Contexte* c){
  return EXIT_SUCCESS;
}

/*
 * Exécute l'expression située à gauche puis celle à droite
 */
int expr_sequence(Expression* e, Contexte* c)
{
    get_expr(e->gauche->type)(e->gauche, c);
    return get_expr(e->droite->type)(e->droite, c);
}

/*
 * Fonction traitant les expressions en arrière plan.
 */
int expr_bg (Expression* e, Contexte* c)
{
    c->bg=true;
    return get_expr(e->gauche->type)(e->gauche, c);
}

/*
 * Fonction traitant les expressions simples. toutes les feuilles d'un arbre
 * expression étant des expressions simples, c'est ici qu'on applique les
 * contextes modifiés par les autres traitants d'expression.
 */
int expr_simple (Expression* e, Contexte* c)
{
    int (*intern)(char**)=get_intern(e->arguments[0]);
    if(intern!=NULL && c->bg!=true) //si la commande est une commande interne
	                            //et en avant-plan
    {
	appliquer_contexte(c,true);  //on applique le contexte
	int ret=intern(e->arguments);//on exécute la commande interne
	appliquer_contexte(c,false); //et on restaure le contexte
	return ret;
	    
    }
    else
    { 
	pid_t pid;
	if(pid==0)
	{ //pour le fils…
	    appliquer_contexte(c,false); //on applique le contexte
	    if(intern != NULL) //si c'est une commande interne
	    {
		exit(intern(e->arguments)); //on l'éxécute, puis on coupe
	    }
	    else
	    {
		execvp(e->arguments[0],e->arguments);//sinon c'est une commande externe,
		                                     //donc on lui passe la main
		perror(e->arguments[0]);
		exit(1);
	    }
	}
	else
	{ //pour le père
	    lpid=pid;//on met à jour le pid du dernier fils créé
	    if(c->tube!=NULL)//s'il existe on ferme le tube
	    {
		close(c->tube[0]);
		close(c->tube[1]);
	    }
	    if (c->bg) //si le fils est exécuté en arrière plan alors on
		return 0; //redonne la main au shell
	    int status;
	    waitpid(pid,&status,0); //sinon on attend qu'il termine et on renvoi son code de retour
	    return WIFEXITED(status) ? WEXITSTATUS(status) : 128 + WTERMSIG(status);
	}
    }
}	

/*
 * Cette fonction traite les pipes.
 */
int expr_pipe(Expression* e, Contexte* c)
{
    Contexte* c2=malloc(sizeof(Contexte)); //on créé une copie du contexte
    copier_contexte(c,c2);

    int* tube=c2->tube=malloc(2*sizeof(int)); //on garde une copie du tube
    pipe(tube);                          //dans le contexte de droite pour
                                         //pouvoir le fermer dans le shell

    c->bg=true;         //la commande à gauche sera en arrière plan
    c->fdout=tube[1];   //sa sortie sera redirigé vers l'entrée du pipe
    c->fdclose=tube[0]; //et on fermera la sortie du tube
    
    c2->fdin=tube[0];   //on redirigera la sortie du tube vers l'entrée de la
                        //seconde commmande
    c2->fdclose=tube[1];//et on fermera l'entrée du tube

    get_expr(e->gauche->type)(e->gauche,c);        //enfin on exécute les
    return get_expr(e->droite->type)(e->droite,c2);//deux commandes
}


/*
 * Traite l'expression SEQUENCE ET
 * Exécute la première commande puis la seconde si la première a
 * renvoyé un succès
 */
int expr_sequence_et (Expression* e, Contexte* c)
{
    int ret=get_expr(e->gauche->type)(e->gauche, c);
    if(ret != 0)
	return ret;
    else
	return get_expr(e->droite->type)(e->droite, c);
}

/*
 * Traite l'expression SEQUENCE OU
 * Exécute la première commande puis la seconde si la première a
 * renvoyé un echec
 */
int expr_sequence_ou (Expression* e, Contexte* c)
{
    int ret=get_expr(e->gauche->type)(e->gauche, c);
    if(ret == 0)
	return ret;
    else
	return get_expr(e->droite->type)(e->droite, c);
}

/*
 * Traite l'expression SOUS_SHELL
 * En pratique on se contante de traiter l'expression de gauche
 */
int expr_sous_shell (Expression* e, Contexte* c)
{
  return get_expr(e->gauche->type)(e->gauche, c);
}

/*
 * Traite l'expression REDIRECTION_I
 * on indique dans le contexte qu'il faudra utiliser à la place de
 * l'entrée standard le descripteur correspondant au fichier donné
 * en paramètre
 */
int expr_redirection_in(Expression* e, Contexte* c)
{
    c->fdin = open(e->arguments[0],O_RDONLY);
    return get_expr(e->gauche->type)(e->gauche, c);
}

/*
 * idem pour la sortie standard
 */
int expr_redirection_o (Expression* e, Contexte* c)
{
    c->fdout=open(e->arguments[0],O_WRONLY|O_CREAT|O_TRUNC,0660);
    return get_expr(e->gauche->type)(e->gauche,c);
}

/*
 * idem mais le fichier est ouvert en mode append
 */
int expr_redirection_a(Expression* e, Contexte* c)
{
  c->fdout = open(e->arguments[0],O_WRONLY|O_CREAT|O_APPEND,0660);
  return get_expr(e->gauche->type)(e->gauche, c);
}

/*
 * idem mais pour la sortie d'erreur
 */
int expr_redirection_er(Expression* e, Contexte* c)
{
  c->fderr = open(e->arguments[0],O_WRONLY|O_CREAT|O_TRUNC,0660);
  return get_expr(e->gauche->type)(e->gauche, c);
}


/*
 * Parcourt le tableau d'association pour trouver quelle est la fonction
 * qui s'occupe de l'expression passée en paramètre et la renvoi.
 * Si cette fonction n'as pas put être trouvée alors c'est cette expression
 * n'as pas été implémenté*/
int (*get_expr (expr_t expr)) (Expression*, Contexte*)
{
    int taille_tab_expr = sizeof (tab_expr)/sizeof (assoc);
    for (int i=0; i<taille_tab_expr ;i++)
	if(tab_expr[i].expr == expr)
	    return tab_expr[i].data;
    return expr_not_implemented;
}

/*
 * Initialise le contexte passé en parametre aux valeurs par défaut
 */
void initialiser_contexte(Contexte* c)
{
  c->bg=false;
  c->fdin=STDIN_FILENO;
  c->fdout=STDOUT_FILENO;
  c->fderr=STDERR_FILENO;
  c->fdclose=-1;
  c->tube=NULL;
}

/*
 *copie les valeurs de c1 dans c2
 */
void copier_contexte(Contexte* c1, Contexte* c2)
{
    c2->bg=c1->bg;
    c2->fdin=c1->fdin;
    c2->fdout=c1->fdout;
    c2->fderr=c1->fderr;
	c2->fdclose=c1->fdclose;
    c2->tube=c1->tube;
}

/*
 * applique un contexte et sauvegarde les anciens fd si save
 * vaut vrai
 */
void appliquer_contexte(Contexte* c,bool save)
{
    swapfd(&(c->fdin),STDIN_FILENO,save);
    swapfd(&(c->fdout),STDOUT_FILENO,save);
    swapfd(&(c->fderr),STDERR_FILENO,save);
    if(c->fdclose != -1)
    	close(c->fdclose);
}

/*
 * effectue un dup2 sur la valeur pointé par fdnew  et sur fdold
 * si save vaut vrai alors fdold est sauvegardé pour pouvoir effectuer 
 * l'opération inverse.
 */
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

/*
 * renvoi le pid du dernier fils créé
 */
pid_t get_last_pid()
{
    return lpid;
}
