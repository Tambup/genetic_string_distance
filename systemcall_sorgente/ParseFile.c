/*
 * ParseFile.c
 *
 *  Created on: 30 mag 2019
 *      Author: ????
 */

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

int parseInt(FILE *fp, const int MAX_INT)
{
    char proc[MAX_INT];
    char **end=0;

    fgets(proc, MAX_INT, fp);
    int ritorna=strtoul(proc, end, 10);
    return ritorna;
}

void parseString(FILE* fp, const int NUM_RIGHE
		, const int LUNG_RIGA, char nomiFile[][LUNG_RIGA])
{
    for(int i=0; i<NUM_RIGHE; i++)
    {
        fgets(nomiFile[i], LUNG_RIGA, fp);
    }
}
