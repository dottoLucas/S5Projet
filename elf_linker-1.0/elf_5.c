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

Elf32_Rel reverseAllEndianness(Elf32_Rel relStruct){
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
  }
  return "Error";
}


void displayElfFileRelTab(char* nomfichier){
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
    Elf32_Shdr sectionTabRel[header.e_shnum];
    int nbSectionRel = 0;
    for (int i = 0; i < header.e_shnum; i++) {
      //on ne rérécupère que les sections qui sont des tables de symboles
      if(reverse_endianess(tabHeadSection[i].sh_type,sizeof(tabHeadSection[i].sh_type)) == SHT_REL){
        sectionTabRel[nbSectionRel] = tabHeadSection[i];
        nbSectionRel++;
      }
    }
    //on parcours nos sections de type SHT_REL
    for (int j = 0; j < nbSectionRel; j++) {
      //on récupère le nombre d'entrées'
      int nbEntry = reverse_endianess(sectionTabRel[j].sh_size,sizeof(sectionTabRel[j].sh_size))/sizeof(Elf32_Rel);
      Elf32_Rel relTab[nbEntry];

      //recuperation du nom de la section
      char* str = malloc(SectionNameLength*sizeof(char));
      fseek(fichier,reverse_endianess(tabHeadSection[header.e_shstrndx].sh_offset,sizeof(tabHeadSection[header.e_shstrndx].sh_offset))+reverse_endianess(sectionTabRel[j].sh_name,sizeof(sectionTabRel[j].sh_name)),SEEK_SET);
      str=fgets(str,SectionNameLength,fichier);

      printf("Section de relocalisation ' %s ' à l'adresse de décalage 0x%x contient %d entrées\n", str,reverse_endianess(sectionTabRel[j].sh_offset,sizeof(sectionTabRel[j].sh_offset)), nbEntry);
      //on récupère le contenu des sections
      fseek(fichier,reverse_endianess(sectionTabRel[j].sh_offset,sizeof(sectionTabRel[j].sh_offset)),SEEK_SET);
      fread(&relTab,1,sizeof(relTab),fichier);
      printf("%-15s%-15s%-10s%-10s%-10s\n", "Décalage","Info","Type","Val.-sym","Noms-symboles");
      for (int i = 0; i < nbEntry; i++) {
        relTab[i] = reverseAllEndianness(relTab[i]);
        fseek(fichier,sectionTabRel[j].sh_offset+relTab[i].r_offset,SEEK_SET);
        fread(&(relTab[i]),1,sizeof(Elf32_Rel),fichier);
        printf("%-10.8x    ",relTab[i].r_offset);
        printf("%-10.8x", relTab[i].r_info);
        printf("%-10s\t", get_rel_type(ELF32_R_TYPE(relTab[i].r_info)));
        printf("%-10.8x", (ELF32_R_SYM(relTab[i].r_info)));
        //printf("%-10s", get_rel_symName(relTab[i].r_info));
        printf("\n");
      }
      free(str);
      printf("\n\n");
    }
    fclose(fichier);
  }else{
    printf("Erreur: ouverture fichier\n");
  }
}

int main(int argc, char *argv[]){
  if(argc == 2){
    displayElfFileRelTab(argv[1]);
  }else if(argc == 1){
    printf("Erreur: il n'y a pas assez d'arguments\n");
  }else{
    printf("Erreur: il y a trop d'arguments\n");
  }
  return 0;
}
