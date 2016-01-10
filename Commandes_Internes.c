#include "Evaluation.h"
#include "Shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <readline/history.h>
#include <limits.h>
#include <errno.h>
#include <limits.h>

/*
 * Déclaration des commandes
 */
int date(char ** arg);
int echo(char ** arg);
int cd(char ** arg);
int my_exit(char ** arg);
int hostname(char ** arg);
int pwd(char ** arg);
int hostname(char ** arg);
int killShell(char ** arg);
int history(char ** arg);
int remote(char** arg);

/*
 * Structure association qui fait correspondre une
 * chaine de carractère à un pointeur sur fonction
 */
typedef struct assoc {
    char* name;
    int (*data) (char ** params);
} assoc;


assoc tab_cmd_intern[] = {{"date", date},
			  {"echo", echo},
			  {"cd", cd},
			  {"hostname", hostname},
			  {"exit", my_exit},
			  {"pwd" ,pwd},
			  {"hostname", hostname},
			  {"kill",killShell},
			  {"history", history},
			  {"remote", remote}};



void verifier(int b,char* m)
{
  if(!b)
    perror(m);
}


int (*get_intern (char* name)) (char**)
{
  int taille_tab_cmd_intern = sizeof (tab_cmd_intern)/sizeof(assoc);
  for (int i=0; i<taille_tab_cmd_intern; i++)
    if (strcmp(name,tab_cmd_intern[i].name)==0)
      return tab_cmd_intern[i].data;
  return NULL;
}

/*
 * Commande echo
 */
int echo(char ** arg)
{
  
  int c=1;
  while (arg[c]!=NULL)
    {
      if (arg[c][0]=='$'){
	char *tmp=arg[c]+1;
	printf("%s ",getenv(tmp));
	c++;
      }
      else{
	printf("%s ",arg[c]);    // on affiche simplement ces parametres sur la sortie standard
	c++;
      }
    }
  printf("\n");
  return 0;
}

/*
 * Commande date
 */
int date(char ** arg)
{
  char c[256]; 
  time_t tmp = time(NULL);                              // on recupére le temps en seconde
  struct tm * t =localtime(&tmp);                       // on génére une structure tm
  strftime(c, sizeof(c), "%A %d %B %Y, %X (UTC%z)",t);  // on affiche celon le format français
  printf("%s\n", c);
  return 0;
}


/*
 * Commande cd
 */
int cd (char ** arg)
{
  int r;
  if (arg[1]==NULL)
  {
    r = chdir(getenv("HOME"));                                       // cas d'un retour au home cd sans paramètre
    verifier(r!=-1,"erreur dans la variable d'environement HOME");
  }
  else
  {
    r = chdir(arg[1]);                                               // cas classique : on apelle juste chdir avec le nom du dossier
    verifier(r!=-1, "Aucun fichier ou dossier de ce type");
  }
  return r;
}


/* 
 * Commande qui affiche le répertoire courant
 */
int pwd(char ** arg) 
{
  char pwd[PATH_MAX];
  getcwd(pwd, sizeof(pwd));
  printf("%s\n", pwd);
  return 0;
}


/*
 * Commande qui affiche le nom de l'hote local
 */
int hostname(char ** arg) 
{
  char hostname[HOST_NAME_MAX +1];
  gethostname(hostname,sizeof(hostname));
  printf("Hostname : %s\n", hostname);
  return 0;
}


/*
 * Commande permettant que quitter le shell
 */
int my_exit(char ** arg) 
{
  if (arg[1] != NULL)         // si il y a un argument
    exit(atoi(arg[1]));       // on quitte le shell en renvoyant la valeur de l'agument
  else
    exit(0);
}


/*
 *Commande kill
 *on va utiliser la fonction de la libc kill
 */
int killShell (char ** arg)
{
  if(arg[1]==NULL)
  {
    errno=EINVAL;
    perror("pas de paramètre");
    return -1;
  }
  int ret;
  if (arg[1][0]!= '-')
  {
    int c = 1;
    while (arg[c]!=NULL)
    {
      ret = kill (atoi(arg[1]),SIGTERM);        // cas par defaut sans signal passé en parametre
      verifier(ret!=-1, "kill");
      c++;
    }
  }
  else
  {
    char *sign = arg[1]+1;                      // on récupère le premier paramètre sans le "-"
    int x = atoi (sign);
    int c=2;
    while (arg[c]!=NULL)
    {
      ret = kill(atoi(arg[c]),x);               // et on lance le signal
      verifier(ret!=-1, "kill");
      c++;
    }
  }
  return ret;
}


/*
 * Commande qui affiche l'historique du shell. On peut l'appeler avec un argument
 * pour obtenir l'historique des n dernières commandes 
 */
int history(char ** arg) 
{
  if (arg[2] != NULL)
  {
    errno=EINVAL;
    perror("trop d'arguments, history admet 0 ou 1 argument");
    return -1;
  }
  HIST_ENTRY ** hystory_list = history_list ();                       // on crée une variable contenant l'historique
  int treshold = history_length;
  if (arg[1] != NULL && atoi(arg[1]) < history_length)                // si il y a un argument, et qu'il est inférieur au nombre d'éléments de l'historique
    treshold = atoi(arg[1]) +1;
  for (int i = history_length - treshold; i < history_length; i++)    // on affiche les n derniers rangs de l'historique (sans compter la commande history qu'on vient de lancer)
    printf ("%d: %s\n", i + history_base, hystory_list[i]->line);
  return 0;
}

//partie remote

typedef struct remote_machine {   // structure représentant une machine distante
    char* name;
    int fd;
} remote_machine;

remote_machine **tab_machines={NULL};
int tab_length =1;
int nb_machine=0;

int remote_localhost(char** param);
int remote_add(char** param);
int remote_remove(char ** param);
int remote_list(char ** param);

			  
int (*get_remote (char* name)) (char**);
			  
assoc tab_remote[] = {{"localhost", remote_localhost},
		      {"add", remote_add},
		      {"remove", remote_remove},
		      {"list", remote_list}};

int remote(char** params)
{
    int (*cmd_remote) (char**)=get_remote(params[1]);
    if(cmd_remote!=NULL)
	return cmd_remote(params);
    else
    {
	fprintf(stderr,"sous commande de remote inconnue (%s)\n",params[1]);
	return 1;
    }
}

int (*get_remote (char* name)) (char**) 
{
  int taille_tab_remote = sizeof (tab_remote)/sizeof(assoc);
  for (int i=0; i<taille_tab_remote; i++)
    if (strcmp(name,tab_remote[i].name)==0)
      return tab_remote[i].data;
  return NULL;
}


int remote_localhost(char** param)
{
    char** param_remote=malloc(3*sizeof(char**));
    param_remote[0]=strdup("./Shell");
    param_remote[1]=strdup("-r");
    param_remote[2]=NULL;
    
    Expression* e=ConstruireNoeud(SIMPLE,NULL,NULL,param_remote);
    e=ConstruireNoeud(BG,e,NULL,NULL);
    Contexte* c=malloc(sizeof(Contexte));
    initialiser_contexte(c);
    int tube[2];
    pipe(tube);
    c->fdin=tube[0];
    c->fdclose=tube[1];
    param+=2;
    while(*param!=NULL)
    {
	write(tube[1],*param,strlen(*param));
	write(tube[1]," ",1);
	param++;
    }
    write(tube[1],"\n",1);
    get_expr(BG)(e,c);
    close(tube[1]);
    expression_free(e);
    free(c);
    int status;
    waitpid(get_last_pid(),&status,NULL);
    return WIFEXITED(status) ? WEXITSTATUS(status) : 128 + WTERMSIG(status);
}

int remote_add(char** param)
{
    int nb_add_machines=0;
  int i=0;
  while(param[i] != NULL){
    nb_add_machines++;
    i++;
    remote_machine  rm;
    rm.name=strdup(param[i]);
    if(i+nb_machine>=tab_length){
      tab_length*=2;
      tab_machines=realloc(tab_machines,sizeof(remote_machine*)*tab_length);
    }
    tab_machines[i+nb_machine] = &rm;
  }
  for(int i=0;i<nb_add_machines;i++){

    char** param_ssh=malloc(4*sizeof(char**));
    param_ssh[0]=strdup("./ssh");
    param_ssh[1]=strdup(tab_machines[i]->name);
    param_ssh[2]=strdup("./Shell -r");
    param_ssh[3]=NULL;

    Expression* e=ConstruireNoeud(SIMPLE,NULL,NULL,param_ssh);
    e=ConstruireNoeud(BG,e,NULL,NULL);
    Contexte* c=malloc(sizeof(Contexte));
    initialiser_contexte(c);
    int tube[2];
    pipe(tube);
    c->fdin=tube[0];
    afficher_expr(e);
    int ret= get_expr(BG)(e,c);
    //expression_free(e);
    free(c);
  }
  return 0;
}


int remote_remove(char ** param) 
{
  for (int i=0; tab_machines[i]; i++) 
  {
    close(tab_machines[i]->fd);
    free(tab_machines[i]);
  }
  return 0;
}


int remote_list(char ** param)
{
  if (nb_machine == 0)
    printf("Il n'y a actuellement aucune machine dans la liste des machines distantes connectées.\nUtilisez remote add pour ajouter des machines.");
  else 
    for (int i=0; i<nb_machine; i++)
      printf("%s\n", tab_machines[i]->name);
  return 0;
}

int remote_cmd_simple(char** param)
{
    remote_machine* lmachine=NULL;
    param++;
    for(int i=0;i<tab_length;i++)
    {
	if(tab_machines[i]!=NULL && strcmp(tab_machines[i]->name,*param))
	{
	    lmachine=tab_machines[i];
	    break;
	}
    }

    param++;
    while(*param!=NULL)
    {
	write(lmachine->fd,*param,strlen(*param));
	write(lmachine->fd," ",1);
	param++;
    }
    write(lmachine->fd,"\n",1);
    return EXIT_SUCCESS;
}
