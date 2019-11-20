/*
 * Processor.c
 *
 *  Created on: Oct 28, 2019
 *      Author: Artem
 */

#include "Processor.h"
#include "ProcessorConsts.h"
#include "Onegin/OneginFunc.h"
#include "Stack/Stack.h"
#include "Stack/StackConsts.h"
#include <math.h>

int CPU(char *input) {
    assert(input);
    Processor proc = ProcessorConstruct(input);
    int CodeLen = strlen(proc.code);
    //reg_t command;
    //bcode buff;
    //buff.code = CreateBuffer(input);
    //buff.current = 0;
    int linenum = 0;
    CommFunc cfunc[MAX_COMM];
    FillCommFunc(cfunc);
    int CommCheck;
    while(proc.rip >= 0) {
        int i;
        CommCheck = -1;
        int comm = GetCPUComm(&proc);
        linenum++;
        for(i = 0; cfunc[i].NumComm >= 0; i++) {
            if(cfunc[i].NumComm == comm) {
                CommCheck = cfunc[i].comm(&proc);

                //if(proc.stk.size > 0) printf("Last elem = %f!\n", proc.stk.stk[proc.stk.size - 1]);

                if(proc.rip >= CodeLen) proc.rip = -1;
                break;
            }
        }
        if(CommCheck < 0) {
            PrintCommError(comm, linenum);
            //printf("Some command catch error!\n");
            ProcessorDestruct(&proc);
            return -1;
        }
    }
    ProcessorDestruct(&proc);
    //free(buff.code);
    return 0;
}

Processor ProcessorConstruct(char* input) {
    assert(input != NULL);
    Processor proc;
    proc.rip = 0;
    proc.code = CreateBuffer(input);
    proc.flag = 0;
    proc.rsp = 0;
    proc.stk = StackConstruct(STACK_START_CAP, NULL, NULL);
    return proc;
}

int ProcessorDestruct(Processor *proc) {
    assert(proc != NULL);
    proc->rip = -1;
    free(proc->code);
    proc->flag = 0;
    proc->rsp = 0;
    StackDestruct(&proc->stk);
    return 0;
}

int FillCommFunc(CommFunc* cf) {
    assert(cf != NULL);
    int i = 0;
    MakePair(cf, OUT,   out,   &i);
    MakePair(cf, POP,   pop,   &i);
    MakePair(cf, POP2,  pop2,  &i);
    MakePair(cf, PUSH,  push,  &i);
    MakePair(cf, PUSH2, push2, &i);
    MakePair(cf, ADD,   add,   &i);
    MakePair(cf, SUB,   sub,   &i);
    MakePair(cf, MUL,   mul,   &i);
    MakePair(cf, DIV,   _div,  &i);
    MakePair(cf, SIN,   _sin,  &i);
    MakePair(cf, COS,   _cos,  &i);
    MakePair(cf, SQRT,  _sqrt, &i);
    MakePair(cf, JMP,   jmp,   &i);
    MakePair(cf, JE,    je,    &i);
    MakePair(cf, JNE,   jne,   &i);
    MakePair(cf, JA,    ja,    &i);
    MakePair(cf, JAE,   jae,   &i);
    MakePair(cf, JB,    jb,    &i);
    MakePair(cf, JBE,   jbe,   &i);
    MakePair(cf, PRNT,  prnt,  &i);
    MakePair(cf, CMP,   cmp,  &i);
    MakePair(cf, -1,    out,   &i);
    return 0;
}

int MakePair(CommFunc* cf, int NumComm, int (*comm)(Processor* proc), int* i) {
    assert(cf != NULL);
    assert(comm != NULL);
    assert(i != NULL);
    assert(*i >= 0 && *i <= MAX_COMM);
    cf[*i].NumComm = NumComm;
    cf[*i].comm = comm;
    (*i)++;
    return 1;
}

int GetCPUArg(Processor* proc, type_a* arg) {
    assert(proc != NULL);
    assert(arg  != NULL);
    int i;
    union Type_a4byte sup;
    for(i = 0; i < UNION_CHAR_CAP; i++) {
        if( proc->code[proc->rip] == '\0') return -1; //+1
        sup.s[i] = proc->code[proc->rip] - '0'; //+1
        proc->rip++;
    }
    *arg = sup.f;
    return 1;
}

int GetCPUReg(Processor* proc, int* num) {
    assert(proc != NULL);
    assert(num  != NULL);
    int RegNum = proc->code[++proc->rip] - '0';
    if(RegNum < 0 && RegNum >= PROC_NUM_OF_REGS)
        return -1;
    else
        *num = RegNum;
    return 1;
}

int GetCPUComm(Processor* proc) {
    assert(proc != NULL);
    int comm = proc->code[proc->rip++] - '0';
    return comm;
}

int PrintCommError(int comm, int linenum) {
    char c[MAX_COMM_NAME];
    c[0] = '\0';
    if(comm == POP || comm == POP2)
        fprintf(stderr, "%d: Command pop catch error: EUNDERFLOW or invalid register number!\n", linenum);
    else if(comm == PUSH || comm == PUSH2)
        fprintf(stderr, "%d: Command push catch error: ENOMEM or invalid register number!\n", linenum);
    else {
        switch(comm) {
        case OUT:
            strcpy(c, "out");
            break;
        case ADD:
            strcpy(c, "add");
            break;
        case SUB:
            strcpy(c, "sub");
            break;
        case MUL:
            strcpy(c, "mul");
            break;
        case SIN:
            strcpy(c, "sin");
            break;
        case COS:
            strcpy(c, "cos");
            break;
        case PRNT:
            strcpy(c, "prnt");
            break;
        }
        if(c[0] != '\0') fprintf(stderr, "%d: Command %s catch error!\n", linenum, c);
        else {
            switch (comm) {
            case JMP:
                strcpy(c, "jmp");
                break;
            case JE:
                strcpy(c, "je");
                break;
            case JNE:
                strcpy(c, "jne");
                break;
            case JA:
                strcpy(c, "ja");
                break;
            case JAE:
                strcpy(c, "jae");
                break;
            case JB:
                strcpy(c, "jb");
                break;
            case JBE:
                strcpy(c, "jbe");
                break;
            }
            if(c[0] != '\0') fprintf(stderr, "%d: Command %s catch error: jamp into big or less ziro number!\n", linenum, c);
            else if(comm == SQRT) fprintf(stderr, "%d: Command sqrt catch error: maybe argument is less then ziro!\n", linenum);
            else if(comm == DIV) fprintf(stderr, "%d: Command div catch error: maybe argument is less then ziro!\n", linenum);
            else {
                switch(comm) {
                case PRNT:
                    strcpy(c, "prnt");
                    break;
                case CMP:
                    strcpy(c, "cmp");
                    break;
                }
                if(c[0] != '\0')
                    fprintf(stderr, "%d: Command %s catch error: Not enough arguments in stack or not enough memory for push back arguments in stack (because we get them with pop)!\n", linenum, c);
                    else fprintf(stderr, "%d: Unknown command used!\n", linenum);
            }
        }
    }
    /*switch(comm) {
    case OUT:
        fprintf(stderr, "Command out catch error!\n");
        break;
    case POP:
    case POP2:
        fprintf(stderr, "Command pop catch error: EUNDERFLOW or invalid register number!\n");
        break;
    case PUSH:
    case PUSH2:
        fprintf(stderr, "Command push catch error: ENOMEM or invalid register number!\n");
        break;
    case ADD:

    }*/
    return 0;
}

int out(Processor* proc) {
    assert(proc != NULL);
    return 0;
}

int pop(Processor* proc) {
    assert(proc != NULL);
    StackPop(&proc->stk);
    if(proc->stk.err == EUNDERFLOW) return -1;
    return 1;
}

int pop2(Processor* proc) {
    assert(proc != NULL);
    //stk_t pop = StackPop(&proc->stk);
    int RegNum; // = proc->code[++proc->rip] - '0';
    if(GetCPUReg(proc, &RegNum) == 1)
        proc->reg[RegNum] = (reg_t) StackPop(&proc->stk);
    else
        return -1;
    if(proc->stk.err == EUNDERFLOW)
        return -1;
    return 1;
}

int push(Processor* proc) {
    assert(proc != NULL);
    type_a arg;
    if(GetCPUArg(proc, &arg) == -1) return -1;
    StackPush(&proc->stk, (stk_t) arg);
    if(proc->stk.err == ENOTENMEM) return -1;
    return 1;
}
int push2(Processor* proc) {
    assert(proc != NULL);
    int num;
    if(GetCPUReg(proc, &num) == -1) return -1;
    StackPush(&proc->stk, (stk_t) proc->reg[num]);
    if(proc->stk.err == ENOTENMEM) return -1;
    return 1;
}

int add(Processor* proc) {
    assert(proc != NULL);
    stk_t arg = StackPop(&proc->stk);
    arg += StackPop(&proc->stk);
    if(proc->stk.err == EUNDERFLOW) return -1;
    else StackPush(&proc->stk, arg);
    if(proc->stk.err == ENOTENMEM) return -1;
    return 1;
}

int sub(Processor* proc) {
    assert(proc != NULL);
    stk_t arg = StackPop(&proc->stk);
    arg -= StackPop(&proc->stk);
    if(proc->stk.err == EUNDERFLOW) return -1;
    else StackPush(&proc->stk, arg);
    if(proc->stk.err == ENOTENMEM) return -1;
    return 1;
}

int mul(Processor* proc) {
    assert(proc != NULL);
    stk_t arg = StackPop(&proc->stk);
    arg *= StackPop(&proc->stk);
    if(proc->stk.err == EUNDERFLOW) return -1;
    else StackPush(&proc->stk, arg);
    if(proc->stk.err == ENOTENMEM) return -1;
    return 1;
}

int _div(Processor* proc) {
    assert(proc != NULL);
    stk_t arg1 = StackPop(&proc->stk);
    stk_t arg2 = StackPop(&proc->stk);
    if(arg2 == 0) return -1;
    arg1 /= arg2;
    if(proc->stk.err == EUNDERFLOW) return -1;
    else StackPush(&proc->stk, arg1);
    if(proc->stk.err == ENOTENMEM) return -1;
    return 1;
}

int _sin(Processor* proc) {
    assert(proc != NULL);
    stk_t arg1 = StackPop(&proc->stk);
  //  stk_t arg2 = sin(arg1);
    if(proc->stk.err == EUNDERFLOW) return -1;
    //else StackPush(&proc->stk, arg2);
    if(proc->stk.err == ENOTENMEM) return -1;
    return 1;
}

int _cos(Processor* proc) {
    assert(proc != NULL);
    stk_t arg1 = StackPop(&proc->stk);
    //stk_t arg2 = cos(arg1);
    if(proc->stk.err == EUNDERFLOW) return -1;
    //else StackPush(&proc->stk, arg2);
    if(proc->stk.err == ENOTENMEM) return -1;
    return 1;
}

int _sqrt(Processor* proc) {
    assert(proc != NULL);
    stk_t arg1 = StackPop(&proc->stk);
    //stk_t arg2 = sqrt(arg1);
    if(proc->stk.err == EUNDERFLOW) return -1;
    //else StackPush(&proc->stk, arg2);
    if(proc->stk.err == ENOTENMEM) return -1;
    return 1;
}

int jmp(Processor* proc) {
    assert(proc != NULL);
    type_a arg;
    if(GetCPUArg(proc, &arg) == -1) return -1;
    else proc->rip = (int)arg;
    return 1;
}

int ja(Processor* proc) {
    assert(proc != NULL);
    if(proc->flag < 1) {
        proc->rip += UNION_CHAR_CAP;
        return 1;
    }
    return jmp(proc);
}

int jae(Processor* proc) {
    assert(proc != NULL);
    if(proc->flag < 0) {
        proc->rip += UNION_CHAR_CAP;
        return 1;
    }
       return jmp(proc);
}

int jb(Processor* proc) {
    assert(proc != NULL);
    if(proc->flag > -1) {
           proc->rip += UNION_CHAR_CAP;
           return 1;
       }
       return jmp(proc);
}

int jbe(Processor* proc) {
    assert(proc != NULL);
    if(proc->flag <= 0) {
        proc->rip += UNION_CHAR_CAP;
        return 1;
    }
    return jmp(proc);
}

int je(Processor* proc) {
    assert(proc != NULL);
    if(proc->flag != 0) {
        proc->rip += UNION_CHAR_CAP;
        return 1;
    }
    return jmp(proc);
}

int jne(Processor* proc) {
    assert(proc != NULL);
    if(proc->flag == 0) {
        proc->rip += UNION_CHAR_CAP;
        return 1;
    }
    return jmp(proc);
}

int prnt(Processor* proc) {
    assert(proc != NULL);
    stk_t arg = StackPop(&proc->stk);
    if(proc->stk.err == EUNDERFLOW) return -1;
    printf("%f", arg);
    StackPush(&proc->stk, arg);
    if(proc->stk.err == ENOTENMEM) return -1;
    return 1;
}

int cmp(Processor* proc) {
    assert(proc != NULL);
    stk_t arg1 = StackPop(&proc->stk);
    stk_t arg2 = StackPop(&proc->stk);
    if(proc->stk.err == EUNDERFLOW) return -1;
    if(arg1 < arg2) proc->flag = 1;
    else if(arg1 > arg2) proc->flag = -1;
    else proc->flag = 0;
    StackPush(&proc->stk, arg2);
    StackPush(&proc->stk, arg1);
    if(proc->stk.err == ENOTENMEM) return -1;
    return 1;
}


