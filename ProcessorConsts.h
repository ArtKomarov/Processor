/*
 * ProcessorConsts.h
 *
 *  Created on: Nov 16, 2019
 *      Author: artem
 */

#ifndef PROCESSORCONSTS_H_
#define PROCESSORCONSTS_H_

#include "Stack/Stack.h"

typedef float ram_t;
typedef float reg_t;
typedef float  type_a;

/*#define LABEL_NAME_SIZE 50
#define MAX_COMM 100
#define MAX_COMM_NAME 10*/
#define PROC_RAM_SIZE 8096
#define PROC_NUM_OF_REGS 4
#define UNION_CHAR_CAP ((int)(sizeof(type_a)))

/*#define IFCOM(comm, num)\
        if(strncmp(code, comm, strlen(comm)) == 0) {\
            return num;\
        }*/


typedef struct _Processor {
    reg_t reg[PROC_NUM_OF_REGS];
    reg_t rip;
    reg_t rsp;
    reg_t flag;
    stack stk;
    ram_t ram[PROC_RAM_SIZE];
} Processor;

/*typedef struct _label {
    //char* name; //start:
    char name[LABEL_NAME_SIZE];
    int   num;
} label;

typedef struct _CommArray {
    char com[MAX_COMM][MAX_COMM_NAME];
    int  comnum[MAX_COMM];
} CommArray;*/

union Type_a4byte {
    type_a f;
    char s[UNION_CHAR_CAP];
};

/*extern const int LAST8;
extern const int FIRST8;
extern const char COMMENT_SYMB;
extern const char LABEL_SYMB;*/

enum {
    FREELINE = -1,
    ISLABEL = -2,
    UNKNOWN = -3,

    OUT = 0,
    POP = 1,
    POP2 = 2,
    PUSH = 3,
    PUSH2 = 4,
    ADD = 5,
    SUB = 6,
    MUL = 7,
    DIV = 8,
    SIN = 9,
    COS = 10,
    SQRT = 11,
    JMP = 12,
    JE = 13,
    JNE = 14,
    JA = 15,
    JAE = 16,
    JB = 17,
    JBE = 18

};

enum {
    RAX = 0,
    RBX = 1,
    RCX = 2,
    RDX = 3,
    SP = 13,
    IP = 14
};


#endif /* PROCESSORCONSTS_H_ */
