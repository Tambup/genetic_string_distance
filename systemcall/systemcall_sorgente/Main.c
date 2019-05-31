/*
 * Main.c
 *
 *  Created on: 30 mag 2019
 *      Author: ????
 */

#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<unistd.h>
#include<stdbool.h>
#define TABULATURA "\n\n----------\n\n"
#define NOME_FILE_CONF "config.txt"

void routine()
{

}

bool presenzaConfig(const char *path)
{
	DIR *cartella;
	struct dirent *file;

	cartella=opendir(path);
	if(cartella==NULL)
	{
		return false;
	} else{
		while((file=readdir(cartella)) != NULL)
		{
			if(strcmp(file->d_name, NOME_FILE_CONF)==0){
				closedir(cartella);
				return true;
			}
		}
		closedir(cartella);
		return false;
	}
}

void stringheSaluto()
{
	printf("PROGRAMMA PER IL CALCOLO DELLE DISTANZE TRA STRINGHE");
	printf("\nALGORITMO UTILIZZATO: Q-GRAM DISTANCE");
	printf("\nUTILIZZO DEI CARATTERI SEGUENTI: A C T G");
	printf("\nVERRANNO LETTI SOLO PATH RELATIVI A PARTIRE DALLA POSIZIONE DELL'ESEGUIBILE");

}

int main()
{
	char continua;
	char *confPath;
	bool presente;

	stringheSaluto();
	do{

		printf(TABULATURA);
		printf("Inserire il path della cartella contenente i file: ");
		scanf("%s", confPath);
		presente=presenzaConfig(confPath);

		if(presente)
		{
			printf("\n\nINDIVIDUATO IL FILE DI CONFIGURAZIONE");
			printf("\nInizio lettura...");
			routine();
		} else
			printf("\n\nFILE DI CONFIGURAZIONE NON PRESENTE");

		printf("\n\nSi desidera effettuare un altro calcolo (y/altro)? ");
		scanf(" %c", &continua);
		getchar();
	}while(continua=='y');


	return 0;
}
