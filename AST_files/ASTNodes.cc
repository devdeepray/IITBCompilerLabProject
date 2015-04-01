
#include <iostream>
#include <list>
#include <vector>
#include "ASTNodes.h"
#include "../Util/catter.h"
using namespace std;

int tab_degree = 0;
int labelId = 0;
vector<string> codeArray;
bool const& abstract_astnode::validAST() const { return valid; }
bool& abstract_astnode::validAST()  { return valid; }


// For printing with indentation
void indent_print(std::string s) 
{
  for(int i = 1; i < tab_degree; ++i)
    std::cout << "  ";
  std::cout << s;
}


void ProgAst::addFunctionDef(StmtAst* funcDef)
{
	funcList.push_back(funcDef);
}

void ProgAst::print()
{
}

DataType& ExpAst::dataType()
{
  return data_type;
}

// Empty AST
Empty::Empty()
{
  astnode_type = AST_EMP;
}
// Print empty AST
void Empty::print()
{
  tab_degree++;
  indent_print( "(Empty)" );
  tab_degree--;
}


// Block of statements
Block::Block(StmtAst* _c)
{
  astnode_type = AST_BLK;
  clist.push_back(_c);
}

// Print block AST
void Block::print()
{
  tab_degree++;
  indent_print( "(Block [" );
  for(auto it = clist.begin(); it != clist.end(); ++it)
  {
    cout << "\n" ;
    (*it)->print();
    
  }
  cout << "])" ;
  
  tab_degree--;
}

// Add a statement to the block
void Block::insert(StmtAst* c)
{
  clist.push_back(c);
}


// Assignment AST
Ass::Ass(ExpAst* left_stmt, ExpAst* right_stmt)
{
  astnode_type = AST_ASS;
  c1 = left_stmt;
  c2 = right_stmt;
}

// Print assignment AST
void Ass::print()
{
  tab_degree++;
  
  indent_print( "(Assign_exp \n" ); 
  c1->print();
  indent_print( "\n" );
  c2->print();
  cout << ")" ;
  tab_degree--;
}

ExpStmt::ExpStmt(ExpAst* exp)
{
   c1 = exp;
}

void ExpStmt::print()
{
  tab_degree++;
  indent_print("(SingleStmt \n");
  c1->print();
  cout << ")";
  tab_degree--;
}

// Return statement AST
Return::Return(ExpAst* ret_exp)
{
  astnode_type = AST_RET;
  c1 = ret_exp;
}

// Print return AST
void Return::print()
{
  tab_degree++;
  
  indent_print( "(Return\n" );
  c1->print();
  cout << ")" ;
  tab_degree--;
}


 void ExpAst::genCode()
{
	codeArray.push_back("Code for Exp");
}

 void StmtAst::genCode()
{
	codeArray.push_back("Code for Stmt");
}

If::If(ExpAst* cond, StmtAst* if_stats, StmtAst* else_stats)
{
  astnode_type = AST_IF;
  c1 = cond;
  c2 = if_stats;
  c3 = else_stats;
}
void If::print()
{
  tab_degree++;
  
  indent_print( "(If\n" );
  c1->print();
  indent_print( "\n" );
  c2->print();
  indent_print( "\n" );
  c3->print();
  cout << ")";
  tab_degree--;
}

void If::genCode(list<int> *nextList)
{
	// We assume that result of c1 is in eax
	list<int> selfNextList; // Next list for the if statement
	list<int> c1Tl, c1Fl, c2Nl, c3Nl;
	c1->genCode(true, true, &c1Tl, &c1Fl); // Fall, isCond, truelist, falselist
	if(c1Tl.size() == 0 && c1Fl.size() == 0)
	{
		// Case that inside if, we have arithmetic expr
		codeArray.push_back("cmpi(0, eax);");
		int failJumpLine = codeArray.size();
		codeArray.push_back("je"); // Jump to else part
		c2->genCode(&c2Nl);
		selfNextList.push_back(codeArray.size());
		codeArray.push_back("j"); // Jump over else part
		codeArray.push_back("label"+to_string(labelId)+":");
		codeArray.at(failJumpLine) = "je(label" + to_string(labelId) + ");";
		++labelId;
		c3->genCode(&c3Nl);
	}
	else
	{
		codeArray.push_back("label" + to_string(labelId) + ":");
		int trueLabel = labelId;
		++labelId;
		c2->genCode(&c2Nl);
		selfNextList.push_back(codeArray.size()); // Exit jump wala line
		codeArray.push_back("j"); // Jump over else part
		codeArray.push_back("label" + to_string(labelId) + ":");
		int falseLabel = labelId;
		++labelId;
		c3->genCode(&c3Nl);
		backPatch(c1Tl, trueLabel);
		backPatch(c1Fl, falseLabel);
	}

	selfNextList.insert(selfNextList.end(), c2Nl.begin(), c2Nl.end());
	selfNextList.insert(selfNextList.end(), c3Nl.begin(), c3Nl.end());
	(*nextList) = selfNextList;
}




For::For(ExpAst* initialize, ExpAst* guard, ExpAst* update, StmtAst* forbody)
{
  astnode_type = AST_FOR;
  c1 = initialize;
  c2 = guard;
  c3 = update;
  c4 = forbody;
}
void For::print()
{
  tab_degree++;
  
  
  indent_print( "(For\n" );
  c1->print();
  indent_print( "\n" );
  c2->print();
  indent_print( "\n" );
  c3->print();
  indent_print( "\n" );
  c4->print();
  cout << ")";
  tab_degree--;
}



While::While(ExpAst* guard, StmtAst* whilebody)
{
  astnode_type = AST_WHILE;
  c1 = guard;
  c2 = whilebody;
}
void While::print()
{
  tab_degree++;
  indent_print( "(While\n" );
  c1->print();
  indent_print( "\n" );
  c2->print();
  cout << ")" ;
  tab_degree--;
}


FloatConst::FloatConst(float _val)
{
  astnode_type = AST_FLOAT;
  val = _val;
}
void FloatConst::print()
{
  tab_degree++;
  
  indent_print( "(FloatConst " );
  cout << val << ")" ;
  tab_degree--;
}


BinaryOp::BinaryOp(ExpAst* left_exp , ExpAst* right_exp, OpType _op)
{
  astnode_type = AST_BINOP;
  op= _op;
  c1=left_exp;
  c2=right_exp;
}
void BinaryOp::print()
{
  tab_degree++;
  indent_print("(");
  cout << opTypeLookup[op] << "\n";
  c1->print();
  indent_print("\n");
  c2->print();
  cout << ")" ;
  tab_degree--;
}


UnaryOp::UnaryOp(ExpAst* exp, OpType _op)
{
  astnode_type = AST_UNOP;
  op = _op;
  c1=exp;
}
void UnaryOp::print()
{
  tab_degree++;
  indent_print("(");
  cout << opTypeLookup[op] << "\n";
  c1->print();
  cout << ")";
  tab_degree--;
}



FunCall::FunCall(ExpAst* _exp_ast)
{
  astnode_type = AST_FUNCALL;
  if( _exp_ast) list_exp_ast.push_back(_exp_ast);
}

void FunCall::setName(string fname)
{
  func_name = fname;
}

void FunCall::print()
{
  tab_degree++;
  indent_print( "("+func_name );
  for( auto i = list_exp_ast.begin() ; i != list_exp_ast.end() ; i++)
  {
    cout<<"\n";
    (*i)->print();
    
  }
  cout << ")";
  tab_degree--;
}

std::list< DataType > FunCall::getArgTypeList()
{
  std::list<DataType> l;
  for(auto it = list_exp_ast.begin(); it != list_exp_ast.end(); ++it)
  {
    l.push_back((*it)->data_type);
  }
  return l;
}

void FunCall::insert(ExpAst* e)
{
  list_exp_ast.push_back(e);
}


IntConst::IntConst(int _val)
{
  astnode_type = AST_INT;
  val = _val;
}
void IntConst::print()
{
  tab_degree++;
  indent_print("(IntConst ");
  cout << val << ")";
  tab_degree--;
}



StringConst::StringConst(std::string _val)
{
  astnode_type = AST_STR;
  val = _val;
}
void StringConst::print()
{
  tab_degree++;
  indent_print("(StrConst ");
  cout << val << ")";
  tab_degree--;
}




Identifier::Identifier(std::string _val)
{
  astnode_type = AST_IDENTIFIER;
  val = _val;
}
void Identifier::print()
{
  tab_degree++;
  indent_print("(Identifier ");
  cout << val << ")";
  tab_degree--;
}



Index::Index(ArrayRef* arrRef , ExpAst* expAst)
{
  astnode_type = AST_INDEX;
  
  c1 = arrRef;
  
  c2 = expAst;
}


void Index::print()
{
  tab_degree++;
  indent_print("(ArrayRef\n");
  c1->print();
  indent_print("\n");
  c2->print();
  cout << ")";
  tab_degree--;
}

std::string ArrayRef::getArrayName()
{
    return "Arbit";
}
   

std::string Index::getArrayName()
{
    return (c1)->getArrayName();
}

std::string Identifier::getArrayName()
{
    return val;
}
