#include <elf.h>
#include "elfCustom.h"
#include <stdio.h>
#include "debug.h"
#include "util.c"

int reverse_endianess(int value, int size){
  int resultat = 0;
  char *source, *destination;

  source = (char *) &value;
  destination = ((char *) &resultat) + size;
  for (int i = 0; i < size; i++){
        *(--destination) = *(source++);
  }
    return resultat;
}

//Passage de little en big endian

Elf32_Ehdr readElfFileHeader(FILE* fichier){
  Elf32_Ehdr h;
  fread(&h,1,sizeof(h),fichier);
  if (!is_big_endian())
  {
    h.e_type = reverse_endianess(h.e_type,sizeof(h.e_type));
    h.e_machine = reverse_endianess(h.e_machine,sizeof(h.e_machine));
    h.e_version = reverse_endianess(h.e_version,sizeof(h.e_version));
    h.e_entry = reverse_endianess(h.e_entry,sizeof(h.e_entry));
    h.e_phoff = reverse_endianess(h.e_phoff,sizeof(h.e_phoff));
    h.e_shoff = reverse_endianess(h.e_shoff,sizeof(h.e_shoff));
    h.e_flags = reverse_endianess(h.e_flags,sizeof(h.e_flags));
    h.e_ehsize = reverse_endianess(h.e_ehsize,sizeof(h.e_ehsize));
    h.e_phentsize = reverse_endianess(h.e_phentsize,sizeof(h.e_phentsize));
    h.e_phnum = reverse_endianess(h.e_phnum,sizeof(h.e_phnum));
    h.e_shentsize = reverse_endianess(h.e_shentsize,sizeof(h.e_shentsize));
    h.e_shnum = reverse_endianess(h.e_shnum,sizeof(h.e_shnum));
    h.e_shstrndx = reverse_endianess(h.e_shstrndx,sizeof(h.e_shstrndx));
  }
  return h;
}

void displayElfFileHeader(char* fichierElf){
  FILE* fichier = fopen(fichierElf, "r");
  if (fichier != NULL){
    Elf32_Ehdr header;
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
    printf("Classe:   ELF32\n");

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
      printf("Version:   %d(Invalid)\n",header.e_version);break;
      case 1:
      printf("Version:   %d(Current)\n",header.e_version);break;
    }

    //_________________________________________
    //OS/ABI
    //_________________________________________
    printf("OS/ABI:    UNIX - System V\n");

    //_________________________________________
    //TYPE
    //_________________________________________
    printf("Type:     ");
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
    printf("Machine:    ");
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


    printf("Version:   0x%d\n",header.e_version);
    printf("Entry:   0x%x\n",header.e_entry);
    printf("Début des en-têtes de programme:    %d(octets dans le fichier)\n",header.e_phoff);
    printf("Début des en-têtes de section:    %d(octets dans le fichier)\n",header.e_shoff);
    printf("Fanions:   0x%x\n",header.e_flags);
    printf("Taille de cet en-tête:  %d(bytes)\n",header.e_ehsize);
    printf("Taille de l'en-tête du programme::    %d(bytes)\n",header.e_phentsize);
    printf("Nombre d'en-tête du programme:   %d\n",header.e_phnum);
    printf("Taille des en-têtes de section:    %d(bytes)\n",header.e_shentsize);
    printf("Nombre d'en-têtes de section:   %d\n",header.e_shnum);
    printf("Table d'indexes des chaînes d'en-tête de section:   %d\n",header.e_shstrndx);




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
