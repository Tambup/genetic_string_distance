/*
 * Main.c
 *
 *  Created on: 30 mag 2019
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

#include "ParseFile.h"
#include "QgramDistance.h"
#include "Figlio.h"

#define TABULATURA "\n------------------------------\n\n"
#define NOME_FILE_CONF "config.txt"
#define MAX_NUMERI_CONFIG 10
#define MAX_STRINGHE_CONFIG 75
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

void cancellaTemporanei(int NSEQ, int dimRiga, char *file)
{
    for(int i=0; i<NSEQ; i++)
        remove(file + dimRiga*i);
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
void routine(const char *confPath)
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
        long dimRisultati=numeroMatch(NSEQ-1);
        int risultati[dimRisultati];

        parseString(fp, NSEQ, MAX_STRINGHE_CONFIG, nomiFile);
        fclose(fp);
        const int MAX_STRINGHE_DNA=dimSeq(nomiFile[0]);

        printf("\nNon si assume alcuna responsabilita' inerente il formato del file");
        printf("\nPer i valori numerici: max %d cifre\nPer i nomi dei file: max %d caratteri",
                MAX_NUMERI_CONFIG, MAX_STRINGHE_CONFIG);
        printf("\n\n");
        
        gestisciFigli(NPROC, Q_GRAM, dimQProf, NSEQ
                        , MAX_STRINGHE_CONFIG, nomiFile
                        , MAX_STRINGHE_DNA, dimMsg, fileRisultati);
        leggiRisultato(dimQProf, NSEQ, dimMsg, valori, fileRisultati);
        tuttiQGramDistance(dimQProf, risultati, valori, NSEQ);
        scriviRisultatoFinale(NSEQ, dimMsg, risultati, fileRisultati, confPath);
        cancellaTemporanei(NSEQ, dimMsg, fileRisultati);
    } else
        printf("\nERRORE: IMPOSSIBILE APRIRE IL FILE");
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

void stringheSaluto()
{
    printf("PROGRAMMA PER IL CALCOLO DELLE DISTANZE TRA STRINGHE");
    printf("\nALGORITMO UTILIZZATO: Q-GRAM DISTANCE");
    printf("\nUTILIZZO DEI CARATTERI SEGUENTI: A C T G");
    printf("\nVERRANNO LETTI SOLO PATH RELATIVI A PARTIRE DALLA POSIZIONE DELL'ESEGUIBILE");
    printf("\nIL FILE DI CONFIGURAZIONE DEVE TERMINARE CON UN CARATTERE NEWLINE");
    printf("\nQ_GRAM DEVE ESSERE UN DIVISORE DELLA LUNGHEZZA DELLE STRINGHE");
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
    char *confPath;

    stringheSaluto();
    do{
        presente=avvio(confPath);

        if(presente)
        {
            printf("\n\nINDIVIDUATO IL FILE DI CONFIGURAZIONE");
            printf("\nInizio lettura...");
            routine(confPath);
        } else
            printf("\n\nFILE DI CONFIGURAZIONE NON PRESENTE");

        printf("\n\nSi desidera effettuare un altro calcolo (y/altro)? ");
        scanf(" %c", &continua);
        getchar();
    }while(continua=='y');


    return 0;
}
