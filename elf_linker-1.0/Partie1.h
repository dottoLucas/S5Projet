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
void printfheader(Elf32_Ehdr header);
Elf32_Ehdr displayElfFileHeader(char* fichierElf,int b);

//Partie 2
Elf32_Shdr readHsHeader(Elf32_Shdr *Hs,  FILE * fichier);
char* find_type(Elf32_Shdr Hs_header);
void print_flag(unsigned int flag);
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

//Partie 5
Elf32_Rel reverseAllEndiannessRel(Elf32_Rel relStruct);
char* get_symbol_type (unsigned int type);
char* get_symbol_binding (unsigned int binding);
char* get_symbol_visibility (unsigned int visibility);
char* get_section_name(unsigned int type);
char* get_rel_type(unsigned int type);
Elf32_Sym reverseAllEndiannessSym(Elf32_Sym symStruct);
char* getNomSym(FILE *fichier,Elf32_Ehdr header,Elf32_Shdr SymTab,Elf32_Sym Sym);
Elf32_Shdr reverseAllEndiannessSectionHeader(Elf32_Shdr headSection);
void readElfFileHeaderSection(FILE* fichier, Elf32_Shdr tabHeadSection[],Elf32_Ehdr header, int nbSection, int tailleEnTeteSection);
Elf32_Shdr readElfFileTabSymSectionHeader(Elf32_Shdr tabHeadSection[],int nbSection, FILE* fichier);
void readElfFileTabRelSectionHeader(Elf32_Shdr sectionTabRel[], Elf32_Shdr tabHeadSection[],int nbSection, FILE* fichier, int* nbSectionRel);
void getArrayOfRelEntryNumber(int* arrayRelEntryNumber, Elf32_Shdr sectionTabRel[], int nbSectionRel);
void readElfFileRelTable(FILE* fichier, Elf32_Ehdr header, Elf32_Shdr tabHeadSection[], Elf32_Shdr sectionSym, Elf32_Shdr sectionTabRel[], int nbSectionRel, Elf32_Rel** relTabArray, int* tabRelEntryNumber);
void readElfFileSymTable(FILE* fichier, Elf32_Shdr sectionSym, Elf32_Sym* symTab, int nbSymbole, int tailleSymTab);
char* getName(FILE* fichier, Elf32_Shdr tabHeadSection[], Elf32_Ehdr header, int index, int indexStrTab);
void displayElfFileRelTab(FILE* fichier, Elf32_Ehdr header, Elf32_Shdr tabHeadSection[], Elf32_Shdr sectionSym, Elf32_Shdr sectionTabRel[], Elf32_Rel** relTabArray, Elf32_Sym symTab[],int nbSectionRel, int arrayRelEntryNumber[]);

#endif
