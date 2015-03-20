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
%type <expAstPtr> relational_expression additive_expression constant_expression
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
    _g_offset = 4;
    _g_funcTable.reset(); 
    _g_funcTable.setReturnType(_g_varType); 
    _g_functionStartLno = _g_lineCount;
    _g_varType.reset();
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
    else 
    {
	_g_globalSymTable.updateFuncTable(_g_funcTable);
    }
    
    // Set higher level error
    _g_semanticError = _g_semanticError || _g_functionDecError || _g_stmtListError;
    
}
;
type_specifier  : TOK_VOID_KW
{
    _g_varType.reset();
    _g_varType.setPrimitive(TYPE_VOID);
    _g_width = 0;
}
| TOK_INT_KW
{
    _g_varType.reset();
    _g_varType.setPrimitive(TYPE_INT);
    _g_width = 4;
}
| TOK_FLOAT_KW
{
    _g_varType.reset();
    _g_varType.setPrimitive(TYPE_FLOAT);
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

	
  if(_g_varType.getPrimitiveType() == TYPE_VOID){
    _g_declarationError = true;
    cat::parse::fdecerror::voidtype(_g_lineCount, _g_currentId);
  }
  
  VarDeclaration v;
  v.setDeclType(PARAM);
  v.setName(_g_currentId);
  v.setSize(_g_size);
  v.setOffset(_g_offset);
  v.setDataType(_g_varType);
  _g_funcTable.addParam(v);
  if(_g_varType.isPrimitive())_g_offset += _g_size;
  else _g_offset += SIZE_OF_PTR;
  
  
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
    _g_varType.reset();
    _g_size = _g_width;
}
| declarator '[' constant_expression ']' // Changed constant expr to INT_CONST
{
    
    if(($3)->astnode_type == AST_FLOAT)
    {
	
        _g_declarationError = true;
        cat::parse::declaratorerror::baddimarray(_g_lineCount, _g_currentId);
    }
    else if(((IntConst*)($3))->val == 0)
    {
    
	_g_declarationError = true;
        cat::parse::declaratorerror::baddimarray(_g_lineCount, _g_currentId);
    }
    int size = 0;
    if(!_g_declarationError)
    {
      size = ((IntConst*)($3))->val ;
    }
    _g_varType.addDimension(size);
    _g_size *= (size);
}
;

constant_expression   : TOK_INT_CONST
{
  ($$) = new IntConst(stoi($1));
}
| TOK_FP_CONST   
{
  ($$) = new FloatConst(stof($1));
  cout << "tit" << endl;
}
;

compound_statement  : '{' '}'   
{
	_g_stmtListError = false;
}
| '{' statement_list '}'
{
   // ($2)->print();
	_g_stmtListError = ($2)->validAST();
}
| '{' declaration_list statement_list '}'
{
   // ($3)->print();
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
    ExpAst* tmp = $2;
    DataType retType = _g_funcTable.getReturnType();
    bool retComp = castTypeCompatible(retType, ($2)->dataType());
   
    if(retType !=  ($2)->dataType() && retComp)
    {
   
      if(retType.getPrimitiveType() == TYPE_FLOAT)
      {
	    tmp = new UnaryOp(($2), OP_TOFLT);
      }
      else if(retType.getPrimitiveType() == TYPE_INT)
      {
      	tmp = new UnaryOp(($2), OP_TOINT);
      }
      else if(retType.getPrimitiveType() != TYPE_WEAK)
      {
	    cerr << "Bug in parser, should never come here"; 
      }
      tmp->validAST() = ($2)->validAST();
    }
    ($$) = new Return( tmp );
    ($$)->validAST() = ($2)->validAST() && retComp;

    if(!retComp)
    {
        cat::parse::stmterror::rettypeerror(_g_lineCount, _g_funcTable.getReturnType(), ($2)->dataType());
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
    
    bool comp = castTypeCompatible(($1)->dataType(), ($3)->dataType());
    ExpAst* tmp;
    tmp = $3;
    if((($1)->dataType() != ($3)->dataType()) && comp)
    {
      if(($1)->dataType().getPrimitiveType() == TYPE_FLOAT)
      {
	    tmp = new UnaryOp(($3), OP_TOFLT);
	    tmp->dataType().setPrimitive(TYPE_FLOAT);
      }
      else if(($1)->dataType().getPrimitiveType() == TYPE_INT)
      {
	    tmp = new UnaryOp(($3), OP_TOINT);
	    tmp->dataType().setPrimitive(TYPE_INT);
      }
      else if(($1)->dataType().getPrimitiveType() != TYPE_WEAK)
      {
	    cerr << "Bug in parser, should never come here"; 
      }
      tmp->validAST() = ($3)->validAST();
    }
    $$ = new Ass($1, tmp);
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::incompasstype(_g_lineCount, ($1)->dataType(), ($3)->dataType());
    }
}
;
expression  : logical_and_expression
{
    ($$) = ($1);
}
| expression TOK_LOR_OP logical_and_expression
{
    bool comp = binOpTypeCompatible(($1)->dataType(), ($3)->dataType(), OP_OR);
    
    $$ = new BinaryOp($1, $3, OP_OR);
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->dataType().setPrimitive( TYPE_INT );
    
    if(!comp)
    {
        // Wrong type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->dataType(), ($3)->dataType(), OP_OR);
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
    
    bool comp = binOpTypeCompatible(($1)->dataType(), ($3)->dataType(), OP_AND);
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->dataType().setPrimitive( TYPE_INT );
    
    if(!comp)
    {
        // Wrong  type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->dataType(), ($3)->dataType(), OP_AND);
    }
}
;
equality_expression  : relational_expression
{
    $$ = $1;
}
| equality_expression TOK_EQ_OP relational_expression
{
   
    
    bool comp = binOpTypeCompatible(($1)->dataType(), ($3)->dataType(), OP_EQ);
    DataType tmp = getDominantType(($1)->dataType(), ($3)->dataType());
    ExpAst* tmp1 = $1, *tmp2 = $3;
    
    if(tmp.getPrimitiveType() == TYPE_FLOAT)
    {
	
	if(($1)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp1 = new UnaryOp($1, OP_TOFLT);
	  tmp1->validAST() = ($1)->validAST();
	}
	
	if(($3)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp2 = new UnaryOp($3, OP_TOFLT);
	  tmp2->validAST() = ($3)->validAST();
	}
	
	$$ = new BinaryOp(tmp1, tmp2, OP_FLOAT_EQ);
    }
    else
    {
      $$ = new BinaryOp($1, $3, OP_INT_EQ);
    }
   
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->dataType().setPrimitive( TYPE_INT );
    
    if(!comp)
    {
        // Wrong type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->dataType(), ($3)->dataType(), OP_EQ);
    }
}
| equality_expression TOK_NEQ_OP relational_expression
{
   bool comp = binOpTypeCompatible(($1)->dataType(), ($3)->dataType(), OP_NE);
    DataType tmp = getDominantType(($1)->dataType(), ($3)->dataType());
    ExpAst* tmp1 = $1, *tmp2 = $3;
    
    if(tmp.getPrimitiveType() == TYPE_FLOAT)
    {
	
	if(($1)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp1 = new UnaryOp($1, OP_TOFLT);
	  tmp1->validAST() = ($1)->validAST();
	}
	
	if(($3)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp2 = new UnaryOp($3, OP_TOFLT);
	  tmp2->validAST() = ($3)->validAST();
	}
	
	$$ = new BinaryOp(tmp1, tmp2, OP_FLOAT_NE);
    }
    else
    {
      $$ = new BinaryOp($1, $3, OP_INT_NE);
    }
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->dataType().setPrimitive( TYPE_INT );
    
    if(!comp)
    {
        // Wrong type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->dataType(), ($3)->dataType(), OP_NE);
    }
}
;
relational_expression  : additive_expression
{
    $$ = $1;
}
| relational_expression '<' additive_expression
{
    bool comp = binOpTypeCompatible(($1)->dataType(), ($3)->dataType(), OP_LT);
    DataType tmp = getDominantType(($1)->dataType(), ($3)->dataType());
    ExpAst* tmp1 = $1, *tmp2 = $3;
    
    if(tmp.getPrimitiveType() == TYPE_FLOAT)
    {
	
	if(($1)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp1 = new UnaryOp($1, OP_TOFLT);
	  tmp1->validAST() = ($1)->validAST();
	}
	
	if(($3)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp2 = new UnaryOp($3, OP_TOFLT);
	  tmp2->validAST() = ($3)->validAST();
	}
	
	$$ = new BinaryOp(tmp1, tmp2, OP_FLOAT_LT);
    }
    else
    {
      $$ = new BinaryOp($1, $3, OP_INT_LT);
    }
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->dataType().setPrimitive( TYPE_INT );
    
    if(!comp)
    {
        // Wrong type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->dataType(), ($3)->dataType(), OP_LT);
    }
}
| relational_expression '>' additive_expression
{
    bool comp = binOpTypeCompatible(($1)->dataType(), ($3)->dataType(), OP_GT);
    DataType tmp = getDominantType(($1)->dataType(), ($3)->dataType());
    ExpAst* tmp1 = $1, *tmp2 = $3;
    
    if(tmp.getPrimitiveType() == TYPE_FLOAT)
    {
	
	if(($1)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp1 = new UnaryOp($1, OP_TOFLT);
	  tmp1->validAST() = ($1)->validAST();
	}
	
	if(($3)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp2 = new UnaryOp($3, OP_TOFLT);
	  tmp2->validAST() = ($3)->validAST();
	}
	
	$$ = new BinaryOp(tmp1, tmp2, OP_FLOAT_GT);
    }
    else
    {
      $$ = new BinaryOp($1, $3, OP_INT_GT);
    }
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->dataType().setPrimitive( TYPE_INT );
    
    if(!comp)
    {
        // Wrong type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->dataType(), ($3)->dataType(), OP_GT);
    }
}
| relational_expression TOK_LEQ_OP additive_expression
{
   bool comp = binOpTypeCompatible(($1)->dataType(), ($3)->dataType(), OP_LE);
    DataType tmp = getDominantType(($1)->dataType(), ($3)->dataType());
    ExpAst* tmp1 = $1, *tmp2 = $3;
    
    if(tmp.getPrimitiveType() == TYPE_FLOAT)
    {
	
	if(($1)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp1 = new UnaryOp($1, OP_TOFLT);
	  tmp1->validAST() = ($1)->validAST();
	}
	
	if(($3)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp2 = new UnaryOp($3, OP_TOFLT);
	  tmp2->validAST() = ($3)->validAST();
	}
	
	$$ = new BinaryOp(tmp1, tmp2, OP_FLOAT_LE);
    }
    else
    {
      $$ = new BinaryOp($1, $3, OP_INT_LE);
    }
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->dataType().setPrimitive( TYPE_INT );
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->dataType(), ($3)->dataType(), OP_LE);
    }
}
| relational_expression TOK_GEQ_OP additive_expression
{
   bool comp = binOpTypeCompatible(($1)->dataType(), ($3)->dataType(), OP_GE);
    DataType tmp = getDominantType(($1)->dataType(), ($3)->dataType());
    ExpAst* tmp1 = $1, *tmp2 = $3;
    
    if(tmp.getPrimitiveType() == TYPE_FLOAT)
    {
	
	if(($1)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp1 = new UnaryOp($1, OP_TOFLT);
	  tmp1->validAST() = ($1)->validAST();
	}
	
	if(($3)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp2 = new UnaryOp($3, OP_TOFLT);
	  tmp2->validAST() = ($3)->validAST();
	}
	
	$$ = new BinaryOp(tmp1, tmp2, OP_FLOAT_GE);
    }
    else
    {
      $$ = new BinaryOp($1, $3, OP_INT_GE);
    }
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->dataType().setPrimitive( TYPE_INT );
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->dataType(), ($3)->dataType(), OP_GE);
    }
}
;
additive_expression   : multiplicative_expression
{
    $$ = $1;
}
| additive_expression '+' multiplicative_expression
{

    bool comp = binOpTypeCompatible(($1)->dataType(), ($3)->dataType(), OP_PLUS);
    DataType tmp = getDominantType(($1)->dataType(), ($3)->dataType());
    ExpAst* tmp1 = $1, *tmp2 = $3;
    
    if(tmp.getPrimitiveType() == TYPE_FLOAT)
    {
	
	if(($1)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp1 = new UnaryOp($1, OP_TOFLT);
	  tmp1->validAST() = ($1)->validAST();
	}
	
	if(($3)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp2 = new UnaryOp($3, OP_TOFLT);
	  tmp2->validAST() = ($3)->validAST();
	}
	
	$$ = new BinaryOp(tmp1, tmp2, OP_FLOAT_PLUS);
    }
    else
    {
      $$ = new BinaryOp($1, $3, OP_INT_PLUS);
    }
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->dataType() = getDominantType( ($1)->dataType(), ($3)->dataType());
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->dataType(), ($3)->dataType(), OP_PLUS);
    }
    
}
| additive_expression '-' multiplicative_expression
{
    bool comp = binOpTypeCompatible(($1)->dataType(), ($3)->dataType(), OP_MINUS);
    DataType tmp = getDominantType(($1)->dataType(), ($3)->dataType());
    ExpAst* tmp1 = $1, *tmp2 = $3;
    
    if(tmp.getPrimitiveType() == TYPE_FLOAT)
    {
	
	if(($1)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp1 = new UnaryOp($1, OP_TOFLT);
	  tmp1->validAST() = ($1)->validAST();
	}
	
	if(($3)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp2 = new UnaryOp($3, OP_TOFLT);
	  tmp2->validAST() = ($3)->validAST();
	}
	
	$$ = new BinaryOp(tmp1, tmp2, OP_FLOAT_MINUS);
    }
    else
    {
      $$ = new BinaryOp($1, $3, OP_INT_MINUS);
    }
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->dataType() = getDominantType( ($1)->dataType(), ($3)->dataType());
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->dataType(), ($3)->dataType(), OP_MINUS);
    }
}
;
multiplicative_expression  : unary_expression
{
    $$ = $1;
}
| multiplicative_expression '*' unary_expression
{
    bool comp = binOpTypeCompatible(($1)->dataType(), ($3)->dataType(), OP_MULT);
    DataType tmp = getDominantType(($1)->dataType(), ($3)->dataType());
    ExpAst* tmp1 = $1, *tmp2 = $3;
    
    if(tmp.getPrimitiveType() == TYPE_FLOAT)
    {
	
	if(($1)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp1 = new UnaryOp($1, OP_TOFLT);
	  tmp1->validAST() = ($1)->validAST();
	}
	
	if(($3)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp2 = new UnaryOp($3, OP_TOFLT);
	  tmp2->validAST() = ($3)->validAST();
	}
	
	$$ = new BinaryOp(tmp1, tmp2, OP_FLOAT_MULT);
    }
    else
    {
      $$ = new BinaryOp($1, $3, OP_INT_MULT);
    }
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->dataType() = getDominantType( ($1)->dataType(), ($3)->dataType());
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->dataType(), ($3)->dataType(), OP_MULT);
    }
}
| multiplicative_expression '/' unary_expression
{
    bool comp = binOpTypeCompatible(($1)->dataType(), ($3)->dataType(), OP_DIV);
    DataType tmp = getDominantType(($1)->dataType(), ($3)->dataType());
    ExpAst* tmp1 = $1, *tmp2 = $3;
    
    if(tmp.getPrimitiveType() == TYPE_FLOAT)
    {
	
	if(($1)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp1 = new UnaryOp($1, OP_TOFLT);
	  tmp1->validAST() = ($1)->validAST();
	}
	
	if(($3)->dataType().getPrimitiveType() == TYPE_INT)
	{
	  tmp2 = new UnaryOp($3, OP_TOFLT);
	  tmp2->validAST() = ($3)->validAST();
	}
	
	$$ = new BinaryOp(tmp1, tmp2, OP_FLOAT_DIV);
    }
    else
    {
      $$ = new BinaryOp($1, $3, OP_INT_DIV);
    }
    
    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->dataType() = getDominantType( ($1)->dataType(), ($3)->dataType());
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->dataType(), ($3)->dataType(), OP_DIV);
    }
}
;
unary_expression  : postfix_expression
{
    $$ = $1;
}
| unary_operator postfix_expression
{

    bool comp = unaryOpCompatible($1, ($2)->dataType());
    
    
    if($1 == OP_UMINUS)
    {
      if(($2)->dataType().getPrimitiveType() == TYPE_FLOAT)
      {
	$$ = new UnaryOp($2, OP_FLOAT_UMINUS);
      }
      else
      {
	$$ = new UnaryOp($2, OP_INT_UMINUS);
      }
    }
    else // NOT case
    {
      $$ = new UnaryOp($2, OP_NOT);
    }
	
    
    
    
    ($$)->validAST() = ($2)->validAST() && comp;
    ($$)->dataType() = comp? ($2)->dataType() : DataType(TYPE_WEAK);
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::invalidunop(_g_lineCount, $1, ($2)->dataType());
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
    ($$)->dataType().setPrimitive( TYPE_WEAK);
    
    FunctionSignature fsig($1, list<DataType>());
    
    if(_g_globalSymTable.existsSignature(fsig))
    {
        // Valid function call  
        ($$)->validAST() = true;
        ($$)->dataType() = _g_globalSymTable.getFuncTable(fsig).getReturnType();
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
    ($$)->dataType().setPrimitive( TYPE_WEAK );
    
    FunctionSignature fsig($1, ((FunCall*)($3))->getArgTypeList());
    if(($3)->validAST() && _g_globalSymTable.existsSignature(fsig))
    {
        ($$)->validAST() = true;
        ($$)->dataType() = _g_globalSymTable.getFuncTable(fsig).getReturnType();
    }
    else if(($3)->validAST())
    {
	int wmcount;
	FunctionSignature tmpsig = getCompatibleSignature(fsig, &wmcount);
	if(wmcount == 0)
	{
	    ($$)->validAST() = false;
	    cat::parse::fdecerror::badfcall(_g_lineCount, fsig);
	}
	else if(wmcount > 1)
	{
	    ($$)->validAST() = false;
	    cat::parse::fdecerror:ambiguousfcall(_g_lineCount, fsig);
	}
	else
	{
	    ($$)->validAST() = true;
	    
	    auto it1 = tmpsig.arg_types.begin();
	    auto it2 = ($$)->list_exp_ast.begin();
	    for(; it1 != tmpsig.arg_types.end(); ++it1, ++it2)
	    {
		if((*it2)->dataType().isPrimitive()
		    && (*it2)->dataType().getPrimitiveType() != it1->getPrimitiveType())
		{
		    if(it1->getPrimitiveType() == TYPE_FLOAT)
		    {
			ExpAst* tmp = new UnaryOp((*it2), OP_TOFLT);
			tmp->validAST() = (*it2)->validAST();
			(*it2) = tmp;
		    }
		    else
		    {
			
			ExpAst* tmp = new UnaryOp((*it2), OP_TOINT);
			tmp->validAST() = (*it2)->validAST();
			(*it2) = tmp;
		    }
		}
	    }
	}
        
    }
    
    
      
}
| l_expression TOK_INCR_OP
{


    $$ = new UnaryOp($1, OP_PP);
    
    bool comp = unaryOpCompatible(OP_PP, ($1)->dataType());
    
    ($$)->validAST() = ($1)->validAST() && comp;
    ($$)->dataType() = comp? ($1)->dataType() : TYPE_WEAK;
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::invalidunop(_g_lineCount, OP_PP, ($1)->dataType());
    }
    
}
;
primary_expression  : l_expression
{
    $$ = $1;
}
| l_expression '=' expression // added this production
{
    bool comp = binOpTypeCompatible(($1)->dataType(), ($3)->dataType(), OP_ASSIGN);
    ExpAst* tmp;
    tmp = $3;
    
    if((($1)->dataType() != ($3)->dataType()) && comp)
    {
      if(($1)->dataType().getPrimitiveType() == TYPE_FLOAT)
      {
        tmp = new UnaryOp(($3), OP_TOFLT);
      }
      else if(($1)->dataType().getPrimitiveType() == TYPE_INT)
      {
        tmp = new UnaryOp(($3), OP_TOINT);
      }
      else if(($1)->dataType().getPrimitiveType() != TYPE_WEAK)
      {
        cerr << "Bug in parser, should never come here";
      }
      tmp->validAST() = ($3)->validAST();
    }

    $$ = new BinaryOp($1, tmp, OP_ASSIGN);

    ($$)->validAST() = ($1)->validAST() && ($3)->validAST() && comp;
    ($$)->dataType() =  ($1)->dataType() ;
    
    if(!comp)
    {
        // Wrong assignment type mismatch   
        cat::parse::stmterror::incompboptype(_g_lineCount, ($1)->dataType(), ($3)->dataType(), OP_ASSIGN);
    }
    
    
}
| TOK_INT_CONST
{
    $$ = new IntConst(std::stoi($1));
    ($$)->dataType().setPrimitive( TYPE_INT );
}
| TOK_FP_CONST
{
    $$ = new FloatConst(std::stof($1));
    ($$)->dataType().setPrimitive( TYPE_FLOAT );
}
| TOK_STR_CONST
{
    $$ = new StringConst((std::string)$1);
    ($$)->dataType().setPrimitive( TYPE_STR );
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
    ($$)->dataType().setPrimitive( TYPE_WEAK );
    
    if(_g_funcTable.existsSymbol($1))
    {
      ($$)->validAST() = true;
      
      ($$)->dataType() = _g_funcTable.getVar($1).data_type;
      
      
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
      
      
      
      
      Index* tmp = new Index($1, $3);
      
      
      bool canIndex = !(($1)->dataType().isPrimitive()) && ($3)->dataType().isPrimitive() 
	    &&($3)->dataType().getPrimitiveType() == TYPE_INT;
      tmp->validAST() = ($1)->validAST() && ($3)->validAST() && canIndex;
      
      if(!canIndex)
      {
        
	    cat::parse::stmterror::arrayreferror(_g_lineCount, ((Index*)tmp)->getArrayName());
            (tmp)->dataType().setPrimitive( TYPE_WEAK );
      }
      else
      {
          
          (tmp)->dataType() = ($1)->dataType();
          (tmp)->dataType().removeDimension();
      }
      $$ = tmp;
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
    ($$)->validAST() = ($3)->validAST() && ($5)->validAST() && ($7)->validAST();
    if(!(($3)->dataType().getPrimitiveType() == TYPE_INT || ($3)->dataType().getPrimitiveType() == TYPE_FLOAT) 
      || !(($3)->dataType().isPrimitive()))
    {
        cat::parse::stmterror::ifexprerror(_g_lineCount);
        ($$)->validAST() = false;
    }
}
;
iteration_statement  : TOK_WHILE_KW '(' expression ')' statement
{
    ($$) = new While( ($3), ($5));
    ($$)->validAST() = ($3)->validAST() && ($5)->validAST();
    if(!(($3)->dataType().getPrimitiveType() == TYPE_INT || ($3)->dataType().getPrimitiveType() == TYPE_FLOAT)
    || !(($3)->dataType().isPrimitive()))
    {
        cat::parse::stmterror::whileexprerror(_g_lineCount);
        ($$)->validAST() = false;
    }
}
| TOK_FOR_KW '(' expression ';' expression ';' expression ')' statement //modified this production
{
    ($$) = new For( ($3), ($5), ($7), ($9));
    ($$)->validAST() = ($3)->validAST() && ($5)->validAST() && ($7)->validAST() && ($9)->validAST();
    if(!(($5)->dataType().getPrimitiveType() == TYPE_INT || ($5)->dataType().getPrimitiveType() == TYPE_FLOAT)
    || !(($5)->dataType().isPrimitive()))
    {
        cat::parse::stmterror::forexprerror(_g_lineCount);
        ($$)->validAST() = false;
    }
}
;
declaration_list  : declaration    
| declaration_list declaration  
;

declaration  : type_specifier declarator_list';'  ;

declarator_list  : declarator
{
     
    
    if(!_g_declarationError)
    {
            
	    VarDeclaration v;
	    _g_varType.clearDimensions();
	    v.setDeclType(LOCAL);
	    v.setName(_g_currentId);
	    v.setSize(_g_size);
	    v.setOffset(_g_offset);
            v.setDataType(_g_varType);
	    _g_funcTable.addLocal(v);
	    if(_g_varType.isPrimitive()) _g_offset -= _g_size;
	    else _g_offset -= SIZE_OF_PTR;
    }
	
    if(_g_varType.getPrimitiveType() == TYPE_VOID){
      _g_declarationError = true;
      cat::parse::fdecerror::voidtype(_g_lineCount, _g_currentId);
    }
    
    _g_varDecError = _g_varDecError || _g_declarationError;
    _g_declarationError = false;
}
| declarator_list ',' declarator
{

    
    
    if(!_g_declarationError)
    {
        
        VarDeclaration v;
        _g_varType.clearDimensions();
        v.setDeclType(LOCAL);
        v.setName(_g_currentId);
        v.setSize(_g_size);
        v.setOffset(_g_offset);
        v.setDataType(_g_varType);
        _g_funcTable.addLocal(v);
        if(_g_varType.isPrimitive())_g_offset -= _g_size;
        else _g_offset -= SIZE_OF_PTR;
    }
    
    if(_g_varType.getPrimitiveType() == TYPE_VOID){
      _g_declarationError = true;
      cat::parse::fdecerror::voidtype(_g_lineCount, _g_currentId);
    }
    
    _g_varDecError = _g_varDecError || _g_declarationError;
    _g_declarationError = false;
}
;
