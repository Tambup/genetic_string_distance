/*
 * ParseFile.h
 *
 *  Created on: 30 mag 2019
 *      Author: ????
 */

#ifndef PARSECONFIG_H_INCLUDED
#define PARSECONFIG_H_INCLUDED

/*
    Parsing delle righe numeriche(le prime)
*/
int parseInt(FILE*, const int);

/*
    Parsing delle righe contenenti gli indirizzi dei file
*/
void parseString(FILE*, const int, const int, char[]);

#endif // PARSECONFIG_H_INCLUDED
