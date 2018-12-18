#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <elf.h>
#include<sys/stat.h>

int elf_check_header(Elf32_Ehdr * data){
    // verification de EI_MAG
	if (data->e_ident[EI_MAG0] != ELFMAG0 ||
		data->e_ident[EI_MAG1] != ELFMAG1 ||
		data->e_ident[EI_MAG2] != ELFMAG2 ||
		data->e_ident[EI_MAG3] != ELFMAG3){
		return (-1);
	}
  
	// verification de EI_CLASS
	if (data->e_ident[EI_CLASS] != ELFCLASS64){
		return (-1);
	}
	
	//verification de EI_DATA 
	if (data->e_ident[EI_DATA]==ELFDATANONE){
		printf("invalid data encoding\n");
	}else if (data->e_ident[EI_DATA]==ELFDATA2LSB){
		printf("big endian\n");
	}else if (data->e_ident[EI_DATA]==ELFDATA2MSB){
		printf("little endian\n");
	}	

return (0);
}


void elf_print_header(Elf32_Ehdr *data){

	

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
						if (!elf_check_header((Elf32_Ehdr*)data)){
							elf_print_header((Elf32_Ehdr*)data);
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
