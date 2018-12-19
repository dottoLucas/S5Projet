#include <elf.h>
#include "util.h"
#include "elfCustom.h"
#include <stdio.h>

void displayElfFileSymTab(char* nomfichier){
  FILE* fichier = fopen(fichierElf, "r");
  if (fichier != NULL){
    //partie2 récupérer toutes les sections avec type=SYMTAB
    //partie3 récupérer pour chacune les informations contenue
      //partie4 pour chaque sections on traduit les informations
      
  }else{
    printf("Erreur: ouverture fichier\n");
  }
  fclose(fichier);
}

int main(int argc, char *argv[]){
  if(argc == 2){
    displayElfFileSymTab(argv[1]);
  }else if(argc == 1){
    printf("Erreur: il n'y a pas assez d'arguments\n");
  }else{
    printf("Erreur: il y a trop d'arguments\n");
  }
  return 0;
}

Elf32_Ehdr readElfFileTabSymbole(FILE* fichier)
{
  Elf32_Sym h;
  fread(&h,1,sizeof(h),fichier);
}
