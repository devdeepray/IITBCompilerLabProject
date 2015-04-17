#ifndef ASTCREATION_H
#define ASTCREATION_H
#include "ASTNodes.h"
#include "Enums.h"
#include "SymbolTable.h"
#include "TypeChecks.h"
ExpAst* createBinOpAst(ExpAst* lexp, ExpAst* rexp, OpType genop, OpType intop, OpType fltop, int lineCount);
ExpAst* createAssignmentAst(ExpAst* lexp, ExpAst* rexp, int lineCount);
#endif // ASTCREATION_H
