#include "Enums.h"

std::string varDeclTypeLookup[] = {"GLOBAL", "LOCAL", "PARAM" };
				    
				    
std::string astTypeLookup[] = {"STMT", "BLK", "EXP", "EMP_BLK", "ASS", "RET", "IF", "FOR", 
			"WHILE", "FLOAT", "BINARY_OP", "UNARY_OP", "FUNCALL",
			  "INT", "STR", "IDENTIFIER", "INDEX"};

std::string opTypeLookup[]  = {"OR", "AND", "EQ_OP", "NE_OP", "LT","GT", "LE_OP", "GE_OP", 
		"PLUS", "MINUS", "MULT", "DIV", "ASSIGN", "UMINUS", "NOT", "PP"};

std::string valTypeLookup[] = {"INT", "FLOAT", "VOID", "CHAR", "ARRAY", "POINTER", "TYPE_WEAK"};