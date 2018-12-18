#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <elf.h>
#include<sys/stat.h>
#include"util.h"

int reverse_endianess(int value,int size) {
    int resultat = 0;
    char *source, *destination;
    int i;

    source = (char *) &value;
    destination = ((char *) &resultat) + sizeof(size);
    for (i = 0; i < sizeof(size); i++)
        *(--destination) = *(source++);
    return resultat;
}


int elf_check_header(Elf64_Ehdr * data){
    // verification de EI_MAG
	if (data->e_ident[EI_MAG0] != ELFMAG0 ||
		data->e_ident[EI_MAG1] != ELFMAG1 ||
		data->e_ident[EI_MAG2] != ELFMAG2 ||
		data->e_ident[EI_MAG3] != ELFMAG3){
		return (-1);
	}
  
	// verification de EI_CLASS
	if (data->e_ident[EI_CLASS] == ELFCLASS32){
		return (-1);
	}
	
	//verification de EI_DATA 
	if (data->e_ident[EI_DATA]==ELFDATANONE){
		printf("invalid data encoding\n");
	}else if (data->e_ident[EI_DATA]==ELFDATA2LSB){
		printf("little endian\n");
	}else if (data->e_ident[EI_DATA]==ELFDATA2MSB){
		printf("big endian\n");
	}	

return (0);
}


char* find_type(Elf64_Shdr section_header){
	
	char* type;
	
	switch (section_header.sh_type){
		case SHT_NULL:
			type="NULL";
		break;
			
		case SHT_PROGBITS:
			type="ROGBITS";
		break;
			
		case SHT_SYMTAB:
			type="SYMTAB";
		break;
			
		case SHT_STRTAB:
			type="STRTAB";
		break;
			
		case SHT_RELA:
			type="RELA";
		break;
			
		case SHT_HASH:
			type="HASH";
		break;
			
		case SHT_DYNAMIC:
			type="DYNAMI";
		break;
			
		case SHT_NOTE:
			type="NOTE";
		break;
			
		case SHT_NOBITS:
			type="NOBITS";
		break;
			
		case SHT_REL:
			type="REL";
		break;
			
		case SHT_SHLIB:
			type="SHLIB";
		break;
			
		case SHT_DYNSYM:
			type="DYNSYM";
		break;
			
		case SHT_LOPROC:
			type="LOPROC";
		break;
			
		case SHT_HIPROC:
			type="HIPROC";
		break;
			
		case SHT_LOUSER:
			type="LOUSER";
		break;
			
		case SHT_HIUSER:
			type="HIUSER";
		break;
	default:;
	}
	
	return type;
}



void elf_print_header(Elf64_Ehdr *data){

	// recuperation du header de section
	Elf64_Shdr *section=(Elf64_Shdr*)((void*)data + data->e_shoff);
	Elf64_Shdr nom_s =  section[data->e_shstrndx];
	char* type;
	char* head = (char*)((void*)data+nom_s.sh_offset);
	
	Elf64_Shdr section_header;
	printf("[Nr]\t%-15s\t\t%-15s\t\t%-15s\t\t%-15s\n\t%-15s\t\t%-15s\t\t%s  %s  %s   \t%-15s\n","Nom","Type","Adresse","Decalage","Taille","TaillEntre","Fanion","Lien","Info","Alignement");

	for(int i=0;i<data->e_shnum;i++){
		section_header = section[i];
		type=find_type(section_header);
		
		
		printf("%llu",section[i].sh_offset);
		
		//if(i==11 || i==13 || i==18 || i==20 || i==22 )
			//printf("[%2d]\t%-15s\t\t%-15s\t\t%-15u\t\t%-15x\n\t%-15x\t\t%-15x\t\t%x  %x  %x   \t%-15u\n",i,head+section_header.sh_name,type,head+section_header.sh_addr,head+section_header.sh_offset,head+section_header.sh_size,head+section_header.sh_entsize,head+section_header.sh_flags,head+section_header.sh_link,head+section_header.sh_info,head+section_header.sh_addralign);

			//printf("[%d]\t%-15s\t%-15s\n",i,head+section_header.sh_name,type);
		//else
			
			//printf("[%d]\t%-15s\t\t%-15s\n",i,head+section_header.sh_name,type);

	
	}
	
	

}



int main(int argc, char *argv[]){
	int fd;
	void *data;
	struct stat file_infos;

	if (argc == 2){
		if ((fd = open(argv[1], O_RDONLY)) > 0){
			if (fstat(fd, &file_infos))
				fprintf(stderr, "Recuperation des informations du fichier impossible\n");
			else if ((data = mmap(0, file_infos.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
						fprintf(stderr, "Chargement du fichier en memoire impossible\n");
			else{
				if (!elf_check_header((Elf64_Ehdr*)data)){
					elf_print_header((Elf64_Ehdr*)data);
				}else
					munmap(data, file_infos.st_size);
				}
				close(fd);
		}else
			fprintf(stderr, "Ouverture du fichier impossible\n");
	}else
		fprintf(stderr, "USAGE: %s <binary_file>\n", argv[0]);
  return (0);
}
