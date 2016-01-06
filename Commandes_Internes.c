#include "Commandes_Internes.h"


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


