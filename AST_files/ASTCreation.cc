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
	switch(genop)
	{
		case OP_LT: 
		
		case OP_GT:
		
		case OP_LE:
	
		case OP_GE:	
		
		case OP_EQ:
		case OP_AND:
		case OP_OR:
		res->data_type = TYPE_INT;
		break;
		
	}
    if(!comp)
    {
        // Wrong type mismatch
        cat::parse::stmterror::incompboptype(lineCount, lexp->dataType(), rexp->dataType(), genop);
    }
    return res;
}

ExpAst* createAssignmentAst(ExpAst* lexp, ExpAst* rexp, int lineCount)
{
	bool comp = binOpTypeCompatible(lexp->dataType(), rexp->dataType(), OP_ASSIGN);
	DataType tmp = lexp->dataType();
	ExpAst* rtmp = rexp;
	if(tmp.getPrimitiveType() != rtmp->dataType().getPrimitiveType())
	{
		if(tmp.getPrimitiveType() == TYPE_INT)
		{
			rtmp = new UnaryOp(rexp, OP_TOINT);
		}
		else
		{
			rtmp = new UnaryOp(rexp, OP_TOFLT);
		}
		rtmp->validAST() = rexp->validAST();
	}
	ExpAst* res = new BinaryOp(lexp, rtmp, OP_ASSIGN);
	res->validAST() = lexp->validAST() && rexp->validAST() && comp;
	
	res->data_type = tmp;
	if(!comp)
	{
		cat::parse::stmterror::incompboptype(lineCount, lexp->dataType(), rexp->dataType(), OP_ASSIGN);
	}
	return res;
}
	
