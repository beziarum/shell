#include "Commandes_Internes.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

void verifier(int b,char* m)
{
    if(!b)
    {
	perror(m);
	exit(EXIT_FAILURE);
    }
}

int date(char** arg);
int echo(char** arg);
int cd(char** arg);

typedef struct assoc {
    char* name;
    int (*data) (char** params);
} assoc;

assoc tab_cmd_intern[] = {{"date", date},
			  {"echo", echo},
			  {"cd", cd}};

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
