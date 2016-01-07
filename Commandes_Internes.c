#include "Commandes_Internes.h"

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
			  {"history", history}};


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
    printf("%s ",arg[c]);    // on affiche simplement ces parametres sur la sortie standard
    c++;
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
  char pwd[500];
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
  if (arg[1] != NULL && atoi(arg[1]) <= history_length)               // si il y a un argument, et qu'il est inférieur au nombre d'éléments de l'historique
    treshold = atoi(arg[1]);
  for (int i = history_length - treshold; i < history_length; i++)    // on affiche les n derniers rangs de l'historique 
    printf ("%d: %s\n", i + history_base, hystory_list[i]->line);
  return 0;
}
