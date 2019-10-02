/*
 * Cerca_Elabora_config.c
 *
 *  Created on: 9 giu 2019
 *      Author: ????
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "FiglioFactory.h"
#include "ParseFile.h"
#include "QgramDistance.h"

#define NOME_FILE_CONF "config.txt"
#define NOME_FILE_SALVATAGGIO "result.txt"

void cancellaTmp(char risultato[], char fileRisultati[])
{
	int togli=strlen(TMP_ADD);
	int lunghezza=strlen(fileRisultati)-togli;
	memcpy(risultato, &fileRisultati[0], lunghezza);
	risultato[lunghezza]='\0';
}

void leggiRisultato(const int DIM_Q_PROF, int NSEQ, int maxDim
		, int valori[][DIM_Q_PROF], char fileRisultati[][maxDim])
{
	for(int i=0; i<NSEQ; i++)
	{
		FILE *fp=fopen(fileRisultati[i], "r");
		for(int j=0; j<DIM_Q_PROF; j++)
			valori[i][j]=getw(fp);
		fclose(fp);
	}
}

void formaStringa(int valore, char sinistra[], char destra[], char result[])
{
	char buffer[30];
	sprintf(buffer, "%d\n", valore);
	strcat(result, sinistra);
	strcat(result, ", ");
	strcat(result, destra);
	strcat(result, ": ");
	strcat(result, buffer);
}

void scriviRisultatoFinale(int NSEQ, int maxDim, int valori[]
			, char fileRisultati[][maxDim], char *confPath)
{
	char fileRisultato[strlen(confPath)+strlen(NOME_FILE_SALVATAGGIO)+2];
	strcpy(fileRisultato, confPath);
	strcat(fileRisultato, NOME_FILE_SALVATAGGIO);
	FILE *fp=fopen(fileRisultato, "w");
	int indice=0;
	char temp[NSEQ][maxDim];
	for(int i=0; i<NSEQ; i++)
		cancellaTmp(temp[i], fileRisultati[i]);
	for(int i=0; i<NSEQ; i++)
	{
		int lunghezza=strlen(temp[i])*2+100;//fa da margine
		for(int j=i; j<NSEQ-1; j++)
		{
			char riga[lunghezza];
			riga[0]='\0';
			formaStringa(valori[indice++], temp[i], temp[j+1], riga);
			fputs(riga, fp);
		}
	}
	fclose(fp);
}

void cancellaTemporanei(int NSEQ, int dimRiga, char file[][dimRiga])
{
	for(int i=0; i<NSEQ; i++)
		remove(file[i]);
}

int dimSeq(char *percorso)
{
	char nomeFile[strlen(percorso)];
	strcpy(nomeFile, percorso);
	*(nomeFile+strlen(nomeFile)-1)='\0';
	FILE *file = fopen(nomeFile, "r");
	fseek(file, 0, SEEK_END);
	int conta = ftell(file)+2;
	fclose(file);
	return conta;
}

/*
 * Chiama il file per leggere il file config
 * e chiama la funzione per creare e far funzionare i figli
 * */
void routine(const char confPath[], int MAX_STRINGHE_CONFIG, int MAX_NUMERI_CONFIG)
{
	strcat(confPath, "/");
	char percorsoConfig[strlen(confPath)+strlen(NOME_FILE_CONF)+2];
	strcpy(percorsoConfig, confPath);
    strcat(percorsoConfig, NOME_FILE_CONF);
    FILE *fp=fopen(percorsoConfig, "r");

    if(fp!=NULL)
    {
        //vanno eseguiti in sequenza senza eccezioni
        const int NPROC=parseInt(fp, MAX_NUMERI_CONFIG);
        const int Q_GRAM=parseInt(fp, MAX_NUMERI_CONFIG);
        const int NSEQ=parseInt(fp, MAX_NUMERI_CONFIG);
        char nomiFile[NSEQ][MAX_STRINGHE_CONFIG];
        int dimMsg=maxDimMsg(MAX_STRINGHE_CONFIG);
        char fileRisultati[NSEQ][dimMsg];
        int dimQProf=dimQGramProfile(Q_GRAM);
        int valori[NSEQ][dimQProf];
        int dimRisultati=numeroMatch(NSEQ-1);
        int risultati[dimRisultati];

        parseString(fp, NSEQ, MAX_STRINGHE_CONFIG, nomiFile);
        fclose(fp);
        const int MAX_STRINGHE_DNA=dimSeq(nomiFile[0]);

        printf("Non si assume alcuna responsabilita' inerente il formato del file\n");
        printf("Per i valori numerici: max %d cifre\nPer i nomi dei file: max %d caratteri\n",
                MAX_NUMERI_CONFIG, MAX_STRINGHE_CONFIG);
		fflush(stdout);

		gestisciFigli(NPROC, Q_GRAM, dimQProf, NSEQ
				, MAX_STRINGHE_CONFIG, nomiFile
				, MAX_STRINGHE_DNA, dimMsg, fileRisultati);


		leggiRisultato(dimQProf, NSEQ, dimMsg, valori, fileRisultati);
		tuttiQGramDistance(dimQProf, risultati, valori, NSEQ);
		scriviRisultatoFinale(NSEQ, dimMsg, risultati, fileRisultati, confPath);
		cancellaTemporanei(NSEQ, dimMsg, fileRisultati);
    } else
        printf("ERRORE: IMPOSSIBILE APRIRE IL FILE\n");
}

/*
 * Verifica la presenza del file di configurazione nel path indicato
 * */
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
