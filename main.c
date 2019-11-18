/*
 * main.c
 *
 *  Created on: Oct 28, 2019
 *      Author: artem
 */
//#include <Processor.h>

#include "ProcessorConsts.h"
#include "Assembler.h"


int main() {
    Assembler("input.asemb", "out.byte");
    return 0;
}
