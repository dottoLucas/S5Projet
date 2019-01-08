q#include <elf.h>
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
  reverseAllEndiannessSym(Sym);
  char* str = malloc(SectionNameLength*sizeof(char));
	fseek(fichier,reverse_endianess(SymTab.sh_offset,sizeof(SymTab.sh_offset))+Sym.st_name,SEEK_SET);
 	fgets(str,SectionNameLength,fichier);
  free(str);
  return str;
}

void readSectionHeaders(Elf32_Shdr *tabHeaderSection, Elf32_Ehdr headerFile, FILE* fichier){

  //décallage pour aller aux en-têtes de section
  fseek(fichier,headerFile.e_shoff,SEEK_SET);

  //on récupère les en-têtes
  fread(&tabHeaderSection,1,headerFile.e_shnum*headerFile.e_shentsize,fichier);

}

char* getNomSection(Elf32_Shdr tabHeaderSection[], Elf32_Shdr sectionTabRel[], Elf32_Ehdr header, int index, FILE* fichier){
  //recuperation du nom de la section
  char str[SectionNameLength*sizeof(char)];
  fseek(fichier,tabHeaderSection[header.e_shstrndx].sh_offset + sectionTabRel[index].sh_name,SEEK_SET);
  str=fgets(str,SectionNameLength,fichier);
  return str;
}

void reverseAllEndiannessSectionHeader(Elf32_Shdr *headSection){
    if (!is_big_endian()) {
      headSection->sh_name = reverse_endianess(headSection->sh_name,sizeof(headSection->sh_name));
      headSection->sh_type = reverse_endianess(headSection->sh_type,sizeof(headSection->sh_type));
      headSection->sh_flags = reverse_endianess(headSection->sh_flags,sizeof(headSection->sh_flags));
      headSection->sh_addr = reverse_endianess(headSection->sh_addr,sizeof(headSection->sh_addr));
      headSection->sh_offset = reverse_endianess(headSection->sh_offset,sizeof(headSection->sh_offset));
      headSection->sh_size = reverse_endianess(headSection->sh_size,sizeof(headSection->sh_size));
      headSection->sh_link = reverse_endianess(headSection->sh_link,sizeof(headSection->sh_link));
      headSection->sh_info = reverse_endianess(headSection->sh_info,sizeof(headSection->sh_info));
      headSection->sh_addralign = reverse_endianess(headSection->sh_addralign,sizeof(headSection->sh_addralign));
      headSection->sh_entsize = reverse_endianess(headSection->sh_entsize,sizeof(headSection->sh_entsize));
    }
}

void reverseAllEndiannessSectionHeaderTab(Elf32_Shdr *tabHeadSection, int nbSection){
  for (int i = 0; i < nbSection; i++) {
    reverseAllEndiannessSectionHeader(&tabHeadSection[i]);
  }
}

void displayElfFileRelTab(char* nomfichier){
  FILE* fichier = fopen(nomfichier, "r");

  if (fichier != NULL){
    //getHeader
    Elf32_Ehdr header = readElfFileHeader(fichier);

    //on initialise le tableau d'en-têtes de section
    Elf32_Shdr tabHeadSection[header.e_shnum];
    readSectionHeaders(tabHeadSection, header,fichier);
    reverseAllEndiannessSectionHeaderTab[tabHeadSection,header.e_shnum];

    Elf32_Shdr sectionTabSym[header.e_shnum];
    Elf32_Shdr sectionTabRel[header.e_shnum];

    int nbSectionRel = 0;
    int nbSectionSym = 0;


    for (int i = 0; i < header.e_shnum; i++) {
      //on ne rérécupère que les sections qui sont des tables de symboles
      if (reverse_endianess(tabHeadSection[i].sh_type,sizeof(tabHeadSection[i].sh_type)) == SHT_SYMTAB) {
        sectionTabSym[nbSectionSym] = tabHeadSection[i];
        nbSectionSym++;
      }
      if(reverse_endianess(tabHeadSection[i].sh_type,sizeof(tabHeadSection[i].sh_type)) == SHT_REL){
        sectionTabRel[nbSectionRel] = tabHeadSection[i];
        nbSectionRel++;
      }
    }

    reverseAllEndiannessSectionHeaderTab(&tabHeadSection);
    reverseAllEndiannessSectionHeaderTab(&sectionTabSym);
    reverseAllEndiannessSectionHeaderTab(&sectionTabRel);


    //on parcours nos sections de type SHT_REL
    for (int j = 0; j < nbSectionRel; j++) {
      //on récupère le nombre d'entrées'
      int nbEntry = reverse_endianess(sectionTabRel[j].sh_size,sizeof(sectionTabRel[j].sh_size))/sizeof(Elf32_Rel);
      Elf32_Rel relTab[nbEntry];

      //recuperation du nom de la section
      char str[SectionNameLength*sizeof(char)];
      str = getNomSection(tabHeaderSection, sectionTabRel, header, index, fichier);

      printf("Section de relocalisation ' %s ' à l'adresse de décalage 0x%x contient %d entrées\n", str,reverse_endianess(sectionTabRel[j].sh_offset,sizeof(sectionTabRel[j].sh_offset)), nbEntry);

      fseek(fichier,reverse_endianess(sectionTabRel[j].sh_offset,sizeof(sectionTabRel[j].sh_offset)),SEEK_SET);
      fread(&relTab,1,sizeof(relTab),fichier);

      //on récupère le contenu des sections Rel
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

        //recuperation du nom pour ce reloc
        int indexName = ELF32_R_SYM(relTab[i].r_info);
        int symValue = 0;
        char* strRel = malloc(SectionNameLength*sizeof(char));

        //on récupère le contenu des tables de symboles
        for (int k = 0; k < nbSectionSym; k++) {
          //printf("test\n" );

          //on récupère le nombre de symbole
          int nbSymbole = reverse_endianess(sectionTabSym[k].sh_size,sizeof(sectionTabSym[k].sh_size))/sizeof(Elf32_Sym);
          Elf32_Sym symTab[nbSymbole];
          fseek(fichier,reverse_endianess(sectionTabSym[k].sh_offset,sizeof(sectionTabSym[k].sh_offset)),SEEK_SET);
          fread(&symTab,1,sizeof(symTab),fichier);
          symValue = reverse_endianess(symTab[indexName].st_value,sizeof(symTab[indexName].st_value));
          printf("%-10.8x", symValue);

          if (strcmp(get_symbol_type(ELF32_ST_TYPE(symTab[indexName].st_info)), "NOTYPE") == 0){

            //on récupère le nom dans la table de symbole
            //strRel  = getNomSym(fichier,header,sectionTabSym[k],symTab[indexName]);
            fseek(fichier,tabHeadSection[header.e_shstrndx].sh_offset + reverse_endianess(symTab[indexName].st_name,sizeof(symTab[indexName].st_name)),SEEK_SET);
     		    fgets(strRel,SectionNameLength,fichier);
            /*fseek(fichier,reverse_endianess(sectionTabStringTab[0].sh_offset,sizeof(sectionTabStringTab[0].sh_offset))+reverse_endianess(symTab[indexName].st_name,sizeof(symTab[indexName].st_name)),SEEK_SET);
            fgets(strRel,SectionNameLength,fichier);*/
            printf("%d-Symbole \t\t", indexName);


          }else if(strcmp(get_symbol_type(ELF32_ST_TYPE(symTab[indexName].st_info)), "SECTION") == 0){

            //on récupère le nom dans la table de section
            int indexSection = reverse_endianess(symTab[indexName].st_shndx,sizeof(symTab[indexName].st_shndx));
            //strRel  = getNomSym(fichier,header,tabHeadSection[indexSection],symTab[indexName]);

            fseek(fichier,tabHeadSection[header.e_shstrndx].sh_offset + reverse_endianess(tabHeadSection[indexSection].sh_name,sizeof(tabHeadSection[indexSection].sh_name)),SEEK_SET);
     		    fgets(strRel,SectionNameLength,fichier);
            printf("%d-Section \t\t", indexSection);

          }

        }
        //printf("%-10s", strRel);

        //printf("%-10d", indexName);
        printf("\n");
        //free(strRel);

      }
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
