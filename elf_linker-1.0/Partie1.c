
#include <elf.h>
#include <stdio.h>
#include "debug.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "util.c"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

int recurpererNumSec(Elf32_Shdr* TableHs,Elf32_Ehdr data,FILE *fichier,char* param){
  int i=0;
  char* str = malloc(SectionNameLength*sizeof(char));
  fseek(fichier,TableHs[data.e_shstrndx].sh_offset+TableHs[i].sh_name,SEEK_SET);
  fgets(str,SectionNameLength,fichier);
  while(strcmp(str,param) !=0 ){
    i++;
    fseek(fichier,TableHs[data.e_shstrndx].sh_offset+TableHs[i].sh_name,SEEK_SET);
    fgets(str,SectionNameLength,fichier);
  }

  free(str);
  return i;
}

//----------------------------------------------------------------
//----------------------------Partie 1----------------------------
//----------------------------------------------------------------

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


void printfheader(Elf32_Ehdr header){

    printf("En-tête ELF:\n");

    //_________________________________________
    //MAGIQUE
    //_________________________________________
    printf("Magique:    ");
    for (int i = 0; i < EI_NIDENT; i++) {
      printf("%2x ",header.e_ident[i]);
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
      case 40:
      printf("ARM");break;
    }
    printf("\n");

    printf("Version:                           0x%d\n",header.e_version);
    printf("Entry:                             0x%x\n",header.e_entry);
    printf("Début des en-têtes de programme:   %d(octets dans le fichier)\n",header.e_phoff);
    printf("Début des en-têtes de section:     %d(octets dans le fichier)\n",header.e_shoff);
    printf("Fanions:                           0x%x\n",header.e_flags);
    printf("Taille de cet en-tête:             %d(bytes)\n",header.e_ehsize);
    printf("Taille de l'en-tête du programme:: %d(bytes)\n",header.e_phentsize);
    printf("Nombre d'en-tête du programme:     %d\n",header.e_phnum);
    printf("Taille des en-têtes de section:    %d(bytes)\n",header.e_shentsize);
    printf("Nombre d'en-têtes de section:      %d\n",header.e_shnum);
    printf("Table d'indexes des chaînes d'en-tête de section:   %d\n",header.e_shstrndx);
    printf("\n");


}



Elf32_Ehdr displayElfFileHeader(char* fichierElf,int b){
  FILE* fichier = fopen(fichierElf, "r");
  Elf32_Ehdr header;
  if (fichier != NULL){

    header = readElfFileHeader(fichier);
    if(b){
      printfheader(header);
    }
  }else{
    printf("Erreur: ouverture fichier\n");
  }
  fclose(fichier);
  return header;
}

//----------------------------------------------------------------
//----------------------------Partie 2----------------------------
//----------------------------------------------------------------

// prerequis le fichier doit etre ouvert en lecture et pointe vers le debut du "section header"
void readHsHeader(Elf32_Shdr *Hs,  FILE * fichier){

  fread(Hs,1,sizeof(Elf32_Shdr), fichier);
    if (!is_big_endian()){
    Hs->sh_name      = reverse_endianess(Hs->sh_name,sizeof(Hs->sh_name));
    Hs->sh_type      = reverse_endianess(Hs->sh_type,sizeof(Hs->sh_type));
    Hs->sh_flags     = reverse_endianess(Hs->sh_flags,sizeof(Hs->sh_flags));
    Hs->sh_addr      = reverse_endianess(Hs->sh_addr,sizeof(Hs->sh_addr));
    Hs->sh_offset    = reverse_endianess(Hs->sh_offset,sizeof(Hs->sh_offset));
    Hs->sh_size      = reverse_endianess(Hs->sh_size,sizeof(Hs->sh_size));
    Hs->sh_link      = reverse_endianess(Hs->sh_link,sizeof(Hs->sh_link));
    Hs->sh_info      = reverse_endianess(Hs->sh_info,sizeof(Hs->sh_info));
    Hs->sh_addralign = reverse_endianess(Hs->sh_addralign,sizeof(Hs->sh_addralign));
    Hs->sh_entsize   = reverse_endianess(Hs->sh_entsize,sizeof(Hs->sh_entsize));
  }
}

char* find_type(Elf32_Shdr Hs_header){

  char* type;

  switch (Hs_header.sh_type){
    case SHT_NULL:      //sh_type = 0
      type="NULL";
    break;

    case SHT_PROGBITS:    //sh_type = 1
      type="PROGBITS";
    break;

    case SHT_SYMTAB:    //sh_type = 2
      type="SYMTAB";
    break;

    case SHT_STRTAB:    //sh_type = 3
      type="STRTAB";
    break;

    case SHT_RELA:      //sh_type = 4
      type="RELA";
    break;

    case SHT_HASH:      //sh_type = 5
      type="HASH";
    break;

    case SHT_DYNAMIC:   //sh_type = 6
      type="DYNAMI";
    break;

    case SHT_NOTE:      //sh_type = 7
      type="NOTE";
    break;

    case SHT_NOBITS:    //sh_type = 8
      type="NOBITS";
    break;

    case SHT_REL:     //sh_type = 9
      type="REL";
    break;

    case SHT_SHLIB:     //sh_type = 10
      type="SHLIB";
    break;

    case SHT_DYNSYM:    //sh_type = 11
      type="DYNSYM";
    break;

    case SHT_LOPROC:    //sh_type = 0x70000000
      type="LOPROC";
    break;

    case SHT_HIPROC:    //sh_type = 0x7fffffff
      type="HIPROC";
    break;

    case SHT_LOUSER:    //sh_type = 0x80000000
      type="LOUSER";
    break;

    case SHT_HIUSER:    //sh_type = 0xffffffff
      type="HIUSER";
    break;
    case SHT_ARM_ATTRIBUTES:    //sh_type = 0xffffffff
      type="ARM_ATTRIBUTES";
    break;
  default:break;
  }

  return type;
}


void print_flag(unsigned int flag){
  //printf("\t ");
  if((unsigned int)SHF_LINK_ORDER & flag)
    printf("L");

  if((unsigned int)SHF_MASKPROC & flag)
    printf("p");

  if((unsigned int)SHF_OS_NONCONFORMING & flag)
    printf("x");

  if((unsigned int)SHF_INFO_LINK & flag)
    printf("I");

  if((unsigned int)SHF_MERGE & flag)
    printf("M");

  if((unsigned int)SHF_TLS & flag)
    printf("T");

  if((unsigned int)SHF_STRINGS & flag)
    printf("S");

  if((unsigned int)SHF_EXCLUDE & flag)
    printf("E");

  if ((unsigned int)SHF_WRITE & flag)
    printf("W");

  if ((unsigned int)SHF_ALLOC & flag)
    printf("A");

  if((unsigned int)SHF_EXECINSTR & flag)
    printf("X");

  if((unsigned int)flag & SHF_MASKOS)
    printf("o");

  if((unsigned int)SHF_ORDERED & flag)
    printf("O");

  if((unsigned int)SHF_GROUP & flag)
    printf("G");
  printf("\t\t");
}

void afficherHeader(Elf32_Shdr *TableHs,Elf32_Ehdr *data,FILE *fichier){

  Elf32_Shdr Hs_header;
    char* type;
    char* str = malloc(SectionNameLength*sizeof(char));
    printf("il y a %d en-tête de section ,debutant a l'adresse de decalage\n",data->e_shnum );
    printf("En-tête de section\n");
    printf("  [Nr]\t%-15s\t\t%-15s\t\t%-15s\t%-15s\t%s\t    %2s\t %s\t\t%s\t%s\t%s\n","Nom","Type","Adr","Décala.","Taille","ES","Fan","LN","Inf","Al");
    for(int i=0;i<data->e_shnum;i++){
      Hs_header = TableHs[i];
      type=find_type(Hs_header);
      printf("  [%2d]\t",i);                //affichage de 'Nr'
      fseek(fichier,TableHs[data->e_shstrndx].sh_offset+Hs_header.sh_name,SEEK_SET);
      fgets(str,SectionNameLength,fichier);
      printf("%-15s",str);  //affichage de 'Nom'
      if(strlen(str)<16)                   //un 'if' pour un affichage bien alligné
      	printf("\t\t%-15s\t\t",type);//affichage de 'Type'
      else
	printf("\t%-15s\t\t",type);
      printf("%.8x\t",Hs_header.sh_addr);     //affichage de 'Adresse'
      printf("%.6x\t\t",Hs_header.sh_offset);    //affichage de 'Décalage'
      printf("%.6x\t    ",Hs_header.sh_size);     //affichage de 'Taille'
      printf("%.2x\t  ",Hs_header.sh_entsize);    //affichage de 'TaillEntré'

      print_flag(Hs_header.sh_flags);       //affichage de 'Fanion'
      printf("%d\t ",Hs_header.sh_link);        //affichage de 'Lien'
      printf("%d\t ",Hs_header.sh_info);        //affichage de 'Info'
      printf("%d\n",Hs_header.sh_addralign);    //affichage de 'Alignement'
    }
    printf("Clé des fanions :\n");
    printf("  W (écriture), A (allocation), X (exécution), M (fusion), S (chaînes)\n");
    printf("  I (info), L (ordre des liens), G (groupe), T (TLS), E (exclu), x (inconnu)\n");
    printf("  O (traitement supplémentaire par lOS requis), o (spécifique à lOS), p (spécifique au processeur)\n");
    free(str);
}
////////////////////////////////////////////////////////////////////////////////////////Hs_header.sh_flags

void elf_print_header(Elf32_Shdr **TableHs, FILE* fichier,Elf32_Ehdr *data){

  //decalage  du courseur vers table Hs header
  fseek(fichier,data->e_shoff,SEEK_SET);
  // declaration table HEADER Hs
  for (int i = 0; i < data->e_shnum; ++i) {
    readHsHeader(&((*TableHs)[i]), fichier);
  }

      //////////////////////////////////////////////////////////////////
}


//----------------------------------------------------------------
//----------------------------Partie 3----------------------------
//----------------------------------------------------------------

int VerfifierParametre(char* param,Elf32_Ehdr data,Elf32_Shdr* TableHs,FILE* fichier){
  int i;
   i=atoi(param);
  if(i>=data.e_shnum ){
        i=-1;
  }else if(i>= 0 && i<= data.e_shnum && isalnum(param[0])){
    return i;
  }else{
    i=recurpererNumSec(TableHs,data,fichier,param);
  }
  return i;
}

void afficherSection(FILE *fichier, Elf32_Shdr Section_a_traitee ){
  fseek(fichier,Section_a_traitee.sh_offset,SEEK_SET);
  if(Section_a_traitee.sh_size == 0){
    printf("Cette section est vide\n");
  }
  else{
    int NbOctetLu = 0;
    unsigned char* string = malloc(16*sizeof(unsigned char));
    printf("\n");
    while(NbOctetLu < Section_a_traitee.sh_size){
      printf("0x%.8x ",NbOctetLu );
      for (int i = 0; i < 16; i++)
      {
        if(i%4 == 0 && i != 0){
          printf(" ");
        }
        string[i] =  fgetc(fichier);
        printf("%02X",string[i]);

      }

      // afficher les data lu en lettre imprimable
      printf(" ");
      for (int i = 0; i < 16; ++i)
      {
        if(string[i]<='~' && string[i]>=' '){
          printf("%c",string[i]);
        }
        else{
          printf(".");
        }
      }
      printf("\n");
      NbOctetLu += 16;
    }
  }
  printf("\n");

}

//----------------------------------------------------------------
//----------------------------Partie4-----------------------------
//----------------------------------------------------------------

void readSymTab(Elf32_Sym *Sym,  FILE * fichier){

  fread(Sym,1,sizeof(Elf32_Sym), fichier);
    if (!is_big_endian()){
    Sym->st_name      = reverse_endianess(Sym->st_name,sizeof(Sym->st_name));
    Sym->st_value     = reverse_endianess(Sym->st_value,sizeof(Sym->st_value));
    Sym->st_size      = reverse_endianess(Sym->st_size,sizeof(Sym->st_size));
    Sym->st_info      = reverse_endianess(Sym->st_info,sizeof(Sym->st_info));
    Sym->st_other      = reverse_endianess(Sym->st_other,sizeof(Sym->st_other));
    Sym->st_shndx    = reverse_endianess(Sym->st_shndx,sizeof(Sym->st_shndx));
  }
}

char* findSymType(Elf32_Sym Sym ){

  char* type;

  switch (ELF32_ST_TYPE(Sym.st_info)){
    case STT_NOTYPE:
      type="NOTYPE";
    break;

    case STT_OBJECT:
      type="OBJECT";
    break;

    case STT_FUNC:
      type="FUNC";
    break;

    case STT_SECTION:
      type="SECTION";
    break;

    case STT_FILE :
      type="FILE ";
    break;

    case STT_LOPROC:
      type="LOPROC";
    break;

    case STT_HIPROC:
      type="HIPROC";
    break;
    default:
    break;
  }

  return type;
}
char* findSymLink(Elf32_Sym Sym ){
  char* link;
  switch (ELF32_ST_BIND(Sym.st_info)){
    case STB_LOCAL :
      link="LOCAL";
    break;
    case STB_GLOBAL :
      link="GLOBAL";
    break;
    case STB_WEAK :
      link="WEAK";
    break;
    case STB_LOPROC :
      link="LOPROC";
    break;
    case STB_HIPROC :
      link="HIPROC";
    break;
    default :
    break;
  }
  return link;
}

void AfficherNom(FILE *fichier,Elf32_Ehdr *data,Elf32_Shdr SmyTab,Elf32_Sym *Sym){
  char* str = malloc(SectionNameLength*sizeof(char));
  fseek(fichier,SmyTab.sh_offset+Sym->st_name,SEEK_SET);
  //printf("%s\n",TableHs[data->e_sh  dx].sh_size ) ;
  //TableHs[data->e_shstrndx].sh_offset+Sym->st_name
  //afficherSection(fichier,TableHs[data->e_shstrndx]);
  fgets(str,SectionNameLength,fichier);
    printf("%-15s\t\t\n",str);
    //printf("%s\n", TableHs[data->e_shstrndx]);
    free(str);
}

void affichageSymTab(char *fichierG,Elf32_Shdr SmyTab,Elf32_Ehdr *data,Elf32_Shdr *TableHs){
  char *type;
  char* link;

  FILE* fichier = fopen(fichierG, "r");
  FILE* fichierNom = fopen(fichierG, "r");
  Elf32_Sym *Sym=malloc(sizeof(Elf32_Sym));
  int indice=recurpererNumSec(TableHs,*data,fichier,".strtab");
  fseek(fichier,SmyTab.sh_offset,SEEK_SET);
  printf("Symbol table \".symtab\" contains %d entries:\n",SmyTab.sh_info+1);
  printf("%s\t%s\t\t%s\t%s\t%s\t%s\t%s\t%s\n","Num","value","Taille","Type","Lien","vis","Ndx","Nom");
  for (int i = 0; i <=SmyTab.sh_info ; ++i)
  {
    readSymTab(Sym,fichier);
    printf("%2d:\t",i );
    printf("%.8x\t",Sym->st_value);
    printf("%3d\t",Sym->st_size);
    type=findSymType(*Sym);
    printf("%s\t",type);
    link=findSymLink(*Sym);
    printf("%s\t",link);
    if(Sym->st_other==0)
      printf("%s\t","DEFAULT");
    else
      printf("%d\t",Sym->st_other);
    if(Sym->st_shndx==0)
      printf("%s\t","UND");
    else
      printf("%3d\t",Sym->st_shndx);
    AfficherNom(fichierNom,data,TableHs[indice],Sym);
  }
  printf("\n");
}


//----------------------------------------------------------------
//----------------------------Partie5-----------------------------
//----------------------------------------------------------------



Elf32_Rel reverseAllEndiannessRel(Elf32_Rel relStruct){
  if (!is_big_endian()){
    relStruct.r_offset = reverse_endianess(relStruct.r_offset,sizeof(relStruct.r_offset));
    relStruct.r_info = reverse_endianess(relStruct.r_info,sizeof(relStruct.r_info));
  }
  return relStruct;
}


char* get_symbol_type (unsigned int type){
  switch (type)
    {
    case STT_NOTYPE:  return "NOTYPE";
    case STT_OBJECT:  return "OBJECT";
    case STT_FUNC:  return "FUNC";
    case STT_SECTION: return "SECTION";
    case STT_FILE:  return "FILE";
    case STT_COMMON:  return "COMMON";
    case STT_TLS: return "TLS";
    }
    return "Error";
}

char * get_symbol_binding (unsigned int binding){
  switch (binding)
    {
    case STB_LOCAL: return "LOCAL";
    case STB_GLOBAL:  return "GLOBAL";
    case STB_WEAK:  return "WEAK";
    }
    return "Error";
}

char * get_symbol_visibility (unsigned int visibility){

  switch (visibility){
    case STV_DEFAULT: return "DEFAULT";
    case STV_INTERNAL:  return "INTERNAL";
    case STV_HIDDEN:  return "HIDDEN";
    case STV_PROTECTED: return "PROTECTED";
  }
  return "Error";
}

char* get_section_name(unsigned int type){
  switch (type){
    case SHT_NOBITS:  return ".bss";
    case SHT_REL: return ".relTab";
  }
  return "Error";
}

char * get_rel_type(unsigned int type){
  switch (type){
    case R_386_NONE:  return "R_386_NONE";
    case R_386_32:  return "R_386_32";
    case R_386_GOT32: return "R_386_GOT32";
    case R_386_PLT32: return "R_386_PLT32";
    case R_386_COPY:  return "R_386_COPY";
    case R_386_GLOB_DAT:  return "R_386_GLOB_DAT";
    case R_386_JMP_SLOT:  return "R_386_JMP_SLOT";
    case R_386_RELATIVE:  return "R_386_RELATIVE";
    case R_386_GOTOFF:  return "R_386_GOTOFF";
    case R_386_GOTPC: return "R_386_GOTPC";
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
  void* unused __attribute__((unused))=fgets(str,SectionNameLength,fichier);
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
  int unused __attribute__((unused))=fread(tabHeadSection,1,nbSection*tailleEnTeteSection,fichier);
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

void getArrayOfRelEntryNumber(int* arrayRelEntryNumber, Elf32_Shdr sectionTabRel[], int nbSectionRel){
  //on parcours nos sections de relocalisation
  for (int i = 0; i < nbSectionRel; i++) {
    //on récupère le nombre d'entrées'
    int nbEntry = sectionTabRel[i].sh_size / sizeof(Elf32_Rel);
    arrayRelEntryNumber[i] = nbEntry;
  }
}

void readElfFileRelTable(FILE* fichier, Elf32_Ehdr header, Elf32_Shdr tabHeadSection[], Elf32_Shdr sectionSym, Elf32_Shdr sectionTabRel[], int nbSectionRel, Elf32_Rel** relTabArray, int* tabRelEntryNumber){

  //on parcours nos sections de relocalisation
  for (int j = 0; j < nbSectionRel; j++) {
    //on récupère le nombre d'entrées'
    //int nbEntry = sectionTabRel[j].sh_size / sizeof(Elf32_Rel);
    int nbEntry = tabRelEntryNumber[j];

    //on récupère le contenu des sections Rel
    fseek(fichier, sectionTabRel[j].sh_offset,SEEK_SET);
    int unused __attribute__((unused))=fread(relTabArray[j],1,sizeof(Elf32_Rel)*nbEntry,fichier);

    for (int i = 0; i < nbEntry; i++) {
      relTabArray[j][i] = reverseAllEndiannessRel(relTabArray[j][i]);
    }
  }
}

void readElfFileSymTable(FILE* fichier, Elf32_Shdr sectionSym, Elf32_Sym* symTab, int nbSymbole, int tailleSymTab){
    //on récupère le contenu de la table de symboles
    fseek(fichier, sectionSym.sh_offset, SEEK_SET);
    int unused __attribute__((unused))=fread(symTab, 1, tailleSymTab, fichier);

    for (int i = 0; i < nbSymbole; i++) {
      symTab[i] = reverseAllEndiannessSym(symTab[i]);
    }
}

char* getName(FILE* fichier, Elf32_Shdr tabHeadSection[], Elf32_Ehdr header, int index, int indexStrTab){
  //recuperation du nom de la section
  char* strName = malloc(60*sizeof(char));
  fseek(fichier,tabHeadSection[indexStrTab].sh_offset + index, SEEK_SET);

  char c = fgetc(fichier);
  int h = 0;
  while (c != '\0') {
    strName[h] = c;
    c = fgetc(fichier);
    h++;
  }
  return strName;
}


void displayElfFileRelTab(FILE* fichier, Elf32_Ehdr header, Elf32_Shdr tabHeadSection[], Elf32_Shdr sectionSym, Elf32_Shdr sectionTabRel[], Elf32_Rel** relTabArray, Elf32_Sym symTab[],int nbSectionRel, int arrayRelEntryNumber[], int indiceSymTab){

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


        //recuperation du nom de la section
        char* strName = malloc(60*sizeof(char));
        int indexStrTabSym = tabHeadSection[indiceSymTab].sh_link;
        /*
        int indexStrTabSym = 0;
        int strTabSymTrouve = 0;
        for (int k = 0; k < header.e_shnum; k++) {
          if (strTabSymTrouve == 0 && k != header.e_shstrndx &&tabHeadSection[k].sh_type == SHT_STRTAB) {
            strTabSymTrouve = 1;
            indexStrTabSym = k;
          }
        }*/
        strName = getName(fichier, tabHeadSection, header, symTab[indexName].st_name, indexStrTabSym);
        //printf("%d-Symbole / indiceStrTab: %d / nom: %s \t\t", indexName, symTab[indexName].st_name, strName);
        printf("%s \t\t", strName);

      }else if(strcmp(get_symbol_type(ELF32_ST_TYPE(symTab[indexName].st_info)), "SECTION") == 0){

        int indexSection = symTab[indexName].st_shndx;


        //recuperation du nom de la section
        char* strName = malloc(60*sizeof(char));
        strName = getName(fichier, tabHeadSection, header, tabHeadSection[indexSection].sh_name, header.e_shstrndx);
        //printf("%d-Section / indiceStrTab: %d / nom: %s \t\t", indexSection, tabHeadSection[indexSection].sh_name, strName);
        printf("%s \t\t", strName);
      }

      printf("\n");
    }
    printf("\n\n");
  }
}
