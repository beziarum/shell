#include "Commandes_Internes.h"


int echo(){
  void* ptr;
  int c=1;
  while (c>=1){
    c=fread(&ptr,1,1,stdin);
    fwrite(&ptr,1,1,stdout);
    fprintf(stderr,"%d",c);
  }
  return 1;
}

int main (int argc, char** argv){
  echo();
  return EXIT_SCCUSS;
}
