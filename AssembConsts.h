/*
 * AssembConsts.h
 *
 *  Created on: Nov 18, 2019
 *      Author: artem
 */

#ifndef ASSEMBCONSTS_H_
#define ASSEMBCONSTS_H_

#include "ProcessorConsts.h"

#define LABEL_NAME_SIZE 50
//#define MAX_COMM 100
//#define MAX_COMM_NAME 10

typedef struct _label {
    //char* name; //start:
    char name[LABEL_NAME_SIZE];
    int   num;
} label;

typedef struct _CommArray {
    char com[MAX_COMM][MAX_COMM_NAME];
    int  comnum[MAX_COMM];
} CommArray;

extern const int LAST8;
extern const int FIRST8;
extern const char COMMENT_SYMB;
extern const char LABEL_SYMB;

#endif /* ASSEMBCONSTS_H_ */
