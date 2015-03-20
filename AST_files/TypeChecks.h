#ifndef _TYPE_CHECK_H_
#define _TYPE_CHECK_H_

#define SIZE_OF_PTR 4
#include "Enums.h"
#include "SymbolTable.h"
bool castTypeCompatible(DataType expReturnType, DataType actReturnType);

bool binOpTypeCompatible(DataType val1, DataType val2, OpType op);

DataType getDominantType(DataType val1, DataType val2);

bool unaryOpCompatible(OpType op, DataType val);
#endif // _TYPE_CHECK_H_
