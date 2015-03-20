#include "TypeChecks.h"

bool castTypeCompatible(DataType expReturnType, DataType actReturnType)
{
  if(expReturnType.getPrimitiveType() == TYPE_WEAK || actReturnType.getPrimitiveType() == TYPE_WEAK)
  {
    return true;
  }
  
  
  if(expReturnType.getPrimitiveType() == TYPE_VOID || actReturnType.getPrimitiveType() == TYPE_VOID 
    || expReturnType.getPrimitiveType() == TYPE_STR || actReturnType.getPrimitiveType() == TYPE_STR)
  {
    return false;
  }
  
  if(!expReturnType.isPrimitive() || !actReturnType.isPrimitive() )
  {
    return expReturnType == actReturnType;
  }
  
  
  return true;
}


// Compatibility for binary ops
bool binOpTypeCompatible(DataType val1, DataType val2, OpType op)
{
  
  if(val1.getPrimitiveType() == TYPE_WEAK || val1.getPrimitiveType() == TYPE_WEAK)
  {
    return true;
  }
  
  
  if(val1.getPrimitiveType() == TYPE_VOID || val2.getPrimitiveType() == TYPE_VOID 
    || val1.getPrimitiveType() == TYPE_STR || val2.getPrimitiveType() == TYPE_STR)
  {
    return false;
  }
  
  if(!val1.isPrimitive() || !val2.isPrimitive() )
  {
    return false;
  }
  
 
  return true;

}


// Gets dominant type for binary operators
DataType getDominantType(DataType val1, DataType val2)
{

  DataType res;
  
  // Any of them is void or weak
  if(val1.getPrimitiveType() == TYPE_VOID || val2.getPrimitiveType() == TYPE_VOID 
    || val1.getPrimitiveType() == TYPE_WEAK || val2.getPrimitiveType() == TYPE_WEAK
    || val1.getPrimitiveType() == TYPE_STR || val2.getPrimitiveType() == TYPE_STR
    || !val1.isPrimitive() || !val2.isPrimitive())
  {
    res.setPrimitive(TYPE_WEAK);
    return res;
  }
  
  
  if(val1.getPrimitiveType() == TYPE_FLOAT || val2.getPrimitiveType() == TYPE_FLOAT)
  {
    res.setPrimitive(TYPE_FLOAT);
    return res;
  }
  
  res.setPrimitive(TYPE_INT);
  return res;
}

bool unaryOpCompatible(OpType op, DataType val)
{
  if(val == TYPE_WEAK) return true;
  
  if(op == OP_UMINUS || op == OP_PP)
  {
    if(val == TYPE_INT || val == TYPE_FLOAT)
      return true;
    else
      return false;
  }
  else if(op == OP_NOT)
  {
    if(val == TYPE_INT) return true;
    else return false;
  }
  return false;
}
