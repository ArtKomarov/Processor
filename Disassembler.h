/*
 * Disassembler.h
 *
 *  Created on: Nov 23, 2019
 *      Author: artem
 */

#ifndef DISASSEMBLER_H_
#define DISASSEMBLER_H_

#include "ProcessorConsts.h"

typedef struct _CommNumName {
    int num;
    char name[MAX_COMM_NAME];
} CommNumName;

#define CNN_MAKE_PAIR(cnn, CNum, CName, i)\
    if(CNNMakePair(cnn, Cnum, CName, i) == -1)\
        return -1;\

int Disassemb(char* input, char* output);

int CNNMakePair(CommNumName *cnn, int CNum, char* CName, int* i);
int FillCommNumName(CommNumName* cnn);

int DisPrintArg(int command, char** code, FILE* stream);
int DisGetComm(char** code);

#endif /* DISASSEMBLER_H_ */
