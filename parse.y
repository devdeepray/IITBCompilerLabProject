
%scanner Scanner.h
%scanner-token-function d_scanner.lex()

%token FP_CONST INT_CONST VOID INT FLOAT FOR WHILE IF ELSE RETURN IDENTIFIER
%token LEQ_OP GEQ_OP INCREMENT STRING_LITERAL
%token LOGICAL_AND LOGICAL_OR EQUAL_TO NEQ_TO

%%

translation_unit:
	function_definition {$$=$1; std::cout << "t_unit_"<<$1<<" -> func_def_"<<$1<<";"<< std::endl;}
	| translation_unit function_definition
	{ std::cout << "t_unit_"<<$1+$2<<" -> t_unit_"<<$1<<";"<< std::endl;
	std::cout << "t_unit_"<<$1+$2<<" -> func_def_"<<$2<<";"<< std::endl;}
	;

function_definition:
	type_specifier fun_declarator compound_statement
	{$$=$1+$2+$3;
	std::cout << "func_def_"<<$1+$2+$3<<" -> type_spec_"<<$1<<";"<< std::endl;
	std::cout << "func_def_"<<$1+$2+$3<<" -> fun_decl_"<<$2<<";"<< std::endl;
	std::cout << "func_def_"<<$1+$2+$3<<" -> comp_stat_"<<$3<<";"<< std::endl;}
	;

type_specifier:
	VOID {$$=$1;}
	| INT {$$=$1;}
	| FLOAT {$$=$1;}
	;

fun_declarator
	: IDENTIFIER '(' parameter_list ')' {$$=$1+$2;}
	| IDENTIFIER '(' ')' {$$=$1;}
	;

parameter_list
	: parameter_declaration
	| parameter_list ',' parameter_declaration
	;

parameter_declaration
	: type_specifier declarator
	;

declarator
	: IDENTIFIER
	| declarator '[' constant_expression ']'
	;

constant_expression
        : INT_CONST
        | FP_CONST
        ;

compound_statement
	: '{' '}'
	| '{' statement_list '}'
        | '{' declaration_list statement_list '}'
	;

statement_list
	: statement
	| statement_list statement
	;

statement
        : compound_statement { printf("> "); }
	| selection_statement
	| iteration_statement
	| assignment_statement
        | RETURN expression ';'	
	
	;

assignment_statement
	: assignment ';'
	;

assignment
	:
	|  l_expression '=' expression
	;

expression
	: logical_and_expression
	| expression LOGICAL_OR logical_and_expression
	;

logical_and_expression
	: equality_expression
	| logical_and_expression LOGICAL_AND equality_expression
	;

equality_expression
	: relational_expression
	| equality_expression EQUAL_TO relational_expression
	| equality_expression NEQ_TO relational_expression
	;
relational_expression
	: additive_expression
	| relational_expression '<' additive_expression
	| relational_expression '>' additive_expression
	| relational_expression LEQ_OP additive_expression
	| relational_expression GEQ_OP additive_expression
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression
	| additive_expression '-' multiplicative_expression
	;

multiplicative_expression
	: unary_expression
	| multiplicative_expression '*' unary_expression
	| multiplicative_expression '/' unary_expression
	;
unary_expression
	: postfix_expression
	| unary_operator postfix_expression
	;

postfix_expression
	: primary_expression
	| IDENTIFIER '(' ')'
	| IDENTIFIER '(' expression_list ')'
	| l_expression INCREMENT
	;

primary_expression
	: l_expression
	| INT_CONST
	| FP_CONST
        | STRING_LITERAL
	| '(' expression ')'
	;

l_expression
        : IDENTIFIER {std::cout << $$ << std::endl;}
        | l_expression '[' expression ']' 
        ;
expression_list
        : expression
        | expression_list ',' expression
	;

unary_operator
        : '-'
	| '!'
	;

selection_statement
        : IF '(' expression ')' statement ELSE statement
	;

iteration_statement
	: WHILE '(' expression ')' statement
	| FOR '(' assignment_statement expression ';' assignment ')' statement
	;

declaration_list
	: declaration
	| declaration_list declaration
	;

declaration
	: type_specifier declarator_list';'
	;

declarator_list
	: declarator
	| declarator_list ',' declarator
	;
