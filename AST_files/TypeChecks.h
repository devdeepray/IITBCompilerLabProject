#ifndef _TYPE_CHECK_H_
#define _TYPE_CHECK_H_

#define SIZE_OF_PTR 4
#include "Enums.h"
bool retTypeCompatible(ValType expReturnType, ValType actReturnType);

bool assTypeCompatible(ValType lval, ValType rval);

bool binOpTypeCompatible(ValType val1, ValType val2, OpType op);

ValType getDominantType(ValType val1, ValType val2);

bool unaryOpCompatible(OpType op, ValType val);
#endif // _TYPE_CHECK_H_
