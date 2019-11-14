/*
 * Processor.c
 *
 *  Created on: Oct 28, 2019
 *      Author: Artem
 */
#include "Stack/Stack.h"
#include "Stack/StackConsts.h"
#include "Onegin/OneginFunc.h"
#include "Onegin/OneginConsts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int ARG_POISON = -99999;
const char COMMENT_SYMB = '%';
const char LABEL_SYMB = ':';

#define LABEL_NAME_SIZE 50
#define MAX_COMM 100
#define MAX_COMM_NAME 10
#define PROC_RAM_SIZE 8096
#define PROC_NUM_OF_REGS 4

/*#define IFCOM(comm, num)\
        if(strncmp(code, comm, strlen(comm)) == 0) {\
            return num;\
        }*/


typedef double ram_t;
typedef double reg_t;
typedef double type;


typedef struct Processor {
    reg_t reg[PROC_NUM_OF_REGS];
    reg_t rip;
    reg_t rsp;
    reg_t flag;
    stack stk;
    ram_t ram[PROC_RAM_SIZE];
} Processor;

typedef struct _label {
    //char* name; //start:
    char name[LABEL_NAME_SIZE];
    int   num;
} label;

typedef struct CommArray {
    char com[MAX_COMM][MAX_COMM_NAME];
    int  comnum[MAX_COMM];
} CommArray;

enum {
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
    FREELINE = -1,
    ISLABEL = -2,
    UNKNOWN = -3,

    RAX = 0,
    RBX = 1,
    RCX = 2,
    RDX = 3,
    SP = 13,
    IP = 14
};

int   FileCheck    (FILE* f, char* name);
int   AssembFailCom();
int   AssembFailArg(char* comm);
int   GetComm      (char** code, CommArray CommandArray, char** buffcom, label* l);
int   GetReg       (char** s, char* reg);
int   GetArg       (char** s, char* arg);
int   SkipSpace    (char** s);
int   SkipNonSpaces(char** s);
int   StrFree      (char** code);
int   NumLines     (char* s);
int   Assembler    (char* instr, char *outstr);
//char* Getword      (char* s);

int main() {
    Assembler("input.asemb", "out.byte");
    return 0;
}

int Assembler(char* instr, char *outstr) {
    assert(instr);
    assert(outstr);
    FILE *out = fopen(outstr, "wb");
    if(FileCheck(out, outstr) != 0)
        return -1;

    CommArray ComArr = {
            {"out", "pop", "push", "add", "sub", "mul", "div", "sin", "cos", "sqrt", "jmp", "je", "jne", "ja", "jae", "jb", "jbe"},
            {OUT, POP, PUSH, ADD, SUB, MUL, DIV, SIN, COS, SQRT, JMP, JE, JNE, JA, JAE, JB, JBE, -1},
    };
    char* code = CreateBuffer(instr);
    //ll length;
    //line* code2 = ParseBuffer(code, &length);

    int bufflen = strchr(code, '\0') - code;
    char* buffcom;
    int numlen = NumLines(code);

    char wrstr[numlen * 3];
    int  wrlen = 0;

    label l[numlen]; //array of labels {name, code position}
    int labelsize = 0;

    //FIRST PASS
    char* firstpass;
    int scounter = 0;
    for(firstpass = code; firstpass != NULL; firstpass = strchr(firstpass, '\n')) {
        firstpass++;
        SkipSpace(&firstpass);
        int comm = GetComm(&firstpass, ComArr, &buffcom, l);
        if(comm != ISLABEL) {
            if(comm != FREELINE) scounter++;
            continue;
        }
        l[labelsize].num = scounter;
        char* sup = firstpass;
        SkipNonSpaces(&sup);
        strncpy(l[labelsize].name, firstpass, sup - firstpass - 1); //Do not copy ':'
        labelsize++;
    }
    l[labelsize].num = -1;


    //SECOND PASS
    //char command = -1;
    int CheckArg = 0;
    char arg = 0;

    printf("bufflen/3 = %d\n", bufflen);
    for(; code != NULL; code = strchr(code, '\n')) { //&& *(code+1) != '\0'
        code++;
        CheckArg = 0;
        char command = GetComm(&code, ComArr, &buffcom, l);         //must be integer
        printf("comm = %d\n", command);
        if(command == FREELINE || command == ISLABEL) continue;
        if(command == UNKNOWN) return AssembFailCom();
        if(command != 0)
            wrstr[wrlen++] = command;
        //code += len;
        switch(command) {
        case OUT:
            if(! StrFree(&code)) return AssembFailArg(buffcom);
            break;
        case POP:
            //SkipSpaces(code);
            if(StrFree(&code)) return AssembFailArg(buffcom);
            if(GetReg(&code, &arg))
                wrstr[wrlen-1] = POP2;
            else
                CheckArg = GetArg(&code, &arg);
            if(! StrFree(&code)) return AssembFailArg(buffcom);
            break;
        case PUSH:
            if(StrFree(&code)) return AssembFailArg(buffcom);
            if((CheckArg = GetReg(&code, &arg)) == 1)
                wrstr[wrlen-1] = PUSH2;
            else
                CheckArg = GetArg(&code, &arg);
            if((! CheckArg) || (! StrFree(&code))) return AssembFailArg(buffcom);
            break;
        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case SIN:
        case COS:
        case SQRT:
            if(! StrFree(&code)) return AssembFailArg(buffcom);
            break;
        case JMP:
        case JE:
        case JNE:
        case JA:
        case JAE:
        case JB:
        case JBE:
        {
            if(StrFree(&code)) return AssembFailArg(buffcom);
            int i;
            for(i = 0; l[i].num != -1; i++)
                if(strncmp(l[i].name, code, strlen(l[i].name))) {
                    arg = l[i].num;
                    CheckArg = 1;
                    break;
                }
            return AssembFailArg(buffcom); //unknown label
            break;
        }
        default:
            printf("No definition for command %s!\n", buffcom);
            return -3;
        }
        if(CheckArg)
            wrstr[wrlen++] = arg;
        puts(wrstr);

    }

    int i;
    for(i = 0; i < wrlen; i++)
        wrstr[i] += '0';
    wrstr[wrlen] = '\0';
    printf("wrstr|\n");
    printf("wrstr = %s\n",wrstr);
    fwrite(wrstr, sizeof(char), wrlen, out);

    for(i = 0; l[i]; i++)
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

int GetComm(char** code, CommArray CommandArray, char** buffcom, label* l) {
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
            if(*buffcom) *buffcom = CommandArray.com[i];
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

/*char* Getword(char* s) {
    SkipSpace(s);
    char *sup = strchr(s, ' ');
    if(sup == NULL) sup = strchr(s, '\n');
    if(s == sup) return NULL;
    return strndup(s, sup - s);
}*/

int AssembFailCom() {
    fprintf(stderr, "Unknown kommand!\n");
    return -1;
}

int AssembFailArg(char* comm) {
    fprintf(stderr, "%s : Invalid argument!\n", comm);
    return -2;
}

int GetReg(char** s, int *reg) {
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

int GetArg(char** s, int *arg) { // *s = "     54354   \n ghjgkjkg fg"
    SkipSpace(s);
    int i;
    for(i = 0; (*s)[i] != ' ' && (*s)[i] != '\n' && (*s)[i] != '\0'; i++) {
        if((*s)[i] < '0' || (*s)[i] > '9') return 0;
    }
    *arg = atoi(*s);
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

