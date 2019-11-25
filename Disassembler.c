/*
 * Disassembler.c
 *
 *  Created on: Nov 23, 2019
 *      Author: artem
 */
#include <stdio.h>
#include "Onegin/OneginFunc.h"
#include "ProcessorConsts.h"
#include "Disassembler.h"

int Disassemb(char* input, char* output) {
    assert(input  != NULL);
    assert(output != NULL);
    char* code;
    if((code = CreateBuffer(input)) == NULL)
        return -2;
    //int CodeLen = (int)strlen(code);
    FILE* out;
    if((out = fopen(output, "wb")) == NULL) {
        perror("fopen");
        free(code);
        return -2;
    }
    int command;
    CommNumName CommNN[MAX_COMM];
    FillCommNumName(CommNN);
    while(*code != '\0') {
        command = DisGetComm(&code);
        int i;
        for(i = 0; CommNN[i].num != -1; i++) {
            if(CommNN[i].num == command) {
                fputs(CommNN[i].name, out);
                if(DisPrintArg(command, &code, out) == -1) {
                    fprintf(stderr, "Disassembler: Error when writing argument in command %s!\n", CommNN[i].name);
                    free(code);
                    fclose(out);
                    return -1;
                }
                fputs("\n\0", out);
                break;
            }
        }
        if(CommNN[i].num == -1) {
            fclose(out);
            free(code);
            return -1;
        }
    }
    //free(code);
    fclose(out);
    return 0;
}

int FillCommNumName(CommNumName* cnn) {
    int i = 0;
    CNNMakePair(cnn, OUT,   "out",  &i);
    CNNMakePair(cnn, POP,   "pop",  &i);
    CNNMakePair(cnn, POP2,  "pop",  &i);
    CNNMakePair(cnn, PUSH,  "push", &i);
    CNNMakePair(cnn, PUSH2, "push", &i);
    CNNMakePair(cnn, ADD,   "add",  &i);
    CNNMakePair(cnn, SUB,   "sub",  &i);
    CNNMakePair(cnn, DIV,   "div",  &i);
    CNNMakePair(cnn, MUL,   "mul",  &i);
    CNNMakePair(cnn, SIN,   "sin",  &i);
    CNNMakePair(cnn, COS,   "cos",  &i);
    CNNMakePair(cnn, SQRT,  "sqrt", &i);
    CNNMakePair(cnn, JMP,   "jmp",  &i);
    CNNMakePair(cnn, JE,    "je",   &i);
    CNNMakePair(cnn, JNE,   "jne",  &i);
    CNNMakePair(cnn, JA,    "ja",   &i);
    CNNMakePair(cnn, JAE,   "jae",  &i);
    CNNMakePair(cnn, JB,    "jb",   &i);
    CNNMakePair(cnn, JBE,   "jbe",  &i);
    CNNMakePair(cnn, PRNT,  "prnt", &i);
    CNNMakePair(cnn, CMP,   "cmp",  &i);
    CNNMakePair(cnn, SCAN,  "scan", &i);
    return 0;
}

int CNNMakePair(CommNumName *cnn, int CNum, char* CName, int* i) {
    assert(cnn   != NULL);
    assert(CName != NULL);
    assert(i     != NULL);
    if(*i >= MAX_COMM) return -1;
    cnn[*i].num = CNum;
    strcpy(cnn[*i].name, CName);
    (*i)++;
    return 0;
}

int DisGetComm(char** code) {
    if(**code == '\0') return -1;
    else return (int)(*((*code)++) - '0');
}

int DisPrintArg(int command, char** code, FILE* stream) {
    assert(code   != NULL);
    assert(stream != NULL);
    int i;
    switch(command) {
    case POP2:
    case PUSH2:
        if(fputs(" reg", stream) == EOF)
            return -1;
        if(**code == '\0' || fputc(*((*code)++), stream) == EOF)
            return -1;
        break;
    case PUSH:
    case JMP:
    case JE:
    case JNE:
    case JA:
    case JAE:
    case JB:
    case JBE: {
        union Type_a4byte tb;
        for(i = 0; **code != '\0' && i < UNION_CHAR_CAP; i++) {
            tb.s[i] = *((*code)++)-'0';
        }
        if(fprintf(stream, " %.4f", tb.f) == EOF)
            return -1;
        break;
    }
    }
    return 0;
}
