#include <elf.h>
#include <stdio.h>
#include "util.c"



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

Elf32_Rel reverseAllEndianness(Elf32_Rel relStruct){
  if (!is_big_endian()){
    relStruct.r_offset = reverse_endianess(relStruct.r_offset,sizeof(relStruct.r_offset));
    relStruct.r_info = reverse_endianess(relStruct.r_info,sizeof(relStruct.r_info));
  }
  return relStruct;
}
