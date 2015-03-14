#ifndef _ENUMS_H_
#define _ENUMS_H_

#include <iostream>
// Enum for type of the AST. 
enum AstType {AST_STMT, AST_BLK, AST_EXP, AST_EMP, AST_ASS, AST_RET, AST_IF, AST_FOR, AST_WHILE, 
	AST_FLOAT, AST_INT, AST_BINOP, AST_UNOP, AST_FUNCALL, AST_STR, AST_IDENTIFIER, AST_INDEX};
	
// Enum for type of a variable
enum VarDeclType {GLOBAL, LOCAL, PARAM};

// Enum for types of variables
enum ValType {TYPE_INT, TYPE_FLOAT, TYPE_VOID, TYPE_CHAR, TYPE_ARRAY, TYPE_POINTER, TYPE_WEAK, TYPE_STR};

// Enum for ops
enum OpType {OP_OR, OP_AND, OP_EQ, OP_NE, OP_LT, OP_GT, OP_LE, OP_GE, OP_PLUS, OP_MINUS,
				    OP_MULT, OP_DIV, OP_ASSIGN, OP_UMINUS, OP_NOT, OP_PP}; 

extern std::string varDeclTypeLookup[];
				    
				    
extern std::string astTypeLookup[];

extern std::string opTypeLookup[];

extern std::string valTypeLookup[];
#endif //_ENUMS_H_