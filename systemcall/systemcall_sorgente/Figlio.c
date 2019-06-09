/*
 * Figlio.c
 *
 *  Created on: 9 giu 2019
 *      Author: ????
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <mqueue.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <stdbool.h>

#include "QgramDistance.h"

#define NOME_CODA "/codaSyscall"
#define TMP_ADD ".tmp"

//globali SOLAMENTE perchè così possono essere chiuse
//in caso di necessita' dall'handler. In un programma
//ad oggetti non sarebbe necessario.
FILE *lettura;
FILE *scrittura;
pthread_spinlock_t spinLock;

void chiudiFile()
{
	pthread_spin_lock(&spinLock);
	if(lettura!=NULL)
		close(lettura);
	if(scrittura!=NULL)
		close(scrittura);
	pthread_spin_unlock(&spinLock);
}

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
	pthread_spin_lock(&spinLock);
	scrittura=fopen(nomeFile, "w");
	for(int i=0; i< dim; i++)
		putw(scrivi[i], scrittura);
	fclose(scrittura);
	pthread_spin_unlock(&spinLock);
	inserisciInCoda(nomeFile);
}

void processoFiglio(const int Q_GRAM, char nomeFile[], const int MAX_STRINGHE_DNA, const int dimQProf)
{
	char stringa[MAX_STRINGHE_DNA];
	int qProfile[dimQProf];

	*(nomeFile+strlen(nomeFile)-1)='\0';
	pthread_spin_lock(&spinLock);
	lettura=fopen(nomeFile, "r");
	fgets(stringa, MAX_STRINGHE_DNA, lettura);
	fclose(lettura);
	pthread_spin_unlock(&spinLock);

	int lunghezza=strlen(stringa);

	qGramProfile(qProfile, dimQProf, stringa, lunghezza, Q_GRAM);
	scriviSuFile(nomeFile, qProfile, dimQProf);
}

void handler(int segnale)
{
	switch(segnale)
	{
		case SIGHUP:
			chiudiFile();
			pthread_spin_destroy(&spinLock);
			exit(EXIT_SUCCESS);
			break; //inutile
	}
}

void setupHandler()
{
	struct sigaction sig;
	sig.sa_handler = &handler;
	sig.sa_flags = SA_RESTART;
	sigfillset(&sig.sa_mask);
	if (sigaction(SIGHUP, &sig, NULL) == -1) {
		perror("Error: impossibile gestire SIGHUP");
		exit(EXIT_FAILURE);
	}
}

/*
 * Istanzia le risorse necessarie e si dispone per leggere
 * dalla coda.
 * */
void avvioFiglio(int pipe[], const int Q_GRAM, const int MAX_STRINGHE_DNA
		, const int dimQProf, const int MAX_STRINGHE_CONFIG, const int sesId
		, const int i, const char SEGN_FINE[])
{
	int funziona = pthread_spin_init(&spinLock, PTHREAD_PROCESS_SHARED);
	if(funziona!=0)
		exit(EXIT_FAILURE);
	bool continua=true;
	char nomeFile[MAX_STRINGHE_CONFIG+sizeof(TMP_ADD)];
	close(pipe[1]);
	setpgid(getpid(), sesId);
	setupHandler();
	prctl(PR_SET_PDEATHSIG, SIGHUP);
	while(continua){
		read(pipe[0], nomeFile, MAX_STRINGHE_CONFIG);
		if(strcmp(nomeFile, SEGN_FINE)!=0)
			processoFiglio(Q_GRAM, nomeFile, MAX_STRINGHE_DNA, dimQProf);
		else
			continua=false;
	}
	exit(EXIT_SUCCESS);
}
