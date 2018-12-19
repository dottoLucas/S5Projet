#include <elf.h>
#include "util.c"
#include "elfCustom.h"
#include <stdio.h>


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

Elf32_Sym reverseAllEndianness(Elf32_Sym symStruct){
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
    case SHT_SYMTAB:	return ".symtab";
  }
  return "Error";
}


void displayElfFileSymTab(char* nomfichier){
  FILE* fichier = fopen(nomfichier, "r");
  if (fichier != NULL){
    //getHeader
    Elf32_Ehdr header = readElfFileHeader(fichier);
    //décallage pour aller aux en-têtes de section
    fseek(fichier,header.e_shoff,SEEK_SET);
    //on initialise le tableau d'en-têtes de section
    Elf32_Shdr tabHeadSection[header.e_shnum];
    //on récupère les en-têtes
    fread(&tabHeadSection,1,header.e_shnum*header.e_shentsize,fichier);
    // ATTENTION il faut peut etre reverse_endianess ici ATTENTION
    Elf32_Shdr sectionTabSym[header.e_shnum];
    int nbSectionSymTab = 0;
    for (int i = 0; i < header.e_shnum; i++) {
      //on ne rérécupère que les sections qui sont des tables de symboles
      if(reverse_endianess(tabHeadSection[i].sh_type,sizeof(tabHeadSection[i].sh_type)) == SHT_SYMTAB){
        sectionTabSym[nbSectionSymTab] = tabHeadSection[i];
        nbSectionSymTab++;
      }
    }
    //on parcours nos sections de type SHT_SYMTAB
    for (int j = 0; j < nbSectionSymTab; j++) {
      //on récupère le nombre de symbole
      int nbSymbole = reverse_endianess(sectionTabSym[j].sh_size,sizeof(sectionTabSym[j].sh_size))/sizeof(Elf32_Sym);
      Elf32_Sym symTab[nbSymbole];
      printf("Table de symboles de << %s >> contient %d entrées\n", get_section_name(reverse_endianess(sectionTabSym[j].sh_type,sizeof(sectionTabSym[j].sh_type))), nbSymbole);
      //on récupère le contenu des sections
      fseek(fichier,sectionTabSym[j].sh_offset,SEEK_SET);
      fread(&symTab,1,sizeof(symTab),fichier);
      printf("%-10s%-10s%-10s%-20s%-10s%-10s%-10s%-10s\n", "Num","Valeur","Tail","Type","Lien","Vis","Ndx","Nom");
      for (int i = 0; i < nbSymbole; i++) {
        //symTab[i] = reverseAllEndianness(symTab[i]);
        //printf("%d:   %d  %d  %d  %s  %s  %d  %d\n",i ,symTab[i].st_name,symTab[i].st_value,symTab[i].st_size,get_symbol_type(symTab[i].st_info), get_symbol_binding(symTab[i].st_info),symTab[i].st_other,symTab[i].st_shndx);
        printf("%-10d:  ",i);
        fseek(fichier,sectionTabSym[j].sh_offset+symTab[i].st_value,SEEK_SET);
        fread(&(symTab[i].st_value),1,sizeof(symTab[i].st_value),fichier);
        printf("%-10d    ",symTab[i].st_value);
        printf("%-10d    ",symTab[i].st_size);
        printf("%-10s", get_symbol_type(ELF32_ST_TYPE(symTab[i].st_info)));
        printf("%-10s", get_symbol_binding(ELF32_ST_BIND(symTab[i].st_info)));
        printf("%-10s",get_symbol_visibility(symTab[i].st_info));
        printf("%-10d",symTab[i].st_shndx);
        printf("%-10d    ",symTab[i].st_name);
        printf("\n");
      }
    }
    fclose(fichier);
  }else{
    printf("Erreur: ouverture fichier\n");
  }
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
