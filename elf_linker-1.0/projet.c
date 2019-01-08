#include "Partie1.h"
#include "Partie2.h"
#include <elf.h>
#include <stdio.h>
#include "debug.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "util.h"
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
  // faire sortir le fichier
  int indiceSmyTab;
  int indiceSection;
  FILE* fichier = fopen(argv[2], "r");
	Elf32_Ehdr data;
	Elf32_Shdr *TableHs;

    printf("-h --file-header       Afficher l'en-tête du fichier ELF\n");
    printf("-S --section-headers   Afficher les en-têtes des sections\n");
    printf("-x --hex-dump=<numéro|nom> \n                Vidanger le contenu de la section <numéro|nom> sous forme d'octets\n");
    printf("-s --syms              Afficher la table des symboles\n");
    printf("-r --relocs            Afficher les réadressages (si présents)\n");
  
    //Etape 1
    data = displayElfFileHeader(argv[2]);
    //Etape2
    TableHs = malloc(data.e_shnum * sizeof(Elf32_Shdr));
    elf_print_header(&TableHs, fichier,&data);
    //Etape3 
    indiceSection=VerfifierParametre(argv[1],data,TableHs,fichier);
    if(indiceSection<0){
      printf("AVERTISSEMENT: La section %s n'a pas été vidangée parce qu'inexistante !\n",argv[1] );
    }else{
      afficherSection(fichier, TableHs[indiceSection]);
    }
    //Etape 4
    indiceSmyTab=recurpererNumSec(TableHs,data,fichier,".symtab");
    affichageSymTab(argv[2],TableHs[indiceSmyTab],&data,TableHs);
    //Etape 5
    printf("etape 5 a faire\n");
  
    //Etape 6
    SupprimerSection(TableHs,data,fichier); 

  return 0;
}