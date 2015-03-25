#include "Enums.h"

std::string varDeclTypeLookup[] = {"GLOBAL", "LOCAL", "PARAM" };
				    
				    
std::string astTypeLookup[] = {"statement", "block", "expression", "empty statement", "assignment statement", "return statement", "if statement", "for loop", 
			"while loop", "floating point constant", "binary operator", "unary operator", "function call",
			  "integer constant", "string", "identifier", "index"};

std::string opTypeLookup[]  = {"logical OR", "logical AND", "equality", "OP_INT_EQ", "OP_FLOAT_EQ", "inequality", "OP_INT_NE", "OP_FLOAT_NE",
             "less than", "OP_INT_LT", "OP_FLOAT_LT", "greater than", "OP_INT_GT", "OP_FLOAT_GT", "less than or equal",  "OP_INT_LE", "OP_FLOAT_LE",
             "greater than or equal", "OP_INT_GE", "OP_FLOAT_GE", "addition", "OP_INT_PLUS", "OP_FLOAT_PLUS", "subtraction",  "OP_INT_MINUS",
             "OP_FLOAT_MINUS", "multiplication",  "OP_INT_MULT", "OP_FLOAT_MULT", "division", "OP_INT_DIV", "OP_FLOAT_DIV",
             "assignment", "negation",  "OP_INT_UMINUS", "OP_FLOAT_UMINUS", "complement", "increment", "OP_INT_PP", "OP_FLOAT_PP",
             "OP_TOFLT", "OP_TOINT"};
std::string valTypeLookup[] = {"integer", "floating point", "void", "TYPE_WEAK", "TYPE_STR"};
