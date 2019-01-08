#ifndef __PARTIE1__
#define __PARTIE1__

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


int reverse_endianess(int value, int size);
int recurpererNumSec(Elf32_Shdr* TableHs,Elf32_Ehdr data,FILE *fichier,char* param);

//Partie1
Elf32_Ehdr readElfFileHeader(FILE* fichier);
Elf32_Ehdr displayElfFileHeader(char* fichierElf);

//Partie 2
Elf32_Shdr readHsHeader(Elf32_Shdr *Hs,  FILE * fichier);
char* find_type(Elf32_Shdr Hs_header);
void afficherHeader(Elf32_Shdr *TableHs,Elf32_Ehdr *data,FILE *fichier);
void elf_print_header(Elf32_Shdr **TableHs, FILE* fichier,Elf32_Ehdr *data);

//Partie 3
int VerfifierParametre(char* param,Elf32_Ehdr data,Elf32_Shdr* TableHs,FILE* fichier);
void afficherSection(FILE *fichier, Elf32_Shdr Section_a_traitee );

//Partie 4
char* findSymType(Elf32_Sym Sym );
char* findSymLink(Elf32_Sym Sym );
void AfficherNom(FILE *fichier,Elf32_Ehdr *data,Elf32_Shdr SmyTab,Elf32_Sym *Sym);
void affichageSymTab(char *fichierG,Elf32_Shdr SmyTab,Elf32_Ehdr *data,Elf32_Shdr *TableHs);

#endif