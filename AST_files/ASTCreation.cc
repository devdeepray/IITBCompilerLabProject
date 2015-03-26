#include "ASTNodes.h"
#include "Enums.h"
#include "SymbolTable.h"
#include "TypeChecks.h"

ExpAst* createBinOpAst(ExpAst* lexp, ExpAst* rexp, OpType genop, OpType intop, OpType fltop, int lineCount)
{
    bool comp = binOpTypeCompatible(lexp->dataType(), rexp->dataType(), genop);
    DataType tmp = getDominantType(lexp->dataType(), rexp->dataType());
    ExpAst* tmp1 = lexp, *tmp2 = rexp;
    ExpAst *res;
    if(tmp.getPrimitiveType() == TYPE_FLOAT)
    {

    if((lexp)->dataType().getPrimitiveType() == TYPE_INT)
    {
      tmp1 = new UnaryOp(lexp, OP_TOFLT);
      tmp1->validAST() = lexp->validAST();
    }

    if(rexp->dataType().getPrimitiveType() == TYPE_INT)
    {
      tmp2 = new UnaryOp(rexp, OP_TOFLT);
      tmp2->validAST() = rexp->validAST();
    }

    res = new BinaryOp(tmp1, tmp2, fltop);
    }
    else
    {
      res = new BinaryOp(lexp, rexp, intop);
    }

    res->validAST() = lexp->validAST() && rexp->validAST() && comp;
    res->dataType() = tmp;

    if(!comp)
    {
        // Wrong type mismatch
        cat::parse::stmterror::incompboptype(lineCount, lexp->dataType(), rexp->dataType(), genop);
    }
    return res;
}
