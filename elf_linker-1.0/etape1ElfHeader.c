#include <elf.h>
#include "elfCustom.h"
#include <stdio.h>
#include "debug.h"


Elf32Custom_Ehdr readElfFileHeader(FILE* fichier){
  Elf32Custom_Ehdr h;
  fread(&h,1,sizeof(h),fichier);
  return h;
}

void displayElfFileHeader(char* fichierElf){
  FILE* fichier = fopen(fichierElf, "r");
  if (fichier != NULL){
    Elf32Custom_Ehdr header;
    header = readElfFileHeader(fichier);
    printf("En-tête ELF:\n");

    //_________________________________________
    //MAGIQUE
    //_________________________________________
    printf("Magique:    ");
    for (int i = 0; i < EI_NIDENT; i++) {
      printf("%x ",header.e_ident[i]);
    }
    printf("\n");


    //_________________________________________
    //CLASSE
    //_________________________________________
    printf("Classe:   ELF64\n");

    //_________________________________________
    //DONNEES
    //_________________________________________
    printf("Données:    ");
    printf("complément à 2, système à octets de poids faible d'abord (little endian)\n");

    //_________________________________________
    //VERSION
    //_________________________________________
    switch (header.e_version) {
      case 0:
      printf("Version:  %d(Invalid)\n",header.e_version);break;
      case 1:
      printf("Version:  %d(Current)\n",header.e_version);break;
    }

    //_________________________________________
    //OS/ABI
    //_________________________________________
    printf("OS/ABI:    UNIX - System V\n");


    //_________________________________________
    //TYPE
    //_________________________________________
    printf("Type:   ");
    switch (header.e_type) {
      case 0:
      printf("Pas de type");break;
      case 1:
      printf("Fichier translatable");break;
      case 2:
      printf("Fichier exécutable");break;
      case 3:
      printf("Fichier objet partagé");break;
      case 4:
      printf("Fichier de base");break;
    }
    printf("\n");

    //_________________________________________
    //MACHINE
    //_________________________________________
    printf("Machine:  ");
    switch (header.e_machine) {
      case 0:
      printf("Pas d'architecture");break;
      case 1:
      printf("AT&T WE 32100");break;
      case 2:
      printf("SPARC");break;
      case 3:
      printf("Intel 80386");break;
      case 4:
      printf("Motorola 68000");break;
      case 5:
      printf("Motorola 88000");break;
      case 7:
      printf("Intel 80860");break;
      case 8:
      printf("MIPS RS3000");break;
      case 62:
      printf("AMD x86-64 architecture");break;
    }
    printf("\n");

    printf("Version:  0x%d\n",header.e_version);
    printf("Entry:  0x%x\n",header.e_entry);
    printf("Début des en-têtes de programme:  %ld(octets dans le fichier)\n",header.e_phoff);
    printf("Début des en-têtes de section:    %ld(octets dans le fichier)\n",header.e_shoff);
    printf("Fanions:  0x%x\n",header.e_flags);
    printf("Taille de cet en-tête:  %d(bytes)\n",header.e_ehsize);
    printf("Taille de l'en-tête du programme::  %d(bytes)\n",header.e_phentsize);
    printf("Nombre d'en-tête du programme:  %d\n",header.e_phnum);
    printf("Taille des en-têtes de section:  %d(bytes)\n",header.e_shentsize);
    printf("Nombre d'en-têtes de section:  %d\n",header.e_shnum);
    printf("Table d'indexes des chaînes d'en-tête de section:  %d\n",header.e_shstrndx);




  }else{
    printf("Erreur: ouverture fichier\n");
  }
  fclose(fichier);
}

int main(int argc, char *argv[]){
  if(argc == 2){
    displayElfFileHeader(argv[1]);
  }else if(argc == 1){
    printf("Erreur: il n'y a pas assez d'arguments\n");
  }else{
    printf("Erreur: il y a trop d'arguments\n");
  }
  return 0;
}
