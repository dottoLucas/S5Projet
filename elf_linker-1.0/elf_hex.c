#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <elf.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "elfCustom.h"

// Lire le fichier ELF

Elf32Custom_Ehdr readElfFileHeader(FILE* fichier){
  Elf32Custom_Ehdr h;
  fread(&h,1,sizeof(h),fichier);
  return h;
}

// CONVERT CHAR TO HEXADECIMAL

void elf_hexa_convert(int octets,uint32_t addr,unsigned char *data)
{
	  while (octets)
    {
      int j;
      int k;
      int loctets;

      loctets = (octets > 16 ? 16 : octets);

      printf ("  0x%8.8lx ", (unsigned long) addr);

      for (j = 0; j < 16; j++)
		{
		  if (j < loctets)
	 	   printf ("%2.2x", data[j]);
	  else
	   	   printf ("  ");

	  if ((j & 3) == 3)
	       printf (" ");
	}

      for (j = 0; j < loctets; j++)
		{
	 	 k = data[j];
	 	 if (k >= ' ' && k < 0x7f)
	 	   printf ("%c", k);
	 	 else
	  	  printf (".");
		}

      putchar ('\n');

      data  += loctets;
      addr  += loctets;
      octets -= loctets;
    }

}

// CHERCHE LE HEADER PASSER EN ARGUMENT ET RETURN UN POINTEUR VERS CE DERNIER

Elf32_Shdr * trouver_section(char * nom_section,Elf32Custom_Ehdr header,Elf32_Shdr* sections_headers)
{
	int i;
	for(i = 0; i < header.e_shnum; i++)
   	if(strcmp((char*)sections_headers + i, nom_section) == 0)
      return sections_headers + i;

  return NULL;
}



void elf_hexa(char filename[],Elf32Custom_Ehdr header,char* nom_section)
{
	FILE *pf;
	Elf32_Shdr *sections_headers;
	uint32_t addr;
	unsigned  char * data;
	int octets = sections_headers->sh_size;
	char c;
	char hex;
	pf = fopen(filename,"r+");
	header = readElfFileHeader(pf);
	sections_headers= trouver_section(nom_section,header,sections_headers);
	if (octets == 0 || sections_headers->sh_type == SHT_NOBITS)
    {
    	printf ("\nSection '%s' n'as pas de données pour faire le hex dump.\n",nom_section);

    }
  	else
    printf ("\nHex dump de la section '%s':\n", nom_section);
	
  	addr = sections_headers->sh_addr;

  	//data = get_data (NULL, pf, sections_headers->sh_offset, 1, octets,("section data"));
  	data = (unsigned char *)mmap(NULL, octets, PROT_READ | PROT_WRITE | PROT_EXEC,MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	//char string[3];
	//int i=0;
	elf_hexa_convert(octets,addr,data);

	fclose(pf);
}

int main()
{
	Elf32Custom_Ehdr header;
	char filename[15] = "arm_simulator.o";
	char nom_section[5] = ".text";
	elf_hexa(filename,header,nom_section);

	exit(0);

}