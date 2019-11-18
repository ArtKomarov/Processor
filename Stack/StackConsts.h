/*
 * StackConsts.h
 *
 *  Created on: Oct 19, 2019
 *      Author: artem
 */

#ifndef STACKCONSTS_H_
#define STACKCONSTS_H_

#include <errno.h>

typedef unsigned int ui;
typedef unsigned long long int llu;
typedef float stk_t;

enum { //Error numbers
    ECANARY1 = 132,
    ECANARY2 = 133,
    EHASH  = 134,
    ECANARYBOTH = 135,
    EOPFILEERR = 136,
    EOPFILECH = 137,
    EUNDERFLOW = 138,
    EBUFEMPTY = ENOBUFS,
    ENOTENMEM = ENOMEM,
    BUFFOVERFLOW = EOVERFLOW

};

extern const ui CANARY1_CONST;
extern const ui CANARY2_CONST;
extern const ui HASH_START;
extern const llu MASK;//8589934591; // 2^33 - 1
extern const ui ODDS;
extern const stk_t POISON;
extern const stk_t LIMIT;
extern const int STR_LEN;
extern const char STR_ERR_FILE[];
extern const char STR_CHECK_FILE[];

#endif /* STACKCONSTS_H_ */
