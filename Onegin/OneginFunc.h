/*
 * OneginFunc.h
 *
 *  Created on: Oct 14, 2019
 *      Author: artem
 */

#ifndef ONEGINFUNC_H_
#define ONEGINFUNC_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

typedef long long int ll;

typedef struct line{
    char* str;
    int length;
} line;

ll     CountLines   (line* lines);                 //Get number of lines by search for last (NULL) line
char*  CreateBuffer (char* filename);              //Read from file "filename" and create char* with file contents
line*  ParseBuffer  (char* buffer, ll* numline);   //Fill structs line* from the buffer, also get number of structs
int    IsLine       (line line);                   //Check function. Don't accept lines without any LETTERS.
int    ParseRhyme   (line* l);                     // Parse the Onegin's rhyme, set lines with same endings nearby
ll     GetOnlyText  (line *l, ll length);          // Set in first part - text, into second - other lines and get number of last line
int PrintRaveToFile (line *l, ll length, FILE* wf); //Parse by function ParseRhyme() and print to file in random order
int    Compare      (line s1, line s2);            //Compare two structs (< = >) == (-1 0 1)
int    CompareEnd   (line s1, line s2);            //Compare two structs by their endings (< = >) == (-1 0 1)
int    quicksort    (line* l, ll first, ll last, int order); //Quick sort array of structs
int    DelSymbols   (char* from, char* sym);       //Support function, delete symbols (sym) from string (from) (not used but good:D)

#endif /* ONEGINFUNC_H_ */
