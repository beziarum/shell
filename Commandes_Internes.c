#include "Evaluation.h"
#include "Shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <readline/history.h>
#include <limits.h>
#include <errno.h>

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

/* Tableau associant les fonctionnalités des commandes internes avec les traitants */ 
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

/*
 * renvoi un pointeur vers la fonction traitant la commande
 * interne passée en paramètre si elle existe, NULL sinon
 */
int (*get_intern (char* name)) (char**)
{
  int taille_tab_cmd_intern = sizeof (tab_cmd_intern)/sizeof(assoc);
  for (int i=0; i<taille_tab_cmd_intern; i++)
    if (strcmp(name,tab_cmd_intern[i].name)==0)
      return tab_cmd_intern[i].data;
  return NULL;
}

/*
 * Commande permettant d'afficher des chaines de caractères
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
	printf("%s ",arg[c]);    // on affiche simplement ces paramètres sur la sortie standard
	c++;
      }
    }
  printf("\n");
  return EXIT_SUCCESS;
}

/*
 * Commande affichant la date
 */
int date(char ** arg)
{
  char c[256]; 
  time_t tmp = time(NULL);                              // on recupère le temps en secondes
  struct tm * t =localtime(&tmp);                       // on génère une structure tm
  strftime(c, sizeof(c), "%A %d %B %Y, %X (UTC%z)",t);  // on affiche selon le format français
  printf("%s\n", c);
  return EXIT_SUCCESS;
}


/*
 * Commande permettant de changer de répertoire courant
 */
int cd (char ** arg)
{
  int r;
  if (arg[1]==NULL)
  {
    r = chdir(getenv("HOME"));       // cas d'un retour au home cd sans paramètre
    verifier(r!=-1,"cd");
  }
  else
  {
    r = chdir(arg[1]);               // cas classique : on apelle juste chdir avec le nom du dossier
    verifier(r!=-1, "cd");
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
  return EXIT_SUCCESS;
}


/*
 * Commande qui affiche le nom de l'hote local
 */
int hostname(char ** arg) 
{
  char hostname[HOST_NAME_MAX +1];
  gethostname(hostname,sizeof(hostname));
  printf("Hostname : %s\n", hostname);
  return EXIT_SUCCESS;
}


/*
 * Commande permettant que quitter le shell
 */
int my_exit(char ** arg) 
{
  if (arg[1] != NULL)         // si il y a un argument
    exit(atoi(arg[1]));       // on quitte le shell en renvoyant la valeur de l'argument
  else
    exit(0);
}


/*
 * Commande kill
 * on va utiliser la fonction kill de la libc
 */
int killShell (char ** arg)
{
  if(arg[1]==NULL)
  {
    errno=EINVAL;
    perror("kill");
    return EXIT_FAILURE;
  }
  int ret;
  if (arg[1][0]!= '-')
  {
    int c = 1;
    while (arg[c]!=NULL)
    {
      ret = kill (atoi(arg[1]),SIGTERM);        // cas par defaut sans signal passé en paramètre
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
    perror("history");
    return EXIT_FAILURE;
  }
  HIST_ENTRY ** hystory_list = history_list ();                       // on crée une variable contenant l'historique
  int treshold = history_length;
  if (arg[1] != NULL && atoi(arg[1]) < history_length)                // si il y a un argument, 
    treshold = atoi(arg[1]) +1;                                       // et qu'il est inférieur au nombre d'éléments de l'historique
  for (int i = history_length - treshold; i < history_length; i++)    // on affiche les n derniers rangs de l'historique
    printf ("%d: %s\n", i + history_base, hystory_list[i]->line);     // (sans compter la commande history qu'on vient de lancer)
  return EXIT_SUCCESS;
}

//partie remote

typedef struct remote_machine {   // structure représentant une machine distante
    char* name;
} remote_machine;

/* Tableau contenant des pointeurs vers des structures de machines distantes */
remote_machine **tab_machines=NULL;

/* Taille initiale du tableau */
int tab_length =0;

/* Nombre de machines présentes dans le tableau */
int nb_machine=0;

int remote_localhost(char** param);
int remote_add(char** param);
int remote_remove(char ** param);
int remote_list(char ** param);
int remote_all(char** param);
int remote_cmd_simple(char** param);
			  
int (*get_remote (char* name)) (char**);

/* Tableau associant les fonctionnalités de la commande remote avec les traitants */ 			  
assoc tab_remote[] = {{"localhost", remote_localhost},
		      {"add", remote_add},
		      {"remove", remote_remove},
		      {"all", remote_all},
		      {"list", remote_list}};

/*
 * fonction interne remote, elle se contente d'appeler la fonction s'occupant de la 
 * sous-commande correspondant à la commande actuelle
 */
int remote(char** params)
{
    int (*cmd_remote) (char**)=get_remote(params[1]);
    if(cmd_remote!=NULL)
	return cmd_remote(params);
    else
    {
	fprintf(stderr,"sous commande de remote inconnue (%s)\n",params[1]);
	return EXIT_FAILURE;
    }
}

/*
 * parcourt le tableau d'association, renvoi la fonction s'ocupant de « remote
 * <name> » si elle existe, sinon remote_cmd_simple
 */
int (*get_remote (char* name)) (char**) 
{
  int taille_tab_remote = sizeof (tab_remote)/sizeof(assoc);
  for (int i=0; i<taille_tab_remote; i++)
    if (strcmp(name,tab_remote[i].name)==0)
      return tab_remote[i].data;
  return remote_cmd_simple;
}

/*
 * Commande créant un shell faussement distant sur la machine locale 
 */
int remote_localhost(char** param)
{
    char** param_remote=malloc(3*sizeof(char**));//on créé la liste de la commande à exécuter ("./Shell -r")
    param_remote[0]=strdup("./Shell");
    param_remote[1]=strdup("-r");
    param_remote[2]=NULL;
    
    Expression* e=ConstruireNoeud(SIMPLE,NULL,NULL,param_remote);//on construit l'expression
    e=ConstruireNoeud(BG,e,NULL,NULL);
    Contexte* c=malloc(sizeof(Contexte));
    
    initialiser_contexte(c);
    int tube[2];
    pipe(tube);
    c->fdin=tube[0];
    c->fdclose=tube[1];
    param+=2;
    while(*param!=NULL)//on écrit les paramètres de la fonction sur l'entrée du nouveau shell
    {
	write(tube[1],*param,strlen(*param));
	write(tube[1]," ",1);
	param++;
    }
    write(tube[1],"\n",1);
    get_expr(BG)(e,c);//et on l'exécute
    close(tube[1]);
    expression_free(e);
    free(c);
    int status;
    waitpid(get_last_pid(),&status,0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : 128 + WTERMSIG(status);
}

/* 
 * Ajoute les noms passés en paramètres à la liste des machines.
 */

int remote_add(char** param)
{
    param+=2;                               // on saute le «remote» et le «add»
    int i=0;
    int alreadyInList=0;
    while(param[i] != NULL){
      for (int j=0; j<nb_machine; j++) {
	if (strcmp(param[i],tab_machines[j]->name)==0) {
	  alreadyInList=1;
	}
      } 
      if (alreadyInList == 0) {
	//nb_add_machines++;
	remote_machine* rm=malloc(sizeof(remote_machine));
	rm->name=strdup(param[i]);
	if(i+nb_machine>=tab_length){
	    if(tab_length==0)
		tab_length=10;
	    else
		tab_length*=2;
	    tab_machines=realloc(tab_machines,sizeof(remote_machine*)*tab_length);
	}
	tab_machines[nb_machine] = rm;
	nb_machine++;
      }
      alreadyInList=0;
      i++;
    }
    printf("%d", nb_machine);
    return EXIT_SUCCESS;
}

/*
 *supprime les machines existantes du tableau
 */
int remote_remove(char ** param) 
{
  for (int i=0; tab_machines[i]; i++) 
  {
    free(tab_machines[i]->name);
    free(tab_machines[i]);
  }
  nb_machine=0;
  return EXIT_SUCCESS;
}


/* 
 * Commande affichant la liste des machines connectées 
 */
int remote_list(char ** param)
{
  if (nb_machine == 0)
  {
    fprintf(stderr,"Il n'y a actuellement aucune machine dans la liste des machines distantes connectées.\nUtilisez remote add pour ajouter des machines.\n");
    return EXIT_SUCCESS;
  }
  else 
    for (int i=0; i<nb_machine; i++)
      printf("%s\n", tab_machines[i]->name);
  return EXIT_SUCCESS;
}

/*
 * Fonction exécutant une commande sur le shell d'une machine distante connectée.
 * On donnera en paramètre une machine, et une commande à exécuter avec ses paramètres
 */
int remote_cmd_simple(char** param)
{
    remote_machine* lmachine=NULL;
    param++;//on saute le premier paramèter (toujours remote)
    for(int i=0;i<nb_machine;i++)//on cherche si la machine est enregistrée
    {
	if(tab_machines[i]!=NULL && strcmp(tab_machines[i]->name,*param)==0)
	{
	    lmachine=tab_machines[i];
	    break;
	}
    }
    if (!lmachine) //si non on s'arète
    {
      fprintf(stderr,"aucune machine de ce nom n'est présente dans la liste.\nUtilisez remote list pour obtenir la liste\n");
      return EXIT_FAILURE;
    }
    param++;//on saute le deuxième paramètre (c'est à dire le nom de la machine)
    
    
    char** param_ssh=InitialiserListeArguments();//on va lancer notre shell en mode distant via ssh
    param_ssh=AjouterArg(param_ssh,"ssh");
    param_ssh=AjouterArg(param_ssh,lmachine->name);
    param_ssh=AjouterArg(param_ssh,"./Shell -r");

    char** param_echo=InitialiserListeArguments();//lui passer les paramètres via echo
    param_echo=AjouterArg(param_echo,"echo");
    while(*param!=NULL)
	param_echo=AjouterArg(param_echo,*(param++));
    
    char** param_xcat=InitialiserListeArguments();//et afficher en local le tout avec xcat
    param_xcat=AjouterArg(param_xcat,"./xcat.sh");
    param_xcat=AjouterArg(param_xcat,"-hold");//on ne ferme pas à la fin du programme (sinon on a pas le temps de lire les résultats)
    param_xcat=AjouterArg(param_xcat,"-T");//le titre est le nom de la machine
    param_xcat=AjouterArg(param_xcat,lmachine->name);
    
    Expression* e=ConstruireNoeud(SIMPLE,NULL,NULL,param_ssh);     //on construit les trois commandes
    Expression* echo=ConstruireNoeud(SIMPLE,NULL,NULL,param_echo);
    Expression* xcat=ConstruireNoeud(SIMPLE,NULL,NULL,param_xcat);
    e=ConstruireNoeud(PIPE,echo,e,NULL);//on pipe echo sur ssh
    e=ConstruireNoeud(PIPE,e,xcat,NULL);//et ssh sur xcat
    Contexte* c=malloc(sizeof(Contexte));
    initialiser_contexte(c);
    int ret= get_expr(PIPE)(e,c);//on exécute enfin le tout
    expression_free(e);
    free(c);
    return ret;
}

/* 
 * Fonction exécutant une commande sur le shell de toutes les machines connectées.
 * On donnera en paramètre la commande et ses arguments
 */
int remote_all(char ** param) 
{
    if (nb_machine == 0)
    {
	fprintf(stderr,"Il n'y a actuellement aucune machine dans la liste des machines distantes connectées.\nUtilisez remote add pour ajouter des machines.");
	return EXIT_SUCCESS;
    }
    char * tmp = param[1];
    pid_t* pid=malloc(sizeof(pid_t)*nb_machine);
    for (int i=0; i<nb_machine; i++)                          // pour chaque machine, on exécute cmd_simple avec le nom de la machine et les parametres
    {
	if((pid[i]=fork())==0)
	{
	    param[1] = tab_machines[i]->name;                       // le premier paramètre correspond au nom de la machine  
	    exit(remote_cmd_simple(param));                         // on appelle ensuite cmd_simple avec le nom de la machine et la liste de paramètres.
	}
    }
    param[1] = tmp;
    for (int i=0; i<nb_machine; i++)
	waitpid(pid[i],NULL,0);
    return EXIT_SUCCESS;
}
				  

