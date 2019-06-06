/*
 * Figlio.c
 *
 *  Created on: 5 giu 2019
 *      Author: ????
 */
#include <stdbool.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "QgramDistance.h"

#define TMP_ADD ".tmp"
#define SEGN_FINE "<<_-_TERMINA-_->><<-_-ADESSO_-_>>"
#define NOME_CODA "/codaSyscall"
#define DIMENSIONE_CODA 10

void inserisciInCoda(char *nomeFile)
{
    int size=strlen(nomeFile)+1;
    mqd_t mq=mq_open(NOME_CODA, O_WRONLY);
    mq_send(mq, nomeFile, size, 1);
    mq_close(mq);
}

void scriviSuFile(char nomeFile[], int scrivi[], int dim)
{
    strcat(nomeFile, TMP_ADD);
    FILE *fp=fopen(nomeFile, "w");
    for(int i=0; i< dim; i++)
            putw(scrivi[i], fp);
    fclose(fp);
    inserisciInCoda(nomeFile);
}

void processoFiglio(const int Q_GRAM, char nomeFile[], int indice, const int MAX_STRINGHE_DNA, const int dimQProf)
{
    char stringa[MAX_STRINGHE_DNA];
    int qProfile[dimQProf];

    *(nomeFile+strlen(nomeFile)-1)='\0';
    FILE *fp=fopen(nomeFile, "r");
    fgets(stringa, MAX_STRINGHE_DNA, fp);
    fclose(fp);

    int lunghezza=strlen(stringa);

    qGramProfile(qProfile, dimQProf, stringa, lunghezza, Q_GRAM);
    scriviSuFile(nomeFile, qProfile, dimQProf);
}

/*
 * effettua i fork,
 * setta le pipe,
 * ottiene i pid
 * */
void creaFigli(int pids[], const int NPROC, int pipes[][2]
			, const int Q_GRAM, const int MAX_STRINGHE_CONFIG
			, const int MAX_STRINGHE_DNA, const int dimQProf)
{
    int i=0;
    int sesId=getsid(getpid());
    while(i<NPROC){
        if(pipe(pipes[i])==-1)
                exit(-1);
        int pid=fork();
        if (pid>0)
        {
            pids[i++]=pid;
            close(pipes[i][0]);
        } else if(pid<0)
        {
            printf("\nERRORE NELLA CREAZIONE DEI PROCESSI");
            exit(1);
        } else
        {
            bool continua=true;
            char nomeFile[MAX_STRINGHE_CONFIG+sizeof(TMP_ADD)];
            close(pipes[i][1]);
            setpgid(getpid(), sesId);
            while(continua){
                read(pipes[i][0], nomeFile, MAX_STRINGHE_CONFIG);
                if(strcmp(nomeFile, SEGN_FINE)!=0)
                    processoFiglio(Q_GRAM, nomeFile, i, MAX_STRINGHE_DNA, dimQProf);
                else
                    continua=false;
            }
            exit(2);
        }

    }
}

int leggiCoda(const int inizio, const int NUMCODE, const int MAX_LUNG_FILE, char fileRisultati[][MAX_LUNG_FILE])
{
    int numScritti=0;
    mqd_t mq=mq_open(NOME_CODA, O_RDONLY);
    for(int i=inizio; i<NUMCODE; i++)
    {
        mq_receive(mq, fileRisultati[i], MAX_LUNG_FILE, 0);
        numScritti++;
    }
    mq_close(mq);
    return numScritti;
}

void setUpCoda(int maxDimMsg)
{
    struct mq_attr attr;
    attr.mq_maxmsg=DIMENSIONE_CODA;
    attr.mq_msgsize=maxDimMsg;

    mq_unlink(NOME_CODA);
    mqd_t mq=mq_open(NOME_CODA, O_CREAT, S_IWUSR|S_IRUSR, &attr);
    mq_close(mq);
}

void eseguiSuCoda(const int NUM_RIGHE, const int NPROC, const int MAX_STRINGHE_CONFIG
		, const int pipes[][2], const int maxDimMsg, char nomiFile[][MAX_STRINGHE_CONFIG]
		, char nomiRisultati[][maxDimMsg])
{
    int indiceCode=0;
    int ultimoIndiceScritto=0;
    for(int i=0; i<NUM_RIGHE; i++)
    {
        int j=i%NPROC;
        write(pipes[j][1], nomiFile[i], MAX_STRINGHE_CONFIG);
        indiceCode++;
        if(indiceCode==DIMENSIONE_CODA)
        {
            int tmp;
            tmp=leggiCoda(ultimoIndiceScritto, i+1, maxDimMsg, nomiRisultati);
            ultimoIndiceScritto+=tmp;
            indiceCode=DIMENSIONE_CODA-tmp;
        }
    }
    leggiCoda(ultimoIndiceScritto, NUM_RIGHE, maxDimMsg, nomiRisultati);
}

void terminaProcessi(const int NPROC, int pipes[NPROC][2],
		const int MAX_STRINGHE_CONFIG, int pidProcessi[NPROC])
{
    for (int i = 0; i < NPROC; i++)
    {
        int stato = 0;
        write(pipes[i][1], SEGN_FINE, MAX_STRINGHE_CONFIG);
        waitpid(pidProcessi[i], &stato, NULL);
        mq_unlink(NOME_CODA);
    }
}

int maxDimMsg(const int MAX_STRINGHE_CONFIG)
{
	return MAX_STRINGHE_CONFIG+sizeof(TMP_ADD);
}

void gestisciFigli(const int NPROC, const int Q_GRAM, const int dimQProf, const int NUM_RIGHE
		, const int MAX_STRINGHE_CONFIG, char nomiFile[][MAX_STRINGHE_CONFIG]
		, const int MAX_STRINGHE_DNA, const int maxDimMsg, char nomiRisultati[][maxDimMsg])
{
    int pidProcessi[NPROC];
    int pipes[NPROC][2];
    setUpCoda(maxDimMsg);

    creaFigli(pidProcessi, NPROC, pipes, Q_GRAM, MAX_STRINGHE_CONFIG, MAX_STRINGHE_DNA, dimQProf);
    eseguiSuCoda(NUM_RIGHE, NPROC, MAX_STRINGHE_CONFIG, pipes, maxDimMsg, nomiFile, nomiRisultati);
    terminaProcessi(NPROC, pipes, MAX_STRINGHE_CONFIG, pidProcessi);
}
