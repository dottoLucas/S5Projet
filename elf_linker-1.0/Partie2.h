
void NbSectionASup(Elf32_Shdr** TableHsCopie,Elf32_Ehdr* dataCopie);
void EditTabSection(Elf32_Shdr** TableHsCopie,Elf32_Ehdr* dataCopie);
void writeHeader(FILE *SortieElf ,Elf32_Shdr** TableHsCopie,Elf32_Ehdr* dataCopie);
void SupprimerSection(Elf32_Shdr* TableHs,Elf32_Ehdr data,FILE *fichier);