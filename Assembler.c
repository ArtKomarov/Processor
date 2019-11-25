/*
 * Assembler.c
 *
 *  Created on: Nov 16, 2019
 *      Author: artem
 */
#include "Stack/Stack.h"
#include "Stack/StackConsts.h"
#include "Onegin/OneginFunc.h"
#include "Onegin/OneginConsts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ProcessorConsts.h"
#include "AssembConsts.h"
#include "Assembler.h"


int Assembler(char* instr, char *outstr) {
    assert(instr);
    assert(outstr);
    FILE *out;
    if((out = fopen(outstr, "wb")) == NULL) {
        perror("fopen");
        return -1;
    }

    CommArray ComArr = {
            {"out", "pop", "push", "add", "sub", "mul", "div", "sin", "cos", "sqrt", "jmp", "je", "jne", "ja", "jae", "jb", "jbe", "prnt", "cmp", "scan"},
            {OUT,    POP,   PUSH,   ADD,   SUB,   MUL,   DIV,   SIN,   COS,   SQRT,   JMP,   JE,   JNE,   JA,   JAE,   JB,   JBE,   PRNT,   CMP, SCAN, -1}
    };
    char* PRIVIOUS_CODE = CreateBuffer(instr);
    char* code = PRIVIOUS_CODE;
    //int bufflen = strchr(code, '\0') - code;
    char* buffcom;
    int numlen = NumLines(code);

    char wrstr[numlen * 5];
    int  wrlen = 0;

    label l[numlen]; //array of labels {name, code position}
    int labelsize = 0;

    //FIRST PASS
    char* firstpass;
    char firreg;
    int scounter = 0;
    for(firstpass = code; firstpass != NULL; firstpass = strchr(firstpass, '\n')) {
        firstpass++;
        SkipSpace(&firstpass);
        int comm = GetComm(&firstpass, ComArr, &buffcom); //, l);
        if(comm != ISLABEL) {
            if(comm == FREELINE) continue;
            scounter++;
            switch(comm) {
            //case POP2:
            //case PUSH2:
            //scounter++;
            break;
            case POP:
                if(GetReg(&firstpass, &firreg)) {
                    scounter++;
                    break;
                }
                break;
            case PUSH:
                if(GetReg(&firstpass, &firreg)) {
                    scounter++;
                    break;
                }
            case JMP:
            case JE:
            case JNE:
            case JA:
            case JAE:
            case JB :
            scounter += UNION_CHAR_CAP;
            break;
            }
        }
        else {
            char* sup = firstpass;
            SkipNonSpaces(&sup);
            if(sup - firstpass < LABEL_NAME_SIZE) {
                l[labelsize].num = scounter;
                strncpy(l[labelsize].name, firstpass, sup - firstpass - 1); //Do not copy ':'
                l[labelsize].name[sup - firstpass - 1] = '\0';
                labelsize++;
            }
            else {
                fclose(out);
                free(PRIVIOUS_CODE);
                return AssembFailCom();
            }
        }
    }
    l[labelsize].num = -1;
    scounter = 0;

    //SECOND PASS
    //char command = -1;
    int CheckArg = 0;
    union Type_a4byte arg;
    arg.f = 0;
    char arg1;

    //printf("bufflen/3 = %d\n", bufflen);
    for(; code != NULL; code = strchr(code, '\n')) { //&& *(code+1) != '\0'
        code++;
        CheckArg = 0;
        char command = GetComm(&code, ComArr, &buffcom); //, l);         //must be integer
        //printf("comm = %d\n", command);
        scounter++;
        if(command == FREELINE || command == ISLABEL) continue;
        if(command == UNKNOWN) {
            fclose(out);
            free(PRIVIOUS_CODE);
            return AssembFailCom();
        }
        if(command != 0)
            wrstr[wrlen++] = command;
        //code += len;
        switch(command) {
        case OUT:
            if(! StrFree(&code)) {
                fclose(out);
                free(PRIVIOUS_CODE);
                return AssembFailArg(buffcom, scounter);
            }
            break;
        case POP:
            //SkipSpaces(code);
            if(StrFree(&code)) {
                break;
            }
            if(GetReg(&code, &arg1)) {
                wrstr[wrlen-1] = POP2;
                wrstr[wrlen++] = arg1;
            }
            else
                CheckArg = GetArg(&code, &arg);
            if(! StrFree(&code)) {
                fclose(out);
                free(PRIVIOUS_CODE);
                return AssembFailArg(buffcom, scounter);
            }
            break;
        case PUSH:
            /*if(StrFree(&code)) {
                fclose(out);
                free(PRIVIOUS_CODE);
                return AssembFailArg(buffcom);
            }*/
            if(GetReg(&code, &arg1) == 1) {
                wrstr[wrlen-1] = PUSH2;
                wrstr[wrlen++] = arg1;
            }
            else
                CheckArg = GetArg(&code, &arg);
            if(! StrFree(&code)) {
                fclose(out);
                free(PRIVIOUS_CODE);
                return AssembFailArg(buffcom, scounter);
            }
            break;
        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case SIN:
        case COS:
        case SQRT:
        case PRNT:
        case CMP:
        case SCAN:
            if(! StrFree(&code)) {
                fclose(out);
                free(PRIVIOUS_CODE);
                return AssembFailArg(buffcom, scounter);
            }
            break;
        case JMP:
        case JE:
        case JNE:
        case JA:
        case JAE:
        case JB:
        case JBE:
        {
            if(StrFree(&code)) {
                fclose(out);
                free(PRIVIOUS_CODE);
                return AssembFailArg(buffcom, scounter);
            }
            int i;
            for(i = 0; l[i].num != -1; i++)
                if(strncmp(l[i].name, code, strlen(l[i].name)) == 0) {
                    arg.f = l[i].num;
                    CheckArg = 1;
                    break;
                }
            if(! CheckArg) {
                fclose(out);
                free(PRIVIOUS_CODE);
                return AssembFailArg(buffcom, scounter); //unknown label
            }
            break;
        }
        default:
            printf("No definition for command %s!\n", buffcom);
            fclose(out);
            free(PRIVIOUS_CODE);
            return -3;
        }
        if(CheckArg) {
            int i;
            for(i = 0; i < UNION_CHAR_CAP; i++)
            wrstr[wrlen++] = arg.s[i];
        }
        //puts(wrstr);

    }

    int i;

    for(i = 0; i < wrlen; i++)
       wrstr[i] += '0';
    wrstr[wrlen] = '\0';
    //printf("wrstr|\n");
    //printf("wrstr = %s\n",wrstr);
    fwrite(wrstr, sizeof(char), wrlen, out);
    free(PRIVIOUS_CODE); // It was pointer == code (before code become NULL in for)
    fclose(out);
    return 0;
}

int FileCheck(FILE* f, char* name) {
    if(f == NULL) {
        fprintf(stderr, "Unable to open file: %s!\n", name);
        return -1;
    }
    return 0;
}

int SkipSpace(char** s) {
    assert(s != NULL);
    while(**s == ' ') (*s)++;
    return 0;
}

int GetComm(char** code, CommArray CommandArray, char** buffcom) {// ,label* l) {
    SkipSpace(code);
    if(**code == '\n' || **code == '\0' || **code == COMMENT_SYMB) return FREELINE;

    char* lastsym = *code;
    SkipNonSpaces(&lastsym);
    if(lastsym == *code) return FREELINE; //May be deleted in future
    if(*(*code + (lastsym - *code - 1)) == LABEL_SYMB)
        return ISLABEL;

    int i;
    for(i = 0; CommandArray.comnum[i] != -1; i++) {
        if(strncmp(*code, CommandArray.com[i], strlen(CommandArray.com[i])) == 0) {
            SkipNonSpaces(code);
            if(buffcom) *buffcom = CommandArray.com[i];
            return CommandArray.comnum[i];
        }
    }
    return UNKNOWN;
}

int SkipNonSpaces(char** s) {
    assert(s != NULL);
    while(**s != ' ' && **s != '\n' && **s != '\0') (*s)++;
    return 0;
}

int AssembFailCom() {
    fprintf(stderr, "Unknown kommand!\n");
    return -1;
}

int AssembFailArg(char* comm, int scounter) {
    fprintf(stderr, "%s in %d : Invalid argument!\n", comm, scounter);
    return -2;
}

int GetReg(char** s, char *reg) {
    SkipSpace(s);
    if(strncmp(*s, "reg", 3) == 0) {
        int i = 0;
        while(i < 4)
            if((*s)[i++] == '\0') return 0;
        if(((*s)[i-1] - '0' < PROC_NUM_OF_REGS) && ((*s)[i-1] - '0' >= 0)) {
            *reg = atoi ((*s) + i - 1);
            SkipNonSpaces(s);
            return 1;
        }

    }
    return 0;
}

int GetArg(char** s, union Type_a4byte *arg) { // *s = "     54354   \n ghjgkjkg fg"
    SkipSpace(s);
    int i;
    int flag = 1;
    for(i = 0; (*s)[i] != ' ' && (*s)[i] != '\n' && (*s)[i] != '\0'; i++) {
        if((*s)[i] < '0' || (*s)[i] > '9') {
            if(flag && ((*s)[i] == ',' || (*s)[i] == '.')) {
                flag = 0;
                continue;
            }
            else
                return 0;
        }
    }
    (*arg).f = atof(*s);
    SkipNonSpaces(s);
    return 1;
}

int StrFree(char** code) { //return 1 if string contain only spaces and maybe \n
    SkipSpace(code);
    if(**code != '\0' && **code != '\n') return 0;
    return 1;
}

int NumLines(char* s) {
    assert(s != NULL);
    int i = 0;
    for(; s != NULL; s = strchr(s + 1, '\n')) i++;
    return i;
}

char GetFirst(char* s) {
    assert(s != NULL);
    while(*s == ' ') s++;
    return *s;
}
char GetLast(char* s) {
    assert(s != NULL);
    char* sup = strchr(s, '\n') - 1;
    if(sup == NULL) sup = strchr(s, '\0') - 1;
    if(sup == NULL) return '\0';
    while(s < sup && *sup == ' ') sup--;
    return *s;
}
/*
SkipSpace(code);
        if(sscanf(code, "%s", buffcom) == EOF) return 0;
        code += strlen(buffcom);
        if(strcmp(buffcom, "out")  == 0) command = OUT;
        if(strcmp(buffcom, "pop")  == 0) command = POP;
        if(strcmp(buffcom, "push") == 0) command = PUSH;
        if(strcmp(buffcom, "add")  == 0) command = ADD;
        if(strcmp(buffcom, "sub")  == 0) command = SUB;
        if(strcmp(buffcom, "mul")  == 0) command = MUL;
        if(strcmp(buffcom, "div")  == 0) command = DIV;
        if(strcmp(buffcom, "sin")  == 0) command = SIN;
        if(strcmp(buffcom, "cos")  == 0) command = COS;
        if(strcmp(buffcom, "sqrt") == 0) command = SQRT;
        if(strcmp(buffcom, "jmp")  == 0) command = JMP;
        if(strcmp(buffcom, "je")   == 0) command = JE;
        if(strcmp(buffcom, "jne")  == 0) command = JNE;
        if(strcmp(buffcom, "ja")   == 0) command = JA;
        if(strcmp(buffcom, "jae")  == 0) command = JAE;
        if(strcmp(buffcom, "jb")   == 0) command = JB;
        if(strcmp(buffcom, "jbe")  == 0) command = JBE;
        puts(buffcom);
        printf("comm = %d\n", command);
        if(command == -1) return AssembFailCom();
        if(command != 0)
            wrstr[wrlen++] = command;
*/
