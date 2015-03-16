%scanner Scanner.h 
%scanner-token-function d_scanner.lex() 
%token TOK_FP_CONST TOK_INT_CONST TOK_VOID_KW TOK_INT_KW TOK_FLOAT_KW TOK_FOR_KW 
%token TOK_WHILE_KW TOK_IF_KW TOK_ELSE_KW TOK_RETURN_KW TOK_IDENTIFIER 
%token TOK_LEQ_OP TOK_GEQ_OP TOK_INCR_OP TOK_STR_CONST 
%token TOK_LAND_OP TOK_LOR_OP TOK_EQ_OP TOK_NEQ_OP 
%polymorphic   expAstPtr: ExpAst*;
	stmtAstPtr: StmtAst*;
	arrayRefPtr: ArrayRef*;
	opType: OpType;
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
| translation_unit function_definition
;

function_definition  : type_specifier
{
    
    // No errors can occur here
    
    // Begin new function
    _g_offset = 0;
    _g_funcTable.reset(); 
    _g_funcTable.setReturnType(_g_typeSpec); 
    _g_functionStartLno = _g_lineCount;
    _g_functionDecError = false;
    _g_varDecError = false;
    _g_stmtListError = false;
}
fun_declarator compound_statement
{
    // Function declaration was bad
    if(_g_functionDecError) 
    {
        cat::parse::fdecerror(_g_functionStartLno, _g_funcTable.getSignature());
    }
    
    // Set higher level error
    _g_semanticError = _g_semanticError || _g_functionDecError || _g_stmtListError;
    
}
;
type_specifier  : TOK_VOID_KW
{
    _g_typeSpec = TYPE_VOID;
    _g_width = 0;
}
| TOK_INT_KW
{
    _g_typeSpec = TYPE_INT;
    _g_width = 4;
}
| TOK_FLOAT_KW
{
    _g_typeSpec = TYPE_FLOAT;
    _g_width = 4;
}
;
fun_declarator  : TOK_IDENTIFIER '(' parameter_list ')'
{
    
    // Set the name. Params already set
    _g_funcTable.setName($1);
    _g_offset = -4; 
    
    if( _g_globalSymTable.existsSignature(_g_funcTable.getSignature()) )
    {
        // Same signature exists   
        cat::parse::fdecerror::funcdupsig(_g_lineCount, _g_funcTable.getSignature());
        _g_functionDecError = true;
    }
    
    // Add it to the function table if its declaration was correct
    if(!_g_functionDecError)
    {
      _g_globalSymTable.addFuncTable(_g_funcTable);
    }
    
}
| TOK_IDENTIFIER '(' ')'
{
    
    // Set the name of the function. 
    _g_funcTable.setName($1);
    _g_offset = -4; 
    
    if( _g_globalSymTable.existsSignature(_g_funcTable.getSignature()) )
    {
        cat::parse::fdecerror(_g_lineCount, _g_funcTable.getSignature());
        _g_functionDecError = true;
    }
    
    // Add if no errors found
    if(!_g_functionDecError) 
    {
      _g_globalSymTable.addFuncTable(_g_funcTable);
    }
    
}
;
parameter_list  : parameter_declaration  
| parameter_list ',' parameter_declaration   
;
parameter_declaration  : type_specifier declarator
{

	
  if(_g_typeSpec == TYPE_VOID){
    _g_declarationError = true;
    //cat::parse::fdecerror::voidtype(_g_lineCount, _g_currentId);
  }
  
  // Whether decl wrong or right semantically, store it
  _g_curVarType->setPrimitive(_g_typeSpec); // Innermost type of non primitive     
  VarDeclaration v;
  v.setDeclType(PARAM);
  v.setName(_g_currentId);
  v.setSize(_g_size);
  v.setOffset(_g_offset);
  v.setVarType(_g_varType);
  _g_funcTable.addParam(v);
  _g_offset -= _g_size;
  
  
  // Propagate error upwards
  _g_functionDecError = _g_functionDecError || _g_declarationError;  
  _g_declarationError = false; // Reset declaration error for next one
}
;
declarator  : TOK_IDENTIFIER
{
    if(_g_funcTable.existsSymbol($1))
    {
        _g_declarationError = true;
        cat::parse::declaratorerror::dupid(_g_lineCount, $1);
    }
    
    
    _g_currentId = $1;
    _g_varType = new VarType();
    _g_curVarType = _g_varType;
    _g_size = _g_width;
  
}
| declarator '[' TOK_INT_CONST ']' // Changed constant expr to INT_CONST
{
    if(stoi($3) == 0)
    {
        _g_declarationError = true;
        cat::parse::declaratorerror::emptyarray(_g_lineCount, _g_currentId);
    }
    _g_curVarType->setArray(stoi($3)); 
    _g_curVarType = _g_curVarType->getNestedVarType();
    _g_size *= (stoi($3));
}
;

constant_expression   : TOK_INT_CONST  
| TOK_FP_CONST   
;

compound_statement  : '{' '}'   
{
	_g_stmtListError = false;
}
| '{' statement_list '}'
{
	_g_stmtListError = ($2)->validAST();
}
| '{' declaration_list statement_list '}'
{
	_g_stmtListError = _g_varDecError || ($3)->validAST();
}
;
statement_list  : statement
{
    ($$) = new Block($1); // New list of statements  
    ($$)->validAST() = ($1)->validAST(); // Valid if each stmt is valid
}
| statement_list statement
{
    ((Block*)($1))->insert($2); // Insert into orig list  
    ($1)->validAST() = ($1)->validAST() && ($2)->validAST(); // Update validity  
    ($$) = ($1); // Set current list to longer list
}
;
statement  : '{' statement_list '}'
{
    ($$) = ($2);
}
| selection_statement
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
| TOK_RETURN_KW expression ';'
{
    ($$) = new Return( ($2) );
    
    bool retComp = retTypeCompatible(_g_funcTable.getReturnType(), ($2)->valType());
    ($$)->validAST() = ($2)->validAST() && retComp;
	
    if(!retComp)
    {
        cat::parse::stmterror::rettypeerror(_g_lineCount, _g_funcTable.getReturnType(), ($2)->valType());
    }
}
| expression ';' // Added this for void func calls
{
  ($$) = new ExpStmt($1);
  ($$)->validAST() = ($1)->validAST();
}
;
assignment_statement  : ';'
{
    ($$) = new Empty();
}
| l_expression '=' expression ';'
{
    $$ = new Ass($1, $3);
    
    bool comp = assTypeCompatible(($1)->valType(), ($3)->valType());
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::incompasstype(_g_lineCount, ($1)->valType(), ($3)->valType());
    }
}
;
expression  : logical_and_expression
{
    ($$) = ($1);
}
| expression TOK_LOR_OP logical_and_expression
{
    $$ = new BinaryOp($1, $3, OP_OR);
    
    bool comp = binOpTypeCompatible(($1)->valType(), ($3)->valType(), OP_OR);
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->valType() = TYPE_INT;
    
    if(!comp)
    {
        // Wrong type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->valType(), ($3)->valType(), OP_OR);
    }
}
;
logical_and_expression  : equality_expression
{
    $$ = $1;
}
| logical_and_expression TOK_LAND_OP equality_expression
{
    $$ = new BinaryOp($1, $3, OP_AND);
    
    bool comp = binOpTypeCompatible(($1)->valType(), ($3)->valType(), OP_AND);
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->valType() = TYPE_INT;
    
    if(!comp)
    {
        // Wrong  type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->valType(), ($3)->valType(), OP_AND);
    }
}
;
equality_expression  : relational_expression
{
    $$ = $1;
}
| equality_expression TOK_EQ_OP relational_expression
{
   $$ = new BinaryOp($1, $3, OP_EQ);
    
    bool comp = binOpTypeCompatible(($1)->valType(), ($3)->valType(), OP_EQ);
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->valType() = TYPE_INT;
    
    if(!comp)
    {
        // Wrong type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->valType(), ($3)->valType(), OP_EQ);
    }
}
| equality_expression TOK_NEQ_OP relational_expression
{
    $$ = new BinaryOp($1, $3, OP_NE);
    
    bool comp = binOpTypeCompatible(($1)->valType(), ($3)->valType(), OP_NE);
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->valType() = TYPE_INT;
    
    if(!comp)
    {
        // Wrong type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->valType(), ($3)->valType(), OP_NE);
    }
}
;
relational_expression  : additive_expression
{
    $$ = $1;
}
| relational_expression '<' additive_expression
{
    $$ = new BinaryOp($1, $3, OP_LT);
    
    bool comp = binOpTypeCompatible(($1)->valType(), ($3)->valType(), OP_LT);
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->valType() = TYPE_INT;
    
    if(!comp)
    {
        // Wrong type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->valType(), ($3)->valType(), OP_LT);
    }
}
| relational_expression '>' additive_expression
{
    $$ = new BinaryOp($1, $3, OP_GT);
    
    bool comp = binOpTypeCompatible(($1)->valType(), ($3)->valType(), OP_GT);
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->valType() = TYPE_INT;
    
    if(!comp)
    {
        // Wrong type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->valType(), ($3)->valType(), OP_GT);
    }
}
| relational_expression TOK_LEQ_OP additive_expression
{
    $$ = new BinaryOp($1, $3, OP_LE);
    
    bool comp = binOpTypeCompatible(($1)->valType(), ($3)->valType(), OP_LE);
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->valType() = TYPE_INT;
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->valType(), ($3)->valType(), OP_LE);
    }
}
| relational_expression TOK_GEQ_OP additive_expression
{
    $$ = new BinaryOp($1, $3, OP_GE);
    
    bool comp = binOpTypeCompatible(($1)->valType(), ($3)->valType(), OP_GE);
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->valType() = TYPE_INT;
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->valType(), ($3)->valType(), OP_GE);
    }
}
;
additive_expression   : multiplicative_expression
{
    $$ = $1;
}
| additive_expression '+' multiplicative_expression
{

    $$ = new BinaryOp($1, $3, OP_PLUS);
    
    bool comp = binOpTypeCompatible(($1)->valType(), ($3)->valType(), OP_PLUS);
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->valType() = getDominantType( ($1)->valType(), ($3)->valType()); 
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->valType(), ($3)->valType(), OP_PLUS);
    }
    
}
| additive_expression '-' multiplicative_expression
{
    $$ = new BinaryOp($1, $3, OP_MINUS);
    
    bool comp = binOpTypeCompatible(($1)->valType(), ($3)->valType(), OP_MINUS);
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->valType() = getDominantType( ($1)->valType(), ($3)->valType()); 
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->valType(), ($3)->valType(), OP_MINUS);
    }
}
;
multiplicative_expression  : unary_expression
{
    $$ = $1;
}
| multiplicative_expression '*' unary_expression
{
    $$ = new BinaryOp($1, $3, OP_MULT);
    
    bool comp = binOpTypeCompatible(($1)->valType(), ($3)->valType(), OP_MULT);
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->valType() = getDominantType( ($1)->valType(), ($3)->valType()); 
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->valType(), ($3)->valType(), OP_MULT);
    }
}
| multiplicative_expression '/' unary_expression
{
    $$ = new BinaryOp($1, $3, OP_DIV);
    
    bool comp = binOpTypeCompatible(($1)->valType(), ($3)->valType(), OP_DIV);
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->valType() = getDominantType( ($1)->valType(), ($3)->valType()); 
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->valType(), ($3)->valType(), OP_DIV);
    }
}
;
unary_expression  : postfix_expression
{
    $$ = $1;
}
| unary_operator postfix_expression
{

    $$ = new UnaryOp($2, $1);
    
    bool comp = unaryOpCompatible($1, ($2)->valType());
    
    ($$)->validAST() = ($2)->validAST() && comp;
    ($$)->valType() = comp? ($2)->valType() : TYPE_WEAK; 
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::invalidunop(_g_lineCount, $1, ($2)->valType());
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
    // No args fun call  
    ((FunCall*)($$))->setName($1);
    ($$)->valType() = TYPE_WEAK;
    
    FunctionSignature fsig($1, list<ValType>());
    
    if(_g_globalSymTable.existsSignature(fsig))
    {
        // Valid function call  
        ($$)->validAST() = true;
        ($$)->valType() = _g_globalSymTable.getFuncTable(fsig).getReturnType();
    }
    else
    {
        ($$)->validAST() = false;
        cat::parse::fdecerror::badfcall(_g_lineCount, fsig);
    }
}
| TOK_IDENTIFIER '(' expression_list ')'
{
    $$ = $3;
    ((FunCall*)($3))->setName($1);
    ($$)->valType() = TYPE_WEAK;
    
    FunctionSignature fsig($1, ((FunCall*)($3))->getArgTypeList());
    if(($3)->validAST() && _g_globalSymTable.existsSignature(fsig))
    {
        ($$)->validAST() = true;
        ($$)->valType() = _g_globalSymTable.getFuncTable(fsig).getReturnType();
    }
    else if(($3)->validAST())
    {
        ($$)->validAST() = false;
        cat::parse::fdecerror::badfcall(_g_lineCount, fsig);
    }
    
    
      
}
| l_expression TOK_INCR_OP
{


    $$ = new UnaryOp($1, OP_PP);
    
    bool comp = unaryOpCompatible(OP_PP, ($1)->valType());
    
    ($$)->validAST() = ($1)->validAST() && comp;
    ($$)->valType() = comp? ($1)->valType() : TYPE_WEAK; 
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::invalidunop(_g_lineCount, OP_PP, ($1)->valType());
    }
    
}
;
primary_expression  : l_expression
{
    $$ = $1;
}
| l_expression '=' expression // added this production
{
    
    $$ = new BinaryOp($1, $3, OP_ASSIGN);
    
    bool comp = binOpTypeCompatible(($1)->valType(), ($3)->valType(), OP_ASSIGN);
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->valType() =  ($1)->valType() ;
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->valType(), ($3)->valType(), OP_ASSIGN);
    }
    
    
}
| TOK_INT_CONST
{
    $$ = new IntConst(std::stoi($1));
    ($$)->valType() = TYPE_INT;
}
| TOK_FP_CONST
{
    $$ = new FloatConst(std::stof($1));
    ($$)->valType() = TYPE_FLOAT;
}
| TOK_STR_CONST
{
    $$ = new StringConst((std::string)$1);
    ($$)->valType() = TYPE_STR;
}
| '(' expression ')'
{
    $$ = $2;
}
;
l_expression  : TOK_IDENTIFIER
{
    $$ = new Identifier($1);
    // Check for this symbol in the local symbol table
    ($$)->valType() = TYPE_WEAK;
    
    if(_g_funcTable.existsSymbol($1))
    {
      ($$)->validAST() = true;
      
      if(_g_funcTable.getVar($1).varType->primitive)
      {
	($$)->valType() = _g_funcTable.getVar($1).varType->type;
      }
      else
      {
	($$)->valType() = TYPE_ARRAY;
	_g_curVarType = _g_funcTable.getVar($1).varType;
      }
    }
    else
    {
      ($$)->validAST() = false;
      cat::parse::stmterror::symbolnotfound(_g_lineCount, $1);
    }
}
| l_expression '[' expression ']'
{
    
    if(($1)->validAST())
    {
      $$ = new Index($1, $3);
      
      
      
      bool canIndex = !(_g_curVarType->primitive);
      ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && canIndex;
      
      if(!canIndex)
      {
	cat::parse::stmterror::arrayreferror(_g_lineCount, _g_currentId);
	($$)->valType() = TYPE_WEAK;
      }
      else
      {
	  _g_curVarType = _g_curVarType->getNestedVarType();
	  if(_g_curVarType->primitive)
	  {
	    ($$)->valType() = _g_curVarType->type;
	  }
	  else
	  {
	    ($$)->valType() = TYPE_ARRAY;
	  }
      }
    }
    else
    {
      ($$) = ($1);
    }
}
;
expression_list  : expression
{
    $$ = new FunCall($1);
    ($$)->validAST() = ($1)->validAST();
}
| expression_list ',' expression
{
    ((FunCall*)($1))->insert($3);
    ($1)->validAST() = ($1)->validAST() && ($3)->validAST();
    $$ = $1;
}
;
unary_operator  : '-'
{
    $$ = OP_UMINUS;
}
| '!'
{
    $$ = OP_NOT;
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
| TOK_FOR_KW '(' expression ';' expression ';' expression ')' statement //modified this production
{
    ($$) = new For( ($3), ($5), ($7), ($9));
}
;
declaration_list  : declaration    
| declaration_list declaration  
;

declaration  : type_specifier declarator_list';'  ;

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
    
    
    _g_varDecError = _g_varDecError || _g_declarationError;
    _g_declarationError = false;
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
    
    _g_varDecError = _g_varDecError || _g_declarationError;
    _g_declarationError = false;
}
;
