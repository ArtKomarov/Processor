/*
 * ProcessorConsts.c
 *
 *  Created on: Nov 16, 2019
 *      Author: artem
 */
#include "AssembConsts.h"
#include "ProcessorConsts.h"

const int LAST8  = 255;
const int FIRST8 = 255 << ((sizeof(type_a) - 1) * 8);
const char COMMENT_SYMB = '%';
const char LABEL_SYMB = ':';
