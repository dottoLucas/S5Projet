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

Elf32_Ehdr displayElfFileHeader(char* fichierElf){
  FILE* fichier = fopen(fichierElf, "r");
  Elf32_Ehdr header;
  if (fichier != NULL){
    
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
  }else{
    printf("Erreur: ouverture fichier\n");
  }
  fclose(fichier);
  return header;
}

//----------------------------------------------------------------
//----------------------------Partie 2----------------------------
//----------------------------------------------------------------

// prerequis le fichier doit etre ouver en lecture et point vers le debut du section header
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
void afficherHeader(Elf32_Shdr *TableHs,Elf32_Ehdr *data,FILE *fichier){

	Elf32_Shdr Hs_header;
  	char* type;
  	char* str = malloc(SectionNameLength*sizeof(char));
    printf("il y a %d en-tête de section ,debutant a l'adresse de decalage\n",data->e_shnum );
    printf("En-tête de section\n");
  	printf("[Nr]\t%-15s\t\t%-15s\t\t%-15s\t%-15s\t%-15s\t%-15s\t%s  %s  %s   \t%-15s\n","Nom","Type","Adresse","Decalage","Taille","TaillEntre","Fanion","Lien","Info","Alignement");
  	for(int i=0;i<data->e_shnum;i++){
    	Hs_header = TableHs[i];
    	type=find_type(Hs_header);
      printf("[%2d]\t",i);                //affichage de 'Nr'
      fseek(fichier,TableHs[data->e_shstrndx].sh_offset+Hs_header.sh_name,SEEK_SET);
 		  fgets(str,SectionNameLength,fichier);
      printf("%-15s\t\t",str);  //affichage de 'Nom'	
      printf("%-15s\t\t",type);             //affichage de 'Type'
      printf("%.8x\t",Hs_header.sh_addr);     //affichage de 'Adresse'
      printf("%.6x\t\t",Hs_header.sh_offset);    //affichage de 'Décalage'
      printf("%.6x\t\t    ",Hs_header.sh_size);     //affichage de 'Taille'
      printf("%.2x\t\t  ",Hs_header.sh_entsize);    //affichage de 'TaillEntré'
      printf("%x\t ",Hs_header.sh_flags);       //affichage de 'Fanion'
      printf("%d\t ",Hs_header.sh_link);        //affichage de 'Lien'
      printf("%d\t ",Hs_header.sh_info);        //affichage de 'Info'
      printf("%d\n",Hs_header.sh_addralign);    //affichage de 'Alignement'
  	}
    printf("Clé des fanions :");
  printf("W (écriture), A (allocation), X (exécution), M (fusion), S (chaînes), I (info),\n");
  printf("L (ordre des liens), O (traitement supplémentaire par lOS requis), G (groupe),\n");    
  printf("T (TLS), C (compressé), x (inconnu), o (spécifique à lOS), E (exclu),\n");      
  printf("y (purecode), p (processor specific)\n");
  	free(str);
}

void elf_print_header(Elf32_Shdr **TableHs, FILE* fichier,Elf32_Ehdr *data){
  
  //decalage  du courseur vers table Hs header
  fseek(fichier,data->e_shoff,SEEK_SET); 
  // declaration table HEADER Hs
  for (int i = 0; i < data->e_shnum; ++i) {
  	readHsHeader(&((*TableHs)[i]), fichier);
  }
  
  afficherHeader(*TableHs,data,fichier);       
}


//----------------------------------------------------------------
//----------------------------Partie 3----------------------------
//----------------------------------------------------------------

int VerfifierParametre(char* param,Elf32_Ehdr data,Elf32_Shdr* TableHs,FILE* fichier){
	int i;
	 i=atoi(param);
	if(i>data.e_shnum ){
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
	printf("%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n","Num","value","Taille","Type","Lien","vis","Ndx","Nom");
	for (int i = 0; i <=SmyTab.sh_info ; ++i)
	{
		readSymTab(Sym,fichier);
		printf("%d\t\t",i );
		printf("%.8x\t",Sym->st_value);
		printf("%d\t\t",Sym->st_size);
		type=findSymType(*Sym);
		printf("%s\t\t",type);
		link=findSymLink(*Sym);
		printf("%s\t\t",link);
		if(Sym->st_other==0)
			printf("%s\t\t","DEFAULT");
		else
			printf("%d\t\t",Sym->st_other);
		if(Sym->st_shndx==0)
			printf("%s\t\t","UND");
		else
			printf("%d\t\t",Sym->st_shndx);
		AfficherNom(fichierNom,data,TableHs[indice],Sym);
	}
  printf("\n");
}

