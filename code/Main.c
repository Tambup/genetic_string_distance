/*
 * Main.c
 *
 *  Created on: 30 mag 2019
 *      Author: ????
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define TABULATURA "\n\n------------------------------\n\n"
#define MAX_STRINGHE_CONFIG 75
#define MAX_NUMERI_CONFIG 10

void stringheSaluto()
{
	printf("PROGRAMMA PER IL CALCOLO DELLE DISTANZE TRA STRINGHE  DI DNA\n");
	printf("ALGORITMO UTILIZZATO: Q-GRAM DISTANCE\n");
	printf("UTILIZZO DEI CARATTERI SEGUENTI: A C T G\n");
	printf("VERRANNO LETTI SOLO PATH RELATIVI A PARTIRE DALLA POSIZIONE DELL'ESEGUIBILE\n");
	printf("IL FILE DI CONFIGURAZIONE DEVE TERMINARE CON UN CARATTERE NEWLINE\n");
	printf("Q_GRAM DEVE ESSERE UN DIVISORE DELLA LUNGHEZZA DELLE STRINGHE\n");
}

bool avvio(char *confPath)
{
    printf(TABULATURA);
    printf("Inserire il path della cartella contenente i file: ");
    scanf("%s", confPath);
    return presenzaConfig(confPath);
}

int main()
{
	char continua;
	bool presente;
    char confPath[MAX_STRINGHE_CONFIG+5];

	stringheSaluto();
	presente=avvio(confPath);

	if(presente)
	{
		printf("INDIVIDUATO IL FILE DI CONFIGURAZIONE\n");
		printf("Inizio lettura...\n");
		routine(confPath, MAX_STRINGHE_CONFIG, MAX_NUMERI_CONFIG);
	} else
		printf("FILE DI CONFIGURAZIONE NON PRESENTE\n");

	return 0;
}
