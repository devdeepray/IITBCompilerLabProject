
%scanner Scanner.h
%scanner-token-function d_scanner.lex()

%token FP_CONST INT_CONST VOID INT FLOAT FOR WHILE IF ELSE RETURN IDENTIFIER
%token LEQ_OP GEQ_OP INCREMENT STRING_LITERAL
%token LOGICAL_AND LOGICAL_OR EQUAL_TO NEQ_TO


%%



translation_unit:
	function_definition 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"translation_unit\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
	}
	| translation_unit function_definition  
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"translation_unit\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
		std::cout << $$ << "->" << $2 <<";" << std::endl;
	}	
	;

function_definition:
	type_specifier fun_declarator compound_statement 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"function_definition\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
		std::cout << $$ << "->" << $2 <<";" << std::endl;
		std::cout << $$ << "->" << $3 <<";" << std::endl;
	}
	;

type_specifier:
	VOID 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"type_specifier\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"void\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	| INT 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"type_specifier\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"int\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	| FLOAT 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"type_specifier\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"float\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	;

fun_declarator
	: IDENTIFIER '(' parameter_list ')' 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"fun_declarator\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"identifier\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"(\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << $$ << "->" << $3 <<";" << std::endl;		
		
		std::cout << ++nodeCount << " [label=\")\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;	
	}
	| IDENTIFIER '(' ')' 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"fun_declarator\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"identifier\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"(\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		
		std::cout << ++nodeCount << " [label=\")\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;	
	}
	;

parameter_list
	: parameter_declaration 
	{ 
		$$=++nodeCount;
		std::cout << $$ << " [label=\"parameter_list\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl; 
	}
	| parameter_list ',' parameter_declaration
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"parameter_list\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\",\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		
		std::cout << $$ << "->" << $3 <<";" << std::endl;	
	}
	;

parameter_declaration
	: type_specifier declarator 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"parameter_declaration\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;		
		std::cout << $$ << "->" << $2 <<";" << std::endl;
	}
	;


declarator
	: IDENTIFIER 
	{ 
		$$=++nodeCount;
		std::cout << $$ << " [label=\"declarator\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"identifier\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	| declarator '[' constant_expression ']'
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"declarator\"];" << std::endl;

		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"[\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << $$ << "->" << $3 <<";" << std::endl;		
		
		std::cout << ++nodeCount << " [label=\"]\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	;

constant_expression
        : INT_CONST 
	{ 
		$$=++nodeCount;
		std::cout << $$ << " [label=\"constant_expression\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"int_const\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
        | FP_CONST
	{ 
		$$=++nodeCount;
		std::cout << $$ << " [label=\"constant_expression\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"fp_const\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
        ;

compound_statement
	: '{' '}' 
	{ 
		$$=++nodeCount;
		std::cout << $$ << " [label=\"compound_statement\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"{\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
		
		std::cout << ++nodeCount << " [label=\"}\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	| '{' statement_list '}'
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"compound_statement\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"{\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
		std::cout << $$ << "->" << $2 <<";" << std::endl;
		std::cout << ++nodeCount << " [label=\"}\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
  	| '{' declaration_list statement_list '}'
	{
		
		$$=++nodeCount;
		std::cout << $$ << " [label=\"compound_statement\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"{\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
		std::cout << $$ << "->" << $2 <<";" << std::endl;
		std::cout << $$ << "->" << $3 <<";" << std::endl;
		std::cout << ++nodeCount << " [label=\"}\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	;

statement_list
	: statement 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"statement_list\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl; 
	}
	| statement_list statement 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"statement_list\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
		std::cout << $$ << "->" << $2 <<";" << std::endl;
	}
	;

statement
 	: compound_statement 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"statement\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl; 
	}
	| selection_statement 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"statement\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl; 
	}
	| iteration_statement 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"statement\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl; 
	}
	| assignment_statement 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"statement\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl; 
	}
	| RETURN expression ';'	
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"statement\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"return\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
		std::cout << $$ << "->" << $2 <<";" << std::endl;
		
		std::cout << ++nodeCount << " [label=\";\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	;

assignment_statement
	: assignment ';' 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"assignment_statement\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
		
		std::cout << ++nodeCount << " [label=\";\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	;

assignment
	:
	|  l_expression '=' expression
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"assignment\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"=\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		
		std::cout << $$ << "->" << $3 <<";" << std::endl;
	}
	;

expression
	: logical_and_expression 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl; 
	}
	| expression LOGICAL_OR logical_and_expression
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"logical_or\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		
		std::cout << $$ << "->" << $3 <<";" << std::endl;
	}
	;

logical_and_expression
	: equality_expression 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"logical_and_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
	}
	| logical_and_expression LOGICAL_AND equality_expression
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"logical_and_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"logical_and\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		
		std::cout << $$ << "->" << $3 <<";" << std::endl;
	}
	;

equality_expression
	: relational_expression 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"equality_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
	}
	| equality_expression EQUAL_TO relational_expression
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"equality_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"equal_to\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		
		std::cout << $$ << "->" << $3 <<";" << std::endl;
	}
	| equality_expression NEQ_TO relational_expression
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"equality_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"neq_to\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		
		std::cout << $$ << "->" << $3 <<";" << std::endl;
	}
	;
relational_expression
	: additive_expression 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"relational_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
	}
	| relational_expression '<' additive_expression
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"relational_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"<\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		
		std::cout << $$ << "->" << $3 <<";" << std::endl;
	}
	| relational_expression '>' additive_expression
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"relational_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\">\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		
		std::cout << $$ << "->" << $3 <<";" << std::endl;
	}
	| relational_expression LEQ_OP additive_expression
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"relational_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"leq_op\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		
		std::cout << $$ << "->" << $3 <<";" << std::endl;
	}
	| relational_expression GEQ_OP additive_expression
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"relational_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"geq_op\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		
		std::cout << $$ << "->" << $3 <<";" << std::endl;
	}
	;

additive_expression
	: multiplicative_expression 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"additive_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
	}
	| additive_expression '+' multiplicative_expression
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"additive_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"+\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		
		std::cout << $$ << "->" << $3 <<";" << std::endl;
	}
	| additive_expression '-' multiplicative_expression
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"additive_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"-\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		
		std::cout << $$ << "->" << $3 <<";" << std::endl;
	}
	;

multiplicative_expression
	: unary_expression 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"multiplicative_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
	}
	| multiplicative_expression '*' unary_expression
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"multiplicative_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"*\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		
		std::cout << $$ << "->" << $3 <<";" << std::endl;
	}
	| multiplicative_expression '/' unary_expression
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"multiplicative_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"/\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		
		std::cout << $$ << "->" << $3 <<";" << std::endl;
	}
	;
unary_expression
	: postfix_expression 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"unary_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
	}
	| unary_operator postfix_expression 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"unary_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
		std::cout << $$ << "->" << $2 <<";" << std::endl;
	}
	;

postfix_expression
	: primary_expression 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"postfix_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
	}
	| IDENTIFIER '(' ')' 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"postfix_expression\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"identifier\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"(\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		
		std::cout << ++nodeCount << " [label=\")\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	| IDENTIFIER '(' expression_list ')'
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"postfix_expression\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"identifier\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"(\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		std::cout << $$ << "->" << $3 <<";" << std::endl;
		std::cout << ++nodeCount << " [label=\")\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	| l_expression INCREMENT 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"postfix_expression\"];" << std::endl;
		
		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"increment\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	;

primary_expression
	: l_expression 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"primary_expression\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
	}
	| INT_CONST 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"primary_expression\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"int_const\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	| FP_CONST 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"primary_expression\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"fp_const\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	| STRING_LITERAL 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"primary_expression\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"string_literal\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	| '(' expression ')'
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"primary_expression\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"(\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
		std::cout << $$ << "->" << $2 <<";" << std::endl;
		std::cout << ++nodeCount << " [label=\")\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	;

l_expression
	: IDENTIFIER 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"l_expression\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"identifier\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	| l_expression '[' expression ']'
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"l_expression\"];" << std::endl;

		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"[\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << $$ << "->" << $3 <<";" << std::endl;		
		
		std::cout << ++nodeCount << " [label=\"]\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	} 
        ;
expression_list
	: expression 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"expression_list\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
	}
	| expression_list ',' expression
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"expression_list\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\",\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;		
		
		std::cout << $$ << "->" << $3 <<";" << std::endl;
	} 
	;

unary_operator
	: '-'  
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"unary_operator\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"-\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	| '!'  
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"unary_operator\"];" << std::endl;
		std::cout << ++nodeCount << " [label=\"!\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	;

selection_statement
	: IF '(' expression ')' statement ELSE statement
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"selection_statement\"];" << std::endl;

		std::cout << ++nodeCount << " [label=\"IF\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"(\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << $$ << "->" << $3 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\")\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << $$ << "->" << $5 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"else\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << $$ << "->" << $7 <<";" << std::endl;
	}
	;

iteration_statement
	: WHILE '(' expression ')' statement
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"iteration_statement\"];" << std::endl;

		std::cout << ++nodeCount << " [label=\"while\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"(\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << $$ << "->" << $3 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\")\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << $$ << "->" << $5 <<";" << std::endl;
		
	}
	| FOR '(' assignment_statement expression ';' assignment ')' statement
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"iteration_statement\"];" << std::endl;

		std::cout << ++nodeCount << " [label=\"for\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\"(\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << $$ << "->" << $3 <<";" << std::endl;
		std::cout << $$ << "->" << $4 <<";" << std::endl;		

		std::cout << ++nodeCount << " [label=\";\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << $$ << "->" << $6 <<";" << std::endl;

		std::cout << ++nodeCount << " [label=\")\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << $$ << "->" << $8 <<";" << std::endl;
	}
	;

declaration_list
	: declaration 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"declaration_list\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
	}
	| declaration_list declaration 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"declaration_list\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
		std::cout << $$ << "->" << $2 <<";" << std::endl;
	}
	;

declaration
	: type_specifier declarator_list';'
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"declaration\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
		std::cout << $$ << "->" << $2 <<";" << std::endl;
		
		std::cout << ++nodeCount << " [label=\";\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;
	}
	;

declarator_list
	: declarator 
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"declarator_list\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
	}
	| declarator_list ',' declarator
	{
		$$=++nodeCount;
		std::cout << $$ << " [label=\"declarator_list\"];" << std::endl;
		std::cout << $$ << "->" << $1 <<";" << std::endl;
		
		
		std::cout << ++nodeCount << " [label=\",\"];" << std::endl;
		std::cout << $$ << "->" << nodeCount <<";" << std::endl;

		std::cout << $$ << "->" << $3 <<";" << std::endl;
	}
	;
