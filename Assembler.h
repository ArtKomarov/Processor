/*
 * Assembler.h
 *
 *  Created on: Nov 16, 2019
 *      Author: artem
 */

#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include "AssembConsts.h"
#include "ProcessorConsts.h"

int   FileCheck    (FILE* f, char* name);
int   AssembFailCom();
int   AssembFailArg(char* comm, int scounter);
int   GetComm      (char** code, CommArray CommandArray, char** buffcom); //, label* l);
int   GetReg       (char** s, char* reg);
int   GetArg       (char** s, union Type_a4byte* arg);
int   SkipSpace    (char** s);
int   SkipNonSpaces(char** s);
int   StrFree      (char** code);
int   NumLines     (char* s);
int   Assembler    (char* instr, char *outstr);

#endif /* ASSEMBLER_H_ */
