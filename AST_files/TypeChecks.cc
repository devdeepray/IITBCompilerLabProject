#include "TypeChecks.h"

// Checks if one can be cast into the other
bool castTypeCompatible(DataType expReturnType, DataType actReturnType)
{

  // Weak types can be cast into anything 
  if(expReturnType.getPrimitiveType() == TYPE_WEAK 
    || actReturnType.getPrimitiveType() == TYPE_WEAK)
  {
    return true;
  }
  
  // Void or string cannot be cast into anything
  if(expReturnType.getPrimitiveType() == TYPE_VOID 
    || actReturnType.getPrimitiveType() == TYPE_VOID 
    || expReturnType.getPrimitiveType() == TYPE_STR 
    || actReturnType.getPrimitiveType() == TYPE_STR)
  {
    return false;
  }
  
  // If non primitive, then allow casting if exactly the same
  // TODO If non primitive, allow casting if same total size or like GCC,
  // always allow casts
  if(!expReturnType.isPrimitive() || !actReturnType.isPrimitive() )
  {
    return !expReturnType.isPrimitive() && !actReturnType.isPrimitive();
  }
  
  // If int to float or float to int, etc.
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

  if(val.getPrimitiveType() == TYPE_WEAK) return true;
  if(!val.isPrimitive()) return false;
  
  if(op == OP_UMINUS || op == OP_PP)
  {
    if(val.getPrimitiveType() == TYPE_INT || val.getPrimitiveType() == TYPE_FLOAT)
      return true;
    else
      return false;
  }
  else if(op == OP_NOT)
  {
    if(val.getPrimitiveType() == TYPE_INT || val.getPrimitiveType() == TYPE_FLOAT) return true;
    else return false;
  }
  return false;
}
