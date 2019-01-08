#include "Partie1.h" 
#include <string.h>

int octets=0;
int ExElements=0;
//On compte le nombre de séctions à supprimer (les séctions avec des types REL)

Elf32_Ehdr NbSectionASup(Elf32_Shdr* TableHsCopie,Elf32_Ehdr dataCopie){
	
	int cpt=0;
	//printf("dataCopie->e_shnum =%x\n", dataCopie.e_shnum );
	for (int i = 0; i<dataCopie.e_shnum ; ++i)
	{
		//printf("i=%i\n",i );
		//printf("%x\n",TableHsCopie[i].sh_type );
		if(TableHsCopie[i].sh_type == 9){
			cpt++;
		}
	}
	dataCopie.e_shnum -= cpt;
	return	dataCopie;
}

// On supprime les sections de type REL et on compte le nombre d'octets des ces sections
// On envoie une copie de table des sections en argument
//recupere le nouveau index du type e_shstrndx

void EditTabSection(Elf32_Shdr** TableHsCopie,Elf32_Shdr* TableHs,Elf32_Ehdr dataCopie){


	for (int i = 0; i<dataCopie.e_shnum ; ++i)
	{
		if(TableHs[i].sh_type == 9){
			ExElements++;
			octets+=(TableHs)[i].sh_size;
			for(int j=i;j<dataCopie.e_shnum;++j)
			{
				
				if((TableHs[j+1].sh_offset < TableHs[i].sh_offset)){
					
					printf("========\n");
					(*TableHsCopie)[j+1].sh_offset=(TableHs)[j+1].sh_offset-TableHs[j].sh_size;
				}
				(*TableHsCopie)[j] = (*TableHsCopie)[j+1];
				
			}
		}
	}

	
	
}

void writeHeader(FILE *SortieElf, Elf32_Shdr* TableHsCopie,Elf32_Ehdr dataCopie){
	printf("dataCopie.e_shentsize=%d\n",dataCopie.e_shentsize );
	//ecriture du Header 
	fwrite(&dataCopie,1,sizeof(dataCopie),SortieElf);
	//ecriture de l'en-têtes
	fwrite(TableHsCopie,1,sizeof(TableHsCopie)*dataCopie.e_shnum-1,SortieElf);
}

void SupprimerSection(Elf32_Shdr* TableHs,Elf32_Ehdr data,FILE *fichier){
	
	

	int j;
	void* str=NULL;
	Elf32_Shdr* TableHsCopie;
	Elf32_Ehdr dataCopie;	
	FILE* resultat=fopen("SortieElf","w+");
	TableHsCopie=malloc(sizeof(char));
	
	dataCopie=data;
	memcpy((void*)&TableHsCopie,(void*)&TableHs,sizeof(TableHs));
	dataCopie = NbSectionASup(TableHsCopie,dataCopie);
	EditTabSection(&TableHsCopie,TableHs,data);
	//dataCopie.e_ehsize -= octets;
	dataCopie.e_shstrndx -=ExElements;
	//dataCopie.e_shentsize -=octets;
	fwrite(&data,1,sizeof(data),resultat);
	writeHeader(resultat,TableHsCopie,dataCopie);
	for(int i=0;i<dataCopie.e_shnum;i++){
		j=i;
		while(TableHsCopie[i].sh_name!=TableHs[j].sh_name){
			j++;
		}
		//ecriture de la section
		str = malloc(TableHs[j].sh_size*sizeof(char));
		fseek(fichier,TableHs[j].sh_offset,SEEK_SET);
		fread(str,sizeof(char),sizeof(char)*TableHs[j].sh_size,fichier);
		fwrite(str,sizeof(char),sizeof(char)*TableHs[j].sh_size,resultat);	
	}
	afficherHeader(TableHsCopie,&dataCopie,resultat);
	//afficherSection(resultat,TableHsCopie[10]);
	fclose(resultat);
}



	
