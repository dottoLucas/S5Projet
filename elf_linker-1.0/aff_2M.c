#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <elf.h>
#include<sys/stat.h>

int main(int argc, char const *argv[]) {
    FILE *monFichier;
    Elf32_Ehdr headerElf;
    monFichier = fopen(argv[1],"r");
    fread(&headerElf,1,sizeof(headerElf), monFichier);
    if(headerElf.e_ident[0]!=0x7f||headerElf.e_ident[1]!=0x45||headerElf.e_ident[1]!=0x4c||headerElf.e_ident[1]!=0x46){
        // verification de EI_CLASS
        if (headerElf.e_ident[EI_CLASS] != ELFCLASS64){
            return (-1);
        }
        //verification de EI_DATA
        if (headerElf.e_ident[EI_DATA]==ELFDATANONE){
            printf("invalid data encoding\n");
        }else if (headerElf.e_ident[EI_DATA]==ELFDATA2LSB){
            printf("big endian\n");
        }else if (headerElf.e_ident[EI_DATA]==ELFDATA2MSB){
            printf("little endian\n");
        }
        int posPremierElem = 0;
        if(headerElf.e_phoff!=0){
            Elf32_Phdr programHeader;
            fseek(monFichier,headerElf.e_phoff,SEEK_SET);
            fread(&programHeader,1, sizeof(programHeader), monFichier);
            posPremierElem = programHeader.p_offset;
        }
        if (headerElf.e_shoff!=0) {
            Elf32_Shdr sectionHeader;
            fseek(monFichier,headerElf.e_shoff,SEEK_SET);
            fread(&sectionHeader,1, sizeof(sectionHeader), monFichier);
            posPremierElem = sectionHeader.sh_offset;
        }
        unsigned int maPos = posPremierElem;
        fseek(monFichier,maPos,SEEK_SET);
        while(feof(monFichier)!=1/*&&(if(sectionHeader.sh_offset!=0){sh_offset>maPos}else{1})*/){

        }

    }
    fclose(monFichier);
    return 0;
}
