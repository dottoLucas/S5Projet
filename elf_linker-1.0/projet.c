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

void print_err_msg(){
    printf("-h --file-header       Afficher l'en-tête du fichier ELF\n");
    printf("-S --section-headers   Afficher les en-têtes des sections\n");
    printf("-x --hex-dump=<numéro|nom> \n                Vidanger le contenu de la section <numéro|nom> sous forme d'octets\n");
    printf("-s --syms              Afficher la table des symboles\n");
    printf("-r --relocs            Afficher les réadressages (si présents)\n");
    exit(0);
}
int main(int argc, char *argv[]){
  // faire sortir le fichier
  int indiceSmyTab;
  int indiceSection;
  FILE* fichier;

  int f,b=0; 
  
  Elf32_Ehdr data;
  Elf32_Shdr *TableHs;

  //printf("\n\n################################################################\n\n");
  
  if(argc < 3 || (argc==4 && argv[1][1]!='x') || argc > 4 || ((argv[1][1]!='x' && argv[1][1]!='h'&& argv[1][1]!='s'&& argv[1][1]!='S'&& argv[1][1]!='r') || (argv[1][1]=='x' && argc!=4)) ){
     print_err_msg();
  }
  if(argv[1][1]=='x')
    f=3;
  else
    f=2;
  
  if(argv[1][1]=='h')
    b=1;
     fichier= fopen(argv[f], "r");
    //Etape 1
    data = displayElfFileHeader(argv[f],b);
    //Etape2
    TableHs = malloc(data.e_shnum * sizeof(Elf32_Shdr));
    elf_print_header(&TableHs, fichier,&data);
    if(argv[1][1]=='S')
      afficherHeader(TableHs,&data,fichier);
    //Etape3 
    if(argv[1][1]=='x'){
      indiceSection=VerfifierParametre(argv[2],data,TableHs,fichier);
      if(indiceSection<0){
        printf("AVERTISSEMENT: La section %s n'a pas été vidangée parce qu'inexistante !\n",argv[2] );
      }else{
        afficherSection(fichier, TableHs[indiceSection]);
      }
    }
    //Etape 4
    indiceSmyTab=recurpererNumSec(TableHs,data,fichier,".symtab");
    if(argv[1][1]=='s'){
      
      affichageSymTab(argv[2],TableHs[indiceSmyTab],&data,TableHs);
    }
    //Etape 5
    if (argv[1][1]=='r')
    {
     // Elf32_Ehdr header = readElfFileHeader(fichier); //on initialise le tableau d'en-têtes de section
      Elf32_Shdr tabHeadSection[data.e_shnum] ;
      readElfFileHeaderSection(fichier, tabHeadSection, data, data.e_shnum, data.e_shentsize);


      //on initialise l'en-têtes de la section de symboles
      Elf32_Shdr sectionSym = readElfFileTabSymSectionHeader(tabHeadSection,data.e_shnum, fichier); 

      //on initialise le tableau d'en-têtes de relocalisation
      int nbSectionRel = 0; 
      Elf32_Shdr sectionTabRel[data.e_shnum]  ;

      readElfFileTabRelSectionHeader(sectionTabRel, tabHeadSection,data.e_shnum, fichier, &nbSectionRel);

      //on récupère le nombre de symbole
      int nbSymbole = sectionSym.sh_size/sizeof(Elf32_Sym); 

      //on déclare notre tableau de symbole
      Elf32_Sym symTab[nbSymbole] ; 
      int tailleSymTab = sizeof(symTab);

      readElfFileSymTable(fichier, sectionSym, symTab, nbSymbole, tailleSymTab);

      Elf32_Rel* relTabArray[nbSectionRel];

      //on récupère le nombre d'entrées par table de relocalisation
      int tabRelEntryNumber[nbSectionRel] ;

      getArrayOfRelEntryNumber(tabRelEntryNumber, sectionTabRel, nbSectionRel);

      for (int i = 0; i < nbSectionRel; i++) {
        relTabArray[i] = malloc(tabRelEntryNumber[i] * sizeof(Elf32_Rel));
      }

      readElfFileRelTable(fichier, data, tabHeadSection, sectionSym, sectionTabRel, nbSectionRel, relTabArray, tabRelEntryNumber);

      displayElfFileRelTab(fichier, data, tabHeadSection, sectionSym, sectionTabRel, relTabArray, symTab, nbSectionRel, tabRelEntryNumber);
      /*for (int i = nbSectionRel; i >= 0; i--) {
        free(relTabArray[i]);
      }*/

    }
    

    printf("\n\naffichage de la partie 6  ... \n");
    printf("_____________________\n\n");
    //Etape 6
    SupprimerSection(TableHs,data,fichier); 

  return 0;
}
