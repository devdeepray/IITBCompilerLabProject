#include "Enums.h"

std::string varDeclTypeLookup[] = {"GLOBAL", "LOCAL", "PARAM" };
				    
				    
std::string astTypeLookup[] = {"STMT", "BLK", "EXP", "EMP_BLK", "ASS", "RET", "IF", "FOR", 
			"WHILE", "FLOAT", "BINARY_OP", "UNARY_OP", "FUNCALL",
			  "INT", "STR", "IDENTIFIER", "INDEX"};

std::string opTypeLookup[]  = {"OP_OR", "OP_AND", "OP_EQ", "OP_INT_EQ", "OP_FLOAT_EQ", "OP_NE", "OP_INT_NE", "OP_FLOAT_NE",
             "OP_LT", "OP_INT_LT", "OP_FLOAT_LT", "OP_GT", "OP_INT_GT", "OP_FLOAT_GT", "OP_LE",  "OP_INT_LE", "OP_FLOAT_LE",
             "OP_GE", "OP_INT_GE", "OP_FLOAT_GE", "OP_PLUS", "OP_INT_PLUS", "OP_FLOAT_PLUS", "OP_MINUS",  "OP_INT_MINUS",
             "OP_FLOAT_MINUS", "OP_MULT",  "OP_INT_MULT", "OP_FLOAT_MULT", "OP_DIV", "OP_INT_DIV", "OP_FLOAT_DIV",
             "OP_ASSIGN", "OP_UMINUS",  "OP_INT_UMINUS", "OP_FLOAT_UMINUS", "OP_NOT", "OP_PP", "OP_INT_PP", "OP_FLOAT_PP",
             "OP_TOFLT", "OP_TOINT"};
std::string valTypeLookup[] = {"INT", "FLOAT", "VOID", "TYPE_WEAK", "TYPE_STR"};
