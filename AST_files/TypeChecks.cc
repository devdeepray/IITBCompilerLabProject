#include "TypeChecks.h"

bool retTypeCompatible(ValType expReturnType, ValType actReturnType)
{
  if(expReturnType == TYPE_WEAK || actReturnType == TYPE_WEAK) return true;
  
  if(expReturnType == TYPE_INT)
  {
    return actReturnType == TYPE_INT;
  }
  
  if(expReturnType == TYPE_FLOAT)
  {
    return actReturnType == TYPE_INT ||
	    actReturnType == TYPE_FLOAT;
  }
  
  return false;
}

bool assTypeCompatible(ValType lval, ValType rval)
{
  return retTypeCompatible(lval, rval);
}

bool binOpTypeCompatible(ValType val1, ValType val2, OpType op)
{
  
  if(val1 == TYPE_WEAK || val2 == TYPE_WEAK) return true;
  
  return (val1 == TYPE_INT || val1 == TYPE_FLOAT) && (val2 == TYPE_INT || val2 == TYPE_FLOAT);

}

ValType getDominantType(ValType val1, ValType val2)
{
  if(val1 == TYPE_FLOAT)
  {
    if(val2 == TYPE_FLOAT || val2 == TYPE_INT)
      return TYPE_FLOAT;
    else
      return TYPE_WEAK;
  }
  else if(val1 == TYPE_INT)
  {
    if(val2 == TYPE_FLOAT) return TYPE_FLOAT;
    
    else if(val2 == TYPE_INT) return TYPE_INT;
    
    else return TYPE_WEAK;
  }
  return TYPE_WEAK;
}

bool unaryOpCompatible(OpType op, ValType val)
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
