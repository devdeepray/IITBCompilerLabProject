// Enum for type of the AST. 
enum AstType {AST_STMT, AST_BLK, AST_EXP, AST_EMP, AST_ASS, AST_RET, AST_IF, AST_FOR, AST_WHILE, 
	AST_FLOAT, AST_INT, AST_BINOP, AST_UNOP, AST_FUNCALL, STR, IDENTIFIER, INDEX};
	
// Enum for type of a variable
enum VarDeclType {GLOBAL, LOCAL, PARAM};

// Enum for types of variables
enum ValType {TYPE_INT, TYPE_FLOAT, TYPE_VOID, TYPE_CHAR, TYPE_ARRAY, TYPE_POINTER};

// Enum for ops
enum OpType {OP_OR, OP_AND, OP_EQ, OP_NE, OP_LT, OP_GT, OP_LE, OP_GE, OP_PLUS, OP_MINUS,
				    OP_MULT, OP_DIV, OP_ASSIGN, OP_UMINUS, OP_NOT, OP_PP}; 

std::string varDeclTypeLookup[] = {"GLOBAL", "LOCAL", "PARAM" };
				    
				    
std::string astTypeLookup[] = {"STMT", "BLK", "EXP", "EMP_BLK", "ASS", "RET", "IF", "FOR", 
			"WHILE", "FLOAT", "BINARY_OP", "UNARY_OP", "FUNCALL",
			  "INT", "STR", "IDENTIFIER", "INDEX"};

std::string opTypeLookup[]  = {"OR", "AND", "EQ_OP", "NE_OP", "LT","GT", "LE_OP", "GE_OP", 
		"PLUS", "MINUS", "MULT", "DIV", "ASSIGN", "UMINUS", "NOT", "PP"};

std::string valTypeLookup[] = {"INT", "FLOAT", "VOID", "CHAR", "ARRAY", "POINTER"};