/*
 * Processor.h
 *
 *  Created on: Nov 18, 2019
 *      Author: artem
 */

#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "ProcessorConsts.h"

#define STACK_START_CAP 30

/*typedef struct _bytecode {
    int current;
    char* code;
} bcode;*/

typedef struct _Processor {
    reg_t reg[PROC_NUM_OF_REGS];
    int rip;
    int rsp;
    int flag;
    stack stk;
    ram_t ram[PROC_RAM_SIZE];
    char* code;
} Processor;

typedef struct _CommFunc {
    int NumComm;
    int (*comm)(Processor* proc);
} CommFunc;


int CPU(char *input);
Processor ProcessorConstruct();
int ProcessorDestruct(Processor* proc);

int FillCommFunc(CommFunc* cf);
int MakePair(CommFunc* cf, int NumComm, int (*comm)(Processor* proc), int* i);
int GetCPUComm(Processor* proc);
int GetCPUArg(Processor* proc, type_a* arg);
int GetCPUReg(Processor* proc, int* num);

int out  (Processor* proc);
int pop  (Processor* proc);
int pop2 (Processor* proc);
int push (Processor* proc);
int push2(Processor* proc);
int add  (Processor* proc);
int sub  (Processor* proc);
int mul  (Processor* proc);
int _div (Processor* proc);
int _sin (Processor* proc);
int _cos (Processor* proc);
int _sqrt(Processor* proc);
int jmp  (Processor* proc);
int je   (Processor* proc);
int jne  (Processor* proc);
int ja   (Processor* proc);
int jae  (Processor* proc);
int jb   (Processor* proc);
int jbe  (Processor* proc);
int prnt (Processor* proc);

#endif /* PROCESSOR_H_ */
