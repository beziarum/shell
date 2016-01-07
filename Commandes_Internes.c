#include "Commandes_Internes.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

void verifier(int b,char* m)
{
    if(!b)
	perror(m);
}

int date(char** arg);
int echo(char** arg);
int cd(char** arg);
int my_exit(char ** arg);
int hostname(char **);
int pwd(char ** arg);
int hostname(char ** arg);
int killShell(char** arg);


typedef struct assoc {
    char* name;
    int (*data) (char** params);
} assoc;

assoc tab_cmd_intern[] = {{"date", date},
			  {"echo", echo},
			  {"cd", cd},
			  {"hostname", hostname},
			  {"exit", my_exit},
			  {"pwd" ,pwd},
			  {"hostname", hostname},
			  {"kill",killShell}};

int (*get_intern (char* name)) (char**)
{
    int taille_tab_cmd_intern = sizeof (tab_cmd_intern)/sizeof(assoc);
    for (int i=0; i<taille_tab_cmd_intern; i++)
	if (strcmp(name,tab_cmd_intern[i].name)==0)
	    return tab_cmd_intern[i].data;
    return NULL;
}

int echo(char** param){
  char ptr;
  int c=1;
  while (c>=1){
    c=fread(&ptr,1,c,stdin);
    fwrite(&ptr,1,c,stdout);

  }
  return 0;
}

int date(char** param){

  char c[256]; 
  time_t tmp = time(NULL);
  struct tm * t;
  t =localtime(&tmp);

  strftime(c, sizeof(c), "%A %d %B %Y, %X (UTC%z)",t); 
  printf("%s\n", c);
  return 0;
}


int cd (char** arg){
  int r;
  if (arg[1]==NULL){
    r = chdir(getenv("HOME"));
    verifier(r!=-1,"erreur dans la variable d'environement HOME");
  }
  else{
  r = chdir(arg[1]);
  verifier(r!=-1, "Aucun fichier ou dossier de ce type");
  }
  return r;
}

int pwd(char ** arg) {
  char pwd[500];
  getcwd(pwd, sizeof(pwd));
  printf("%s\n", pwd);
  return 0;
}

int hostname(char ** arg) {
  char hostname[500];
  gethostname(hostname, 500);
  printf("Hostname : %s\n", hostname);
  return 0;
}

int my_exit(char ** arg) {
  if (arg + 1 == NULL) 
    exit(atoi(arg[1]));
  else
    exit(0);
}

int killShell (char** arg){
  int ret;
  int c = 2;
  if (arg[1][0]!= '-'){
    ret = kill (atoi(arg[1]),SIGTERM);
    verifier(ret!=-1, "kill");
  }
  else{
    char *sign = arg[1]+1;
    int x = atoi (sign);
    printf("%d",x);
    while (arg[c]!=NULL){
      ret = kill(atoi(arg[c]),x);
      verifier(ret!=-1, "kill");
      c=c+1;
    }
  }
  return ret;
}

