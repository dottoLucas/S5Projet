#include <elf.h>
#include "util.c"
#include "elfCustom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define SectionNameLength 20

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

Elf32_Rel reverseAllEndiannessRel(Elf32_Rel relStruct){
  if (!is_big_endian()){
    relStruct.r_offset = reverse_endianess(relStruct.r_offset,sizeof(relStruct.r_offset));
    relStruct.r_info = reverse_endianess(relStruct.r_info,sizeof(relStruct.r_info));
  }
  return relStruct;
}

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

char* get_symbol_type (unsigned int type){
  switch (type)
    {
    case STT_NOTYPE:	return "NOTYPE";
    case STT_OBJECT:	return "OBJECT";
    case STT_FUNC:	return "FUNC";
    case STT_SECTION:	return "SECTION";
    case STT_FILE:	return "FILE";
    case STT_COMMON:	return "COMMON";
    case STT_TLS:	return "TLS";
    }
    return "Error";
}

char * get_symbol_binding (unsigned int binding){
  switch (binding)
    {
    case STB_LOCAL:	return "LOCAL";
    case STB_GLOBAL:	return "GLOBAL";
    case STB_WEAK:	return "WEAK";
    }
    return "Error";
}

char * get_symbol_visibility (unsigned int visibility){

  switch (visibility){
    case STV_DEFAULT:	return "DEFAULT";
    case STV_INTERNAL:	return "INTERNAL";
    case STV_HIDDEN:	return "HIDDEN";
    case STV_PROTECTED: return "PROTECTED";
  }
  return "Error";
}

char* get_section_name(unsigned int type){
  switch (type){
    case SHT_NOBITS:	return ".bss";
    case SHT_REL:	return ".relTab";
  }
  return "Error";
}

char * get_rel_type(unsigned int type){
  switch (type){
    case R_386_NONE:	return "R_386_NONE";
    case R_386_32:	return "R_386_32";
    case R_386_GOT32:	return "R_386_GOT32";
    case R_386_PLT32:	return "R_386_PLT32";
    case R_386_COPY:	return "R_386_COPY";
    case R_386_GLOB_DAT:	return "R_386_GLOB_DAT";
    case R_386_JMP_SLOT:	return "R_386_JMP_SLOT";
    case R_386_RELATIVE:	return "R_386_RELATIVE";
    case R_386_GOTOFF:	return "R_386_GOTOFF";
    case R_386_GOTPC:	return "R_386_GOTPC";
    case R_ARM_ABS32: return "R_ARM_ABS32";
    case R_ARM_CALL: return "R_ARM_CALL";
    case R_ARM_JUMP24: return "R_ARM_JUMP24";
  }
  return "Type inconnu";
}

Elf32_Sym reverseAllEndiannessSym(Elf32_Sym symStruct){
  if (!is_big_endian()){
    symStruct.st_name = reverse_endianess(symStruct.st_name,sizeof(symStruct.st_name));
    symStruct.st_value = reverse_endianess(symStruct.st_value,sizeof(symStruct.st_value));
    symStruct.st_size = reverse_endianess(symStruct.st_size,sizeof(symStruct.st_size));
    symStruct.st_info = reverse_endianess(symStruct.st_info,sizeof(symStruct.st_info));
    symStruct.st_other = reverse_endianess(symStruct.st_other,sizeof(symStruct.st_other));
    symStruct.st_shndx = reverse_endianess(symStruct.st_shndx,sizeof(symStruct.st_shndx));
  }
  return symStruct;
}

char* getNomSym(FILE *fichier,Elf32_Ehdr header,Elf32_Shdr SymTab,Elf32_Sym Sym){
  char* str = malloc(SectionNameLength*sizeof(char));
	fseek(fichier,SymTab.sh_offset+Sym.st_name,SEEK_SET);
 	fgets(str,SectionNameLength,fichier);
  free(str);
  return str;
}

Elf32_Shdr reverseAllEndiannessSectionHeader(Elf32_Shdr headSection){
    Elf32_Shdr headsec;
    if (!is_big_endian()) {
      headsec.sh_name = reverse_endianess(headSection.sh_name,sizeof(headSection.sh_name));
      headsec.sh_type = reverse_endianess(headSection.sh_type,sizeof(headSection.sh_type));
      headsec.sh_flags = reverse_endianess(headSection.sh_flags,sizeof(headSection.sh_flags));
      headsec.sh_addr = reverse_endianess(headSection.sh_addr,sizeof(headSection.sh_addr));
      headsec.sh_offset = reverse_endianess(headSection.sh_offset,sizeof(headSection.sh_offset));
      headsec.sh_size = reverse_endianess(headSection.sh_size,sizeof(headSection.sh_size));
      headsec.sh_link = reverse_endianess(headSection.sh_link,sizeof(headSection.sh_link));
      headsec.sh_info = reverse_endianess(headSection.sh_info,sizeof(headSection.sh_info));
      headsec.sh_addralign = reverse_endianess(headSection.sh_addralign,sizeof(headSection.sh_addralign));
      headsec.sh_entsize = reverse_endianess(headSection.sh_entsize,sizeof(headSection.sh_entsize));
    }
    return headsec;
}

void readElfFileHeaderSection(FILE* fichier, Elf32_Shdr tabHeadSection[],Elf32_Ehdr header, int nbSection, int tailleEnTeteSection){
  //décallage pour aller aux en-têtes de section
  fseek(fichier,header.e_shoff,SEEK_SET);
  //on récupère les en-têtes
  fread(tabHeadSection,1,nbSection*tailleEnTeteSection,fichier);
  if (!is_big_endian())
  {
    for (int i = 0; i < nbSection; i++) {
      tabHeadSection[i] = reverseAllEndiannessSectionHeader(tabHeadSection[i]);
    }
  }
}


Elf32_Shdr readElfFileTabSymSectionHeader(Elf32_Shdr tabHeadSection[],int nbSection, FILE* fichier){
  Elf32_Shdr sectionSym;
  for (int i = 0; i < nbSection; i++) {
    //on ne rérécupère que les sections qui sont des tables de symboles
    if (tabHeadSection[i].sh_type == SHT_SYMTAB) {
      sectionSym = tabHeadSection[i];
      i = nbSection;
    }
  }
  return sectionSym;
}

void readElfFileTabRelSectionHeader(Elf32_Shdr sectionTabRel[], Elf32_Shdr tabHeadSection[],int nbSection, FILE* fichier, int* nbSectionRel){
  int nbSecRel = 0;
  for (int i = 0; i < nbSection; i++) {
    //on ne rérécupère que les sections qui sont des tables de symboles
    if (tabHeadSection[i].sh_type == SHT_REL) {
      sectionTabRel[nbSecRel] = tabHeadSection[i];
      nbSecRel++;
    }
  }
  *nbSectionRel = nbSecRel;
}


/*void readElfFileStrTable(FILE* fichier, Elf32_Ehdr* header, Elf32_Shdr* tabHeadSection, Elf32_Shdr* sectionSym, Elf32_Shdr* sectionTabRel, int* nbSectionSym, int* nbSectionRel){

  //on récupère l'en-tête de la section string table pour les noms
  Elf32_Shdr sectionNameStringTable = tabHeadSection[header.e_shstrndx];

  //on récupère le tableau de string
  int nbOctetsStrTab =sectionNameStringTable.sh_size;
  char strTabAllChar[nbOctetsStrTab];
  fseek(fichier, sectionNameStringTable.sh_offset, SEEK_SET);
  fread(&strTabAllChar, 1, nbOctetsStrTab, fichier);
  char** tabNoms
    //on parcours nos octets pour récupères les différents noms
  for (int i = 0; i < nbOctetsStrTab; i++) {
    int nbNoms = 0;
    char* nom;
    c = strTabAllChar[i];
    int nbChar = 0;
    while (c != '\0') {
      nom[nbChar] = c;
      nbChar++;
      i++;
      c = strTabAllChar[i];
    }
    tabNoms[nbNoms] = nom;
    nbNoms++;
  }
}*/
void getArrayOfRelEntryNumber(int* arrayRelEntryNumber, Elf32_Shdr sectionTabRel[], int nbSectionRel){
  //on parcours nos sections de relocalisation
  for (int i = 0; i < nbSectionRel; i++) {
    //on récupère le nombre d'entrées'
    int nbEntry = sectionTabRel[i].sh_size / sizeof(Elf32_Rel);
    arrayRelEntryNumber[i] = nbEntry;
  }
}

void readElfFileRelTable(FILE* fichier, Elf32_Ehdr header, Elf32_Shdr tabHeadSection[], Elf32_Shdr sectionSym, Elf32_Shdr sectionTabRel[], int nbSectionRel, Elf32_Rel** relTabArray){
  printf("__________________\n");

  //on parcours nos sections de relocalisation
  for (int j = 0; j < nbSectionRel; j++) {
    //on récupère le nombre d'entrées'
    int nbEntry = sectionTabRel[j].sh_size / sizeof(Elf32_Rel);

    //on récupère le contenu des sections Rel
    fseek(fichier, sectionTabRel[j].sh_offset,SEEK_SET);
    fread(relTabArray[j],1,sizeof(Elf32_Rel)*nbEntry,fichier);

    for (int i = 0; i < nbEntry; i++) {
      relTabArray[j][i] = reverseAllEndiannessRel(relTabArray[j][i]);
      if (j==0) {
        printf("%-10.8x    ",relTabArray[j][i].r_offset);
        printf("%-10.8x", relTabArray[j][i].r_info);
        printf("%-10s\n", get_rel_type(ELF32_R_TYPE(relTabArray[j][i].r_info)));
      }
    }
  }
  printf("\n");
  for (int h = 0; h < nbSectionRel; h++) {
    int nbEntry = sectionTabRel[h].sh_size / sizeof(Elf32_Rel);
    for (int k = 0; k < nbEntry; k++) {
      if (h==0) {
        printf("%-10.8x    ",relTabArray[h][k].r_offset);
        printf("%-10.8x", relTabArray[h][k].r_info);
        printf("%-10s\n", get_rel_type(ELF32_R_TYPE(relTabArray[h][k].r_info)));
      }
    }
  }
  printf("\n");
  printf("__________________\n");

}

void readElfFileSymTable(FILE* fichier, Elf32_Shdr sectionSym, Elf32_Sym* symTab, int nbSymbole, int tailleSymTab){
    //on récupère le contenu de la table de symboles
    fseek(fichier, sectionSym.sh_offset, SEEK_SET);
    fread(symTab, 1, tailleSymTab, fichier);

    for (int i = 0; i < nbSymbole; i++) {
      symTab[i] = reverseAllEndiannessSym(symTab[i]);
    }
}


void displayElfFileRelTab(FILE* fichier, Elf32_Ehdr header, Elf32_Shdr tabHeadSection[], Elf32_Shdr sectionSym, Elf32_Shdr sectionTabRel[], Elf32_Rel** relTabArray, Elf32_Sym symTab[],int nbSectionRel, int arrayRelEntryNumber[]){

  for (int i = 0; i < nbSectionRel; i++) {
    int nbEntry = arrayRelEntryNumber[i];

    //recuperation du nom de la section
    char* str = malloc(SectionNameLength*sizeof(char));
    fseek(fichier,tabHeadSection[header.e_shstrndx].sh_offset + sectionTabRel[i].sh_name, SEEK_SET);
    str=fgets(str, SectionNameLength, fichier);

    printf("Section de relocalisation ' %s ' à l'adresse de décalage 0x%x contient %d entrées\n", str, sectionTabRel[i].sh_offset, nbEntry);

    printf("%-15s%-15s%-10s%-10s%-10s\n", "Décalage","Info","Type","Val.-sym","Noms-symboles");

    Elf32_Rel* relTab = relTabArray[i];

    for (int j = 0; j < nbEntry; j++) {

      printf("%-10.8x    ",relTab[j].r_offset);
      printf("%-10.8x", relTab[j].r_info);
      printf("%-10s\t", get_rel_type(ELF32_R_TYPE(relTab[j].r_info)));

      //recuperation du nom pour ce reloc
      int indexName = ELF32_R_SYM(relTab[j].r_info);
      int symValue = 0;

      symValue = symTab[indexName].st_value;
      printf("%-10.8x", symValue);

      if (strcmp(get_symbol_type(ELF32_ST_TYPE(symTab[indexName].st_info)), "NOTYPE") == 0){
        printf("%d-Symbole \t\t", indexName);

      }else if(strcmp(get_symbol_type(ELF32_ST_TYPE(symTab[indexName].st_info)), "SECTION") == 0){
        int indexSection = symTab[indexName].st_shndx;
        printf("%d-Section \t\t", indexSection);
      }

      printf("\n");
    }
    printf("\n\n");
  }
}


int main(int argc, char *argv[]){
  if(argc == 2){
    FILE* fichier = fopen(argv[1], "r");
    if (fichier != NULL){
      //on initialise le header
      Elf32_Ehdr header = readElfFileHeader(fichier);

      //on initialise le tableau d'en-têtes de section
      Elf32_Shdr tabHeadSection[header.e_shnum] ;
      readElfFileHeaderSection(fichier, tabHeadSection, header, header.e_shnum, header.e_shentsize);

      //on initialise l'en-têtes de la section de symboles
      Elf32_Shdr sectionSym = readElfFileTabSymSectionHeader(tabHeadSection,header.e_shnum, fichier);

      //on initialise le tableau d'en-têtes de relocalisation
      int nbSectionRel = 0;
      Elf32_Shdr sectionTabRel[header.e_shnum]  ;
      readElfFileTabRelSectionHeader(sectionTabRel, tabHeadSection,header.e_shnum, fichier, &nbSectionRel);

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
        relTabArray[i] = malloc(sizeof(tabRelEntryNumber[i] * sizeof(Elf32_Rel)));
      }

      readElfFileRelTable(fichier, header, tabHeadSection, sectionSym, sectionTabRel, nbSectionRel, relTabArray);


      displayElfFileRelTab(fichier, header, tabHeadSection, sectionSym, sectionTabRel, relTabArray, symTab, nbSectionRel, tabRelEntryNumber);

      fclose(fichier);

      for (int i = 0; i < nbSectionRel; i++) {
        free(relTabArray[i]);
      }

    }else{
      printf("Erreur: ouverture fichier\n");
    }

  }else if(argc == 1){
    printf("Erreur: il n'y a pas assez d'arguments\n");

  }else{
    printf("Erreur: il y a trop d'arguments\n");
  }
  return 0;
}
