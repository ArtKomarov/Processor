/*
 * main.c
 *
 *  Created on: Oct 28, 2019
 *      Author: artem
 */
//#include <Processor.h>

#include "ProcessorConsts.h"
#include "Assembler.h"
#include "Processor.h"
#include "Disassembler.h"

int main() {
    Assembler("input.asemb", "out.byte");
    CPU("out.byte");
    Disassemb("out.byte", "out2.asemb");
    return 0;
}
