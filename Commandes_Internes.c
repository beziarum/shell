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

int echo(){
  char ptr;
  int c=1;
  while (c>=1){
    c=fread(&ptr,1,c,stdin);
    fwrite(&ptr,1,c,stdout);

  }
  return 0;
}

int date(){

  char c[256]; 
  time_t tmp = time(NULL);
  struct tm * t;
  t =localtime(&tmp);

  strftime(c, sizeof(c), "%A %d %B %Y, %X (UTC%z)",t); 
  printf("%s\n", c);
  return 0;
}
