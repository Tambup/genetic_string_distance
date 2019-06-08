/*
 * Q_gramDistance.c
 *
 *  Created on: 3 giu 2019
 *      Author: tambu
 */

#include <string.h>
#include <stdlib.h>

long numeroMatch(long val)
{
    return (val==0 || val==1) ? 1 : val+numeroMatch(val-1);
}

int qGramDistance(int prima[], int seconda[], int lunghezza)
{
    int distanza=0;
    for(int i=0; i<lunghezza; i++)
        distanza+=abs(prima[i]-seconda[i]);
    return distanza;
}

/*
 * Date 4 sequenze
 * sono ordinati nel seguente modo:
 * qgd(1,2) ; qgd(1,3) ;  qgd(1,4) ; qgd(2,3) ; qgd(2,4) ;  qgd(3,4).
 * */
void tuttiQGramDistance(int DIM_Q_PROF, int risultato[], int qGramProfile[][DIM_Q_PROF], int NSEQ)
{
    int index=0;
    for(int i=0; i<NSEQ; i++)
        for(int j=i; j<NSEQ-1; j++)
            risultato[index++]=qGramDistance(qGramProfile[i], qGramProfile[j+1], DIM_Q_PROF);
}

long dimQGramProfile(const int qGram)
{
    if(qGram==0)
        return 1;

    long val=4;
    if(qGram>=1)
        for(int i=1; i<qGram; i++)
            val*=4;
    return val;
}

int hash(char stringa[], const int lunghezza)
{
    long val=0;
    for(int i=0; i<lunghezza; i++)
    {
        int j=lunghezza-i-1;
        switch(stringa[i])
        {
            case 'A': val+=0*dimQGramProfile(j);
                break;
            case 'C': val+=1*dimQGramProfile(j);
                break;
            case 'G': val+=2*dimQGramProfile(j);
                break;
            case 'T': val+=3*dimQGramProfile(j);
                break;
        }
    }
    return val;
}

void qGramProfile(int result[], int dim, char stringa[], const int lunghezzaStr, const int qGram)
{
    for(int i=0; i<dim; i++)
        result[i]=0;
    int numIterazioni=lunghezzaStr-qGram+1;
    char sub[qGram];
    for(int i=0; i<numIterazioni; i++)
    {
        memcpy(sub, &stringa[i], qGram);
        int val=hash(sub, qGram);
        result[val]++;
    }
}
