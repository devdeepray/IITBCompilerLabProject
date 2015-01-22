
%scanner Scanner.h
%scanner-token-function d_scanner.lex()

%token FP_CONST INT_CONST VOID INT FLOAT FOR WHILE IF ELSE RETURN IDENTIFIER
%token LEQ_OP GEQ_OP INCREMENT STRING_LITERAL
%token LOGICAL_AND LOGICAL_OR EQUAL_TO NEQ_TO


%%



translation_unit:
	function_definition 
	{
		makeGraph("tran_unit",1);
	}
	| translation_unit function_definition  
	{
		makeGraph("tran_unit",2);
	}
	
	;

function_definition:
	type_specifier fun_declarator compound_statement 
	{
		makeGraph("func_def",3);
	}
	;

type_specifier:
	VOID 
	{
		addTerm("void");
		makeGraph("type_spec",1);
	}
	| INT 
	{
		addTerm("int");
		makeGraph("type_spec",1);
	}
	| FLOAT 
	{
		addTerm("float");
		makeGraph("type_spec",1);
	}
	;

fun_declarator
	: IDENTIFIER '(' parameter_list ')' 
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("identifier");
		addTerm("left_paren");
		shiftReduceStack.push(param);
		addTerm("right_paren");
		makeGraph("fun_decl",4);	
	}
	| IDENTIFIER '(' ')' 
	{
		addTerm("identifier");
		addTerm("left_paren");
		addTerm("right_paren");
		makeGraph("fun_decl",3);
	}
	;

parameter_list
	: parameter_declaration 
	{ 
		makeGraph("param_list",1); 
	}
	| parameter_list ',' parameter_declaration
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("comma");
		shiftReduceStack.push(param);
		makeGraph("param_list",3);
	}
	;

parameter_declaration
	: type_specifier declarator 
	{
		makeGraph("param_decl",2);
	}
	;


declarator
	: IDENTIFIER 
	{ 
		addTerm("identifier"); 
		makeGraph("declarator",1);
	}
	| declarator '[' constant_expression ']'
	{
		std::string const_expr = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("left_box");
		shiftReduceStack.push(const_expr);
		addTerm("right_box");
		makeGraph("declarator", 4);
	}
	;

constant_expression
        : INT_CONST 
				{ 
					addTerm("int_const");
					makeGraph("const_expr",1);
				}
        | FP_CONST
				{ 
					addTerm("fp_const");
					makeGraph("const_expr",1);
				}
        ;

compound_statement
	: '{' '}' 
	{ 
		addTerm("left_curly"); 
		addTerm("right_curly");
		makeGraph("comp_stmt",2);
	}
	| '{' statement_list '}'
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("left_curly");
		shiftReduceStack.push(param);
		addTerm("right_curly");
		makeGraph("comp_stmt",3);
	}
  | '{' declaration_list statement_list '}'
	{
		
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		std::string param1 = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("left_curly");
		shiftReduceStack.push(param1);
		shiftReduceStack.push(param);
		addTerm("right_curly");
		makeGraph("comp_stmt",4);
	}
	;

statement_list
	: statement 
	{
		makeGraph("stmt_list",1);
	}
	| statement_list statement 
	{
		makeGraph("stmt_list",2);
	}
	;

statement
 	: compound_statement 
	{
		makeGraph("stmt",1);
	}
	| selection_statement 
	{
		makeGraph("stmt",1);
	}
	| iteration_statement 
	{
		makeGraph("stmt",1);
	}
	| assignment_statement 
	{
		makeGraph("stmt",1);
	}
	| RETURN expression ';'	
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("return");
		shiftReduceStack.push(param);
		addTerm("semi_colon");
		makeGraph("stmt",3);
	}
	;

assignment_statement
	: assignment ';' 
	{
		addTerm("semi_colon");
		makeGraph("assi_stmt",2);
	}
	;

assignment
	:
	|  l_expression '=' expression
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("equal_to");
		shiftReduceStack.push(param);
		makeGraph("assi",3);
	}
	;

expression
	: logical_and_expression 
	{
		makeGraph("expression",1);
	}
	| expression LOGICAL_OR logical_and_expression
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("logical_or");
		shiftReduceStack.push(param);
		makeGraph("expression",3);
	}
	;

logical_and_expression
	: equality_expression 
	{
		makeGraph("logic_and_expr",1);
	}
	| logical_and_expression LOGICAL_AND equality_expression
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("logical_and");
		shiftReduceStack.push(param);
		makeGraph("logic_and_expr",3);
	}
	;

equality_expression
	: relational_expression 
	{
		makeGraph("equal_expr",1);
	}
	| equality_expression EQUAL_TO relational_expression
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("equal_to");
		shiftReduceStack.push(param);
		makeGraph("equal_expr",3);
	}
	| equality_expression NEQ_TO relational_expression
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("nequal_to");
		shiftReduceStack.push(param);
		makeGraph("equal_expr",3);
	}
	;
relational_expression
	: additive_expression 
	{
		makeGraph("rel_expr",1);
	}
	| relational_expression '<' additive_expression
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("less_than");
		shiftReduceStack.push(param);
		makeGraph("rel_expr",3);
	}
	| relational_expression '>' additive_expression
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("greater_than");
		shiftReduceStack.push(param);
		makeGraph("rel_expr",3);
	}
	| relational_expression LEQ_OP additive_expression
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("le_to");
		shiftReduceStack.push(param);
		makeGraph("rel_expr",3);
	}
	| relational_expression GEQ_OP additive_expression
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("ge_to");
		shiftReduceStack.push(param);
		makeGraph("rel_expr",3);
	}
	;

additive_expression
	: multiplicative_expression 
	{
		makeGraph("add_expr",1);
	}
	| additive_expression '+' multiplicative_expression
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("plus");
		shiftReduceStack.push(param);
		makeGraph("add_expr",3);
	}
	| additive_expression '-' multiplicative_expression
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("minus");
		shiftReduceStack.push(param);
		makeGraph("add_expr",3);
	}
	;

multiplicative_expression
	: unary_expression 
	{
		makeGraph("mul_expr",1);
	}
	| multiplicative_expression '*' unary_expression
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("mul");
		shiftReduceStack.push(param);
		makeGraph("mul_expr",3);
	}
	| multiplicative_expression '/' unary_expression
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("div");
		shiftReduceStack.push(param);
		makeGraph("mul_expr",3);
	}
	;
unary_expression
	: postfix_expression 
	{
		makeGraph("unary_expr",1);
	}
	| unary_operator postfix_expression 
	{
		makeGraph("unary_expr",2);
	}
	;

postfix_expression
	: primary_expression 
	{
		makeGraph("post_expr",1);
	}
	| IDENTIFIER '(' ')' 
	{
		addTerm("identifier");
		addTerm("left_paran");
		addTerm("right_paran");
		makeGraph("post_expr",3);
	}
	| IDENTIFIER '(' expression_list ')'
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("identifier");
		addTerm("left_paran");
		shiftReduceStack.push(param);
		addTerm("right_paran");
		makeGraph("post_expr",4);
	}
	| l_expression INCREMENT 
	{
		addTerm("increment");
		makeGraph("post_expr",2);
	}
	;

primary_expression
	: l_expression 
	{
		makeGraph("prim_expr",1);
	}
	| INT_CONST 
	{
		addTerm("int_const");
		makeGraph("prim_expr",1);
	}
	| FP_CONST 
	{
		addTerm("fp_const");makeGraph("prim_expr",1);
	}
	| STRING_LITERAL 
	{
		addTerm("string");makeGraph("prim_expr",1);
	}
	| '(' expression ')'
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("left_paran");
		shiftReduceStack.push(param);
		addTerm("right_paran");
		makeGraph("prim_expr",3);
	}
	;

l_expression
	: IDENTIFIER 
	{
		addTerm("identifier");
		makeGraph("l_expr",1);
	}
	| l_expression '[' expression ']'
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("left_box");
		shiftReduceStack.push(param);
		addTerm("right_box");
		makeGraph("l_expr",3);
	} 
        ;
expression_list
	: expression 
	{
		makeGraph("expr_list",1);
	}
	| expression_list ',' expression
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("comma");
		shiftReduceStack.push(param);
		makeGraph("expr_list",3);
	} 
	;

unary_operator
	: '-'  
	{
		addTerm("minus");
		makeGraph("unary_op",1);
	}
	| '!'  
	{
		addTerm("exclamation");
		makeGraph("unary_op",1);
	}
	;

selection_statement
	: IF '(' expression ')' statement ELSE statement
	{
		std::string stat1 = shiftReduceStack.top();
		shiftReduceStack.pop();
		std::string stat2 = shiftReduceStack.top();
		shiftReduceStack.pop();
		std::string expr = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("if");
		addTerm("left_paren");
		addTerm(expr);
		addTerm("right_paren");
		addTerm(stat2);
		addTerm("else");
		addTerm(stat1);
		makeGraph("sel_stat", 7);
	}
	;

iteration_statement
	: WHILE '(' expression ')' statement
	{
		std::string stat = shiftReduceStack.top();
		shiftReduceStack.pop();
		std::string expr = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("while");
		addTerm("left_paren");
		addTerm(expr);
		addTerm("right_paren");
		addTerm(stat);
		makeGraph("iter_stat", 5);
		
	}
	| FOR '(' assignment_statement expression ';' assignment ')' statement
	{
		std::string d1 = shiftReduceStack.top();
		shiftReduceStack.pop();
		std::string d2 = shiftReduceStack.top();
		shiftReduceStack.pop();
		std::string d3 = shiftReduceStack.top();
		shiftReduceStack.pop();
		std::string d4 = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("for");
		addTerm("left_paren");
		addTerm(d4);
		addTerm(d3);
		addTerm("semi_colon");
		addTerm(d2);
		addTerm("right_paren");
		addTerm(d1);
		makeGraph("iter_stat", 8);
	}
	;

declaration_list
	: declaration 
	{
		makeGraph("declaration_list",1);
	}
	| declaration_list declaration 
	{
		makeGraph("declaration_list",2);
	}
	;

declaration
	: type_specifier declarator_list';'
	{
		addTerm("semi_colon");
		makeGraph("declaration",3);
	}
	;

declarator_list
	: declarator 
	{
		makeGraph("declarator_list",1);
	}
	| declarator_list ',' declarator
	{
		std::string param = shiftReduceStack.top();
		shiftReduceStack.pop();
		addTerm("comma");
		shiftReduceStack.push(param);
		makeGraph("declarator_list",3);
	}
	;
