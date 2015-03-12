%scanner Scanner.h 
%scanner-token-function d_scanner.lex() 
%token TOK_FP_CONST TOK_INT_CONST TOK_VOID_KW TOK_INT_KW TOK_FLOAT_KW TOK_FOR_KW 
%token TOK_WHILE_KW TOK_IF_KW TOK_ELSE_KW TOK_RETURN_KW TOK_IDENTIFIER 
%token TOK_LEQ_OP TOK_GEQ_OP TOK_INCR_OP TOK_STR_CONST 
%token TOK_LAND_OP TOK_LOR_OP TOK_EQ_OP TOK_NEQ_OP 
%polymorphic   expAstPtr: ExpAst*;
	stmtAstPtr: StmtAst*;
	arrayRefPtr: ArrayRef*;
	opType: typeOp;
	idAttr: std::string;
%type <stmtAstPtr> assignment_statement statement
%type <stmtAstPtr> statement_list selection_statement iteration_statement 
%type <expAstPtr> expression logical_and_expression equality_expression 
%type <expAstPtr> relational_expression additive_expression 
%type <expAstPtr> multiplicative_expression unary_expression  
%type <expAstPtr> postfix_expression primary_expression expression_list 
%type <arrayRefPtr> l_expression  
%type <opType> unary_operator 
%type <idAttr> TOK_IDENTIFIER TOK_STR_CONST TOK_INT_CONST TOK_FP_CONST 
%% 

translation_unit  : function_definition
{
    // _g_globalSymTable.print();
}
| translation_unit function_definition
{
    // _g_globalSymTable.print();
}
;
function_definition  : type_specifier
{
    /*   * This part of the code will be reached at the beginning  * of a function definition (after reducing the type specifier).   * Start a new function table at this point.   * Assume all errors except glob errors are false  */    _g_offset = 0;
    _g_funcTable.reset();
    _g_funcTable.setReturnType(_g_typeSpec);
    _g_functionStartLno = _g_lineCount;
}
fun_declarator compound_statement
{
    /*  * After the function has been processed.   * _g_functionDefError will be true if there was an error.  */  if(!_g_functionDefError) // Function is correct
    {
        _g_funcTable.correctOffsets();
        // Fix offsets   _g_globalSymTable.addFuncTable(_g_funcTable);
        // Add to table
    }
    else // Function encountered error
    {
        cat::parse::fdeferror(_g_functionStartLno, _g_funcTable.getName());
        _g_semanticError = true;
        // Set semantic error
    }
    // Set function and declaration errors to false  _g_functionDefError = false;
    _g_declaratorError = false;
}
;
type_specifier  : TOK_VOID_KW
{
    _g_typeSpec = DECL_P_VOID;
    _g_width = 0;
}
| TOK_INT_KW
{
    _g_typeSpec = DECL_P_INT;
    _g_width = 4;
}
| TOK_FLOAT_KW
{
    _g_typeSpec = DECL_P_FLOAT;
    _g_width = 4;
}
;
fun_declarator  : TOK_IDENTIFIER '(' parameter_list ')'
{
    /* Function declarator.   * if parameter_list is wrong, _g_declaratorError is true  * Need to check for duplicate function signature.   * Wrong stuff means _g_functionDefError set to true  */    /*// Set the name, check for conflicting args.  if( _g_funcTable.existsSymbol($1) )
    {
        cat::parse::fdeferror::dupargid(_g_lineCount, $1);
        _g_functionDefError = true;
        _g_semanticError = true;
    }
    // Apparently no error is given by gcc */    if( _g_globalSymTable.existsSameSignature(_g_funcTable) )
    {
        // Same signature exists   cat::parse::fdeferror::funcdupsig(_g_lineCount, _g_funcTable.getSignature());
        _g_functionDefError = true;
        _g_semanticError = true;
    }
    // On success or failure, doesnt hurt to set the name  _g_funcTable.setName($1);
    _g_offset += 4;
    // Address bytes of machine for ebp
}
| TOK_IDENTIFIER '(' ')'
{
    if( _g_globalSymTable.existsSameSignature(_g_funcTable) )
    {
        cat::parse::fdeferror(_g_lineCount, _g_funcTable.getSignature());
        _g_functionDefError = true;
        _g_semanticError = true;
    }
    _g_funcTable.setName($1);
    _g_offset += 4;
    // Address bytes of machine for ebp
}
;
parameter_list  : parameter_declaration  | parameter_list ',' parameter_declaration   ;
parameter_declaration  : type_specifier declarator
{
    if(!_g_declaratorError)
    {
        _g_curVarType->setPrimitive(_g_typeSpec);
        // Innermost type of non primitive     VarDeclaration v;
        v.setDeclType(PARAM);
        v.setName(_g_currentId);
        v.setSize(_g_size);
        v.setOffset(_g_offset);
        v.setVarType(_g_varType);
        _g_funcTable.addParam(v);
        _g_offset += _g_size;
    }
    _g_declaratorError = false;
}
;
declarator  : TOK_IDENTIFIER
{
    if(_g_funcTable.existsSymbol($1))
    {
        _g_declaratorError = true;
        _g_funcDefError = true;
        _g_semanticError = true;
        cat::parser::declatorerror::dupid(_g_lineCount, $1);
    }
    else
    {
        _g_currentId = $1;
        _g_varType = new VarType();
        _g_varType   _g_curVarType = _g_varType;
        _g_size = _g_width;
    }
}
| declarator '[' TOK_INT_CONST ']' // Changed constant expr to INT_CONST
{
    if(std::stoi($3) == 0)
    {
        _g_declaratorError = true;
        cat::parser::declatorerror::emptyarray(_g_lineCount, _g_currentId);
    }
    if(!_g_declaratorError)
    {
        _g_curVarType->setArray(stoi($3));
        _g_curVarType->setNestedVarType(new VarType());
        _g_curVarType = _g_curVarType->getNestedVarType();
        _g_size *= (stoi($3));
    }
}
;
constant_expression   : TOK_INT_CONST  | TOK_FP_CONST   ;
compound_statement  : '
{
    ' '
}
'   | '
{
    ' statement_list '
}
'
{
    //($2)->print();
    //Uncomment to print the ADT  //std::cout <<'n';
}
| '
{
    ' declaration_list statement_list '
}
'
{
    //($3)->print();
    //Uncomment to print the ADT   //std::cout << 'n';
}
;
statement_list  : statement
{
    ($$) = new Block($1);
    // New list of statements  ($$).validAST = ($1).validAST;
    // Valid if each stmt is valid
}
| statement_list statement
{
    ((Block*)($1))->insert($2);
    // Insert into orig list  ($$).validAST = ($1).validAST() && ($2).validAST();
    // Update validity  ($$) = ($1);
    // Set current list to longer list
}
;
statement  : '
{
    ' statement_list '
}
'
{
    ($$) = ($2);
}
//a solution to the local decl problem  | selection_statement
{
    ($$) = ($1);
}
| iteration_statement
{
    ($$) = ($1);
}
| assignment_statement
{
    ($$) = ($1);
}
| TOK_RETURN_KW expression ';
'
{
    ($$) = new Return( ($2) );
    if( ($2).validAST() &&   retTypeCompatible(_g_funcTable.getReturnType(), ($2).valType())
    {
        // All ok  ($$).validAST() = true;
    }
    else if(($2).validAST())
    {
        // Return type doesnt match  ($$).validAST() = false;
        _g_funcDefError = true;
        _g_semanticError = true;
        cat::parse::stmterror::rettypeerror(_g_lineCount);
    }
    else
    {
        // Error already present in subexpr  ($$).validAST() = false;
    }
}
;
assignment_statement  : ';
'
{
    $$ = new Empty();
}
| l_expression '=' expression ';
'
{
    $$ = new Ass($1, $3);
    if( ($1).validAST() && ($2).validAST() &&   assTypeCompatible(($1).valType(), ($3).valType()))
    {
        // All ok  ($$).validAST() = true;
        ($$).valType() = ($1).valType();
    }
    else if(($1).validAST() && ($2).validAST())
    {
        // Wrong assignment type mismatch   ($$).setValidAST(false);
        _g_funcDefError = true;
        _g_semanticError = true;
        cat::parse::stmterror::incompasstype(_g_lineCount, ($1).valType(), ($3).valType());
    }
    else
    {
        ($$).setValidAST(false);
    }
}
;
expression  : logical_and_expression
{
    ($$) = ($1);
}
| expression TOK_LOR_OP logical_and_expression
{
    $$ = new BinaryOp($1, $3, OR);
    if( ($1).validAST() && ($2).validAST() &&   binOpTypeCompatible(($1).valType(), ($3).valType(), OR));
    {
        // All ok  ($$).validAST() = true;
        ($$).valType() = TYPE_INT;
    }
    else if(($1).validAST() && ($2).validAST())
    {
        // Wrong binOp type mismatch   ($$).validAST() = false;
        _g_funcDefError = true;
        _g_semanticError = true;
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1).valType(), ($3).valType(), OR);
    }
    else
    {
        ($$).validAST() = false;
    }
}
;
logical_and_expression  : equality_expression
{
    $$ = $1;
}
| logical_and_expression TOK_LAND_OP equality_expression
{
    $$ = new BinaryOp($1, $3, AND);
    if( ($1).validAST() && ($2).validAST() &&   binOpTypeCompatible(($1).valType(), ($3).valType(), AND));
    {
        // All ok  ($$).validAST() = true;
        ($$).valType() = TYPE_INT;
    }
    else if(($1).validAST() && ($2).validAST())
    {
        // Wrong binOp type mismatch   ($$).validAST() = false;
        _g_funcDefError = true;
        _g_semanticError = true;
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1).valType(), ($3).valType(), AND);
    }
    else
    {
        ($$).validAST() = false;
    }
}
;
equality_expression  : relational_expression
{
    $$ = $1;
}
| equality_expression TOK_EQ_OP relational_expression
{
    $$ = new BinaryOp($1, $3, EQ_OP);
    if( ($1).validAST() && ($2).validAST() &&   binOpTypeCompatible(($1).valType(), ($3).valType(), EQ_OP));
    {
        // All ok  ($$).validAST() = true;
        ($$).valType() = TYPE_INT;
    }
    else if(($1).validAST() && ($2).validAST())
    {
        // Wrong binOp type mismatch   ($$).validAST() = false;
        _g_funcDefError = true;
        _g_semanticError = true;
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1).valType(), ($3).valType(), EQ_OP);
    }
    else
    {
        ($$).validAST() = false;
    }
}
| equality_expression TOK_NEQ_OP relational_expression
{
    $$ = new BinaryOp($1, $3, NE_OP);
    if( ($1).validAST() && ($2).validAST() &&   binOpTypeCompatible(($1).valType(), ($3).valType(), NE_OP));
    {
        // All ok  ($$).validAST() = true;
        ($$).valType() = TYPE_INT;
    }
    else if(($1).validAST() && ($2).validAST())
    {
        // Wrong binOp type mismatch   ($$).validAST() = false;
        _g_funcDefError = true;
        _g_semanticError = true;
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1).valType(), ($3).valType(), NE_OP);
    }
    else
    {
        ($$).validAST() = false;
    }
}
;
relational_expression  : additive_expression
{
    $$ = $1;
}
| relational_expression '<' additive_expression
{
    $$ = new BinaryOp($1, $3, LT);
    if( ($1).validAST() && ($2).validAST() &&   binOpTypeCompatible(($1).valType(), ($3).valType(), LT));
    {
        // All ok  ($$).validAST() = true;
        ($$).valType() = TYPE_INT;
    }
    else if(($1).validAST() && ($2).validAST())
    {
        // Wrong binOp type mismatch   ($$).validAST() = false;
        _g_funcDefError = true;
        _g_semanticError = true;
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1).valType(), ($3).valType(), LT);
    }
    else
    {
        ($$).validAST() = false;
    }
}
| relational_expression '>' additive_expression
{
    $$ = new BinaryOp($1, $3, GT);
    if( ($1).validAST() && ($2).validAST() &&   binOpTypeCompatible(($1).valType(), ($3).valType(), GT));
    {
        // All ok  ($$).validAST() = true;
        ($$).valType() = TYPE_INT;
    }
    else if(($1).validAST() && ($2).validAST())
    {
        // Wrong binOp type mismatch   ($$).validAST() = false;
        _g_funcDefError = true;
        _g_semanticError = true;
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1).valType(), ($3).valType(), GT);
    }
    else
    {
        ($$).validAST() = false;
    }
}
| relational_expression TOK_LEQ_OP additive_expression
{
    $$ = new BinaryOp($1, $3, LE_OP);
    if( ($1).validAST() && ($2).validAST() &&   binOpTypeCompatible(($1).valType(), ($3).valType(), LE_OP));
    {
        // All ok  ($$).validAST() = true;
        ($$).valType() = TYPE_INT;
    }
    else if(($1).validAST() && ($2).validAST())
    {
        // Wrong binOp type mismatch   ($$).validAST() = false;
        _g_funcDefError = true;
        _g_semanticError = true;
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1).valType(), ($3).valType(), LE_OP);
    }
    else
    {
        ($$).validAST() = false;
    }
}
| relational_expression TOK_GEQ_OP additive_expression
{
    $$ = new BinaryOp($1, $3, GE_OP);
    if( ($1).validAST() && ($2).validAST() &&   binOpTypeCompatible(($1).valType(), ($3).valType(), GE_OP));
    {
        // All ok  ($$).validAST() = true;
        ($$).valType() = TYPE_INT;
    }
    else if(($1).validAST() && ($2).validAST())
    {
        // Wrong binOp type mismatch   ($$).validAST() = false;
        _g_funcDefError = true;
        _g_semanticError = true;
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1).valType(), ($3).valType(), GE_OP);
    }
    else
    {
        ($$).validAST() = false;
    }
}
;
additive_expression   : multiplicative_expression
{
    $$ = $1;
}
| additive_expression '+' multiplicative_expression
{
    $$ = new BinaryOp($1, $3, PLUS);
    if( ($1).validAST() && ($2).validAST() &&   binOpTypeCompatible(($1).valType(), ($3).valType(), PLUS));
    {
        // All ok  ($$).validAST() = true;
        if(($1).valType() == TYPE_FLOAT || ($3).valType() == TYPE_FLOAT)
        {
            ($$).valType() = TYPE_FLOAT;
        }
        else
        {
            ($$).valType() = TYPE_INT;
        }
    }
    else if(($1).validAST() && ($2).validAST())
    {
        // Wrong binOp type mismatch   ($$).validAST() = false;
        _g_funcDefError = true;
        _g_semanticError = true;
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1).valType(), ($3).valType(), PLUS);
    }
    else
    {
        ($$).validAST() = false;
    }
}
| additive_expression '-' multiplicative_expression
{
    $$ = new BinaryOp($1, $3, MINUS);
    if( ($1).validAST() && ($2).validAST() &&   binOpTypeCompatible(($1).valType(), ($3).valType(), MINUS));
    {
        // All ok  ($$).validAST() = true;
        if(($1).valType() == TYPE_FLOAT || ($3).valType() == TYPE_FLOAT)
        {
            ($$).valType() = TYPE_FLOAT;
        }
        else
        {
            ($$).valType() = TYPE_INT;
        }
    }
    else if(($1).validAST() && ($2).validAST())
    {
        // Wrong binOp type mismatch   ($$).validAST() = false;
        _g_funcDefError = true;
        _g_semanticError = true;
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1).valType(), ($3).valType(), MINUS);
    }
    else
    {
        ($$).validAST() = false;
    }
}
;
multiplicative_expression  : unary_expression
{
    $$ = $1;
}
| multiplicative_expression '*' unary_expression
{
    $$ = new BinaryOp($1, $3, MULT);
    if( ($1).validAST() && ($2).validAST() &&   binOpTypeCompatible(($1).valType(), ($3).valType(), MULT));
    {
        // All ok  ($$).validAST() = true;
        if(($1).valType() == TYPE_FLOAT || ($3).valType() == TYPE_FLOAT)
        {
            ($$).valType() = TYPE_FLOAT;
        }
        else
        {
            ($$).valType() = TYPE_INT;
        }
    }
    else if(($1).validAST() && ($2).validAST())
    {
        // Wrong binOp type mismatch   ($$).validAST() = false;
        _g_funcDefError = true;
        _g_semanticError = true;
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1).valType(), ($3).valType(), MULT);
    }
    else
    {
        ($$).validAST() = false;
    }
}
| multiplicative_expression '/' unary_expression
{
    $$ = new BinaryOp($1, $3, DIV);
    if( ($1).validAST() && ($2).validAST() &&   binOpTypeCompatible(($1).valType(), ($3).valType(), DIV));
    {
        // All ok  ($$).validAST() = true;
        if(($1).valType() == TYPE_FLOAT || ($3).valType() == TYPE_FLOAT)
        {
            ($$).valType() = TYPE_FLOAT;
        }
        else
        {
            ($$).valType() = TYPE_INT;
        }
    }
    else if(($1).validAST() && ($2).validAST())
    {
        // Wrong binOp type mismatch   ($$).validAST() = false;
        _g_funcDefError = true;
        _g_semanticError = true;
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1).valType(), ($3).valType(), DIV);
    }
    else
    {
        ($$).validAST() = false;
    }
}
;
unary_expression  : postfix_expression
{
    $$ = $1;
}
| unary_operator postfix_expression
{
    $$ = new UnaryOp($1, $2);
    if( ($2).validAST() &&   validUnaryOp( ($1) , ($2).valType()));
    {
        // All ok  ($$).validAST() = true;
        ($$).valType() = ($2).valType();
    }
    else if(($2).validAST())
    {
        // Wrong binOp type mismatch   ($$).validAST() = false;
        _g_funcDefError = true;
        _g_semanticError = true;
        cat::parse::stmterror::invalidunop(_g_lineCount, $1, ($2).valType());
    }
    else
    {
        ($$).validAST() = false;
    }
}
;
postfix_expression  : primary_expression
{
    $$ = $1;
}
| TOK_IDENTIFIER '(' ')'
{
    $$ = new FunCall(nullptr);
    // No args fun call  ((FunCall*)($$))->setName($1);
    if(_g_globalSymTable.existsFuncDefinition($1, list<ValType>())
    {
        // Valid function call  ($$).validAST() = true;
        ($$).valType() = _g_globalSymTable.getFuncTable($1, list<ValType>()).getReturnType();
    }
    else
    {
        _g_funcDefError = true;
        _g_semanticError = true;
        ($$).validAST() = false;
        cat::parse::fdeferror::badfcall(_g_lineCount, $1, list<ValType>());
    }
}
| TOK_IDENTIFIER '(' expression_list ')'
{
    $$ = $3;
    ((FunCall*)($3))->setName($1);
    if(($3).validAST() && _g_globalSymTable.existsFuncDefinition($1, ($3).getArgTypeList()))
    {
        ($$).validAST() = true;
        ($$).valType() = _g_globalSymTable.getFuncTable($1, list<ValType>()).getReturnType();
    }
    else if(($3).validAST())
    {
        _g_funcDefError = true;
        _g_semanticError = true;
        ($$).validAST() = false;
        cat::parse::fdeferror::badfcall(_g_lineCount, $1, ($3).getArgTypeList());
    }
    else
    {
        ($$).validAST() = false;
    }
}
| l_expression TOK_INCR_OP
{
    $$ = new UnaryOp($1, PP);
    if(($1).validAST() && (($$).valType() == TYPE_INT || ($$).valType() == TYPE_FLOAT))
    {
        ($$).validAST() = true;
        ($$).valType() = l_expression.valType();
    }
    else if(($1).validAST())
    {
        _g_funcDefError = true;
        _g_semanticError = true;
        ($$).validAST() = false;
        cat::parse::fdeferror::def(_g_lineCount,
    }
    else
    {
        ($$).validAST() = false;
    }
}
;
primary_expression  : l_expression
{
    $$ = $1;
}
| l_expression '=' expression // added this production
{
    $$ = new BinaryOp($1, $3, ASSIGN);
}
| TOK_INT_CONST
{
    $$ = new IntConst(std::stoi($1));
}
| TOK_FP_CONST
{
    $$ = new FloatConst(std::stof($1));
}
| TOK_STR_CONST
{
    $$ = new StringConst((std::string)$1);
}
| '(' expression ')'
{
    $$ = $2;
}
;
l_expression  : TOK_IDENTIFIER
{
    $$ = new Identifier($1);
}
| l_expression '[' expression ']'
{
    $$ = new Index($1, $3);
}
;
expression_list  : expression
{
    $$ = new FunCall($1);
}
| expression_list ',' expression
{
    ((FunCall*)($1))->insert($3);
    $$ = $1;
}
;
unary_operator  : '-'
{
    $$ = UMINUS;
}
| '!'
{
    $$ = NOT;
}
;
selection_statement  : TOK_IF_KW '(' expression ')' statement TOK_ELSE_KW statement
{
    ($$) = new If( ($3), ($5), ($7));
}
;
iteration_statement  : TOK_WHILE_KW '(' expression ')' statement
{
    ($$) = new While( ($3), ($5));
}
| TOK_FOR_KW '(' expression ';
' expression ';
' expression ')' statement //modified this production
{
    ($$) = new For( ($3), ($5), ($7), ($9));
}
;
declaration_list  : declaration    | declaration_list declaration  ;
declaration  : type_specifier declarator_list';
'  ;
declarator_list  : declarator
{
    _g_curVarType->setPrimitive(_g_typeSpec);
    VarDeclaration v;
    v.setDeclType(LOCAL);
    v.setName(_g_currentId);
    v.setSize(_g_size);
    v.setOffset(_g_offset);
    v.setVarType(_g_varType);
    _g_funcTable.addVar(v);
    _g_offset += _g_size;
}
| declarator_list ',' declarator
{
    _g_curVarType->setPrimitive(_g_typeSpec);
    VarDeclaration v;
    v.setDeclType(LOCAL);
    v.setName(_g_currentId);
    v.setSize(_g_size);
    v.setOffset(_g_offset);
    v.setVarType(_g_varType);
    _g_funcTable.addVar(v);
    _g_offset += _g_size;
}
;
