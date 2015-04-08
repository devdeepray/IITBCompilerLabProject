
#include <iostream>
#include <list>
#include <vector>
#include "ASTNodes.h"
#include "../Util/catter.h"
using namespace std;

int tab_degree = 0;
int labelId = 0;
vector<string> codeArray;
FunctionTable currentFuncTable;

bool const& abstract_astnode::validAST() const { return valid; }
bool& abstract_astnode::validAST()  { return valid; }

void backPatch(list<int> indices, int label)
{
    for(auto it = indices.begin(); it != indices.end(); ++it)
    {
        codeArray[*it] += ("(label" + to_string(label) + ");");
    }
}

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

void Ass::genCode(bool fall, bool iscond, bool onstack, list<int>* fl, list<int>* tl)
{
	ValType valtype;
	int varoffset;
	c1->genLCode(&varoffset, &valtype);
	c2->genCode(false, false, true, &c2Tl, &c2Fl);
	
	
	if(valtype == TYPE_INT)
	{
		codeArray.push_back("loadi(ind(esp, 0), eax);"); // Load rval
		codeArray.push_back("loadi(ind(esp, I), ebx);"); // Load offset from var
		codeArray.push_back("popi(2);");
		codeArray.push_back("addi(ebp, ebx);");
		codeArray.push_back("storei(eax, ind(ebx, " + to_string(varoffset) + "));"); 
		
	}
	else
	{
		codeArray.push_back("loadf(ind(esp, 0), eax);"); // Load rval
		codeArray.push_back("loadi(ind(esp, I), ebx);"); // Load offset from var
		codeArray.push_back("popf(1);");
		codeArray.push_back("popi(1);");
		codeArray.push_back("addi(ebp, ebx);");
		codeArray.push_back("storef(eax, ind(ebx, " + to_string(varoffset) + "));"); 
	}

	if(!iscond)
	{
		if(onstack)
		{
			if(valtype == TYPE_INT)
			{
				codeArray.push_back("pushi(eax);");
			}
			else
			{
				codeArray.push_back("pushf(eax);");
			}
		}
	}
	else
	{
		codeArray.push_back("cmpi(0,eax);"); 
		if(fall)
		{
			*fl->push_back(codeArray.size());
			codeArray.push_back("je");
		}
		else
		{
			*tl->push_back(codeArray.size());
			codeArray.push_back("jne");
		}
	}
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


void ExpAst::genCode(bool fall, bool iscond, bool onstack, list<int>* tl, list<int>* fl)
{
	codeArray.push_back("Code for Exp");
}

void StmtAst::genCode(list<int>* nl)
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
	c1->genCode(true, true, &c1Tl, &c1Fl); // Fall, iscond, truelist, falselist
	
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

void FloatConst::genCode(bool fall, bool iscond, bool onstack, list<int>* tl, list<int>* fl)
{
	if(!iscond)
    {
		if(onstack)
		codeArray.push_back("pushf(" + to_string(val) + ");");
	}
	else
	{
	    if(val == 0.0)
	    {
	        if(fall)
	        {
	            fl->push_back(codeArray.size());
	            codeArray.push_back("j");
	        }
	    }
	    else
	    {
	        if(!fall)
	        {
	            tl->push_back(codeArray.size());
	            codeArray.push_back("j");
	        }
	    }
	}
	
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

void BinaryOp::genCode(bool fall,bool iscond, bool onstack, list<int> *truelist, list<int> *falselist)
{
    std::list<int> c1Tl, c1Fl, c2Tl, c2Fl;
	switch(op)
	{
		case OP_OR:
		case OP_AND:
		{
			
			if(op == OP_OR)
			{
				c1->genCode(false, true, false, &c1Tl, &c1Fl);
				codeArray.push_back("label" + to_string(labelId) + ":");
				backPatch(c1Fl, labelId);
				labelId++;
				c2->genCode(fall || !iscond, true, false, &c2Tl, &c2Fl);
				if(iscond)
				{
					(*truelist) = c1Tl;
					truelist->insert(truelist->end(), c2Tl.begin(), c2Tl.end());
					(*falselist) = c2Fl;
				}
				else
				{
					if(onstack)
					{
						codeArray.push_back("label" + to_string(labelId) + ":");
						backPatch(c1Tl, labelId);
						backPatch(c2Tl, labelId);
						labelId++;
						codeArray.push_back("pushi(1);");
						int pos = codeArray.size();
						codeArray.push_back("j");
						codeArray.push_back("label" + to_string(labelId) + ":");
						backPatch(c2Fl, labelId);
						labelId++;
						codeArray.push_back("pushi(0);");
						codeArray.push_back("label" + to_string(labelId) + ":");
						codeArray[pos] = "j(label" + to_string(labelId) +");"; 
						labelId++;
					}
					else
					{
						codeArray.push_back("label" + to_string(labelId) + ":");
						backPatch(c1Tl, labelId);
						backPatch(c2Tl, labelId);
						backPatch(c2Fl, labelId);
						++labelId;
					}
				}
			}
			else
			{
				c1->genCode(true, true, false, &c1Tl, &c1Fl);
				codeArray.push_back("label" + to_string(labelId) + ":");
				backPatch(c1Tl, labelId);
				labelId++;
				c2->genCode(fall || !iscond, true, false, &c2Tl, &c2Fl);
				if(iscond)
				{
				    (*truelist) = c2Tl;
				    (*falselist) = c1Fl;
				    falselist->insert(falselist->end(), c2Fl.begin(), c2Fl.end());
				}
				else
				{
					if(onstack)
					{
						codeArray.push_back("label" + to_string(labelId) + ":");
						backPatch(c2Tl, labelId);
						labelId++;
						codeArray.push_back("pushi(1);");
						int pos = codeArray.size();
						codeArray.push_back("j");
						codeArray.push_back("label" + to_string(labelId) + ":");
						backPatch(c1Fl, labelId);
						backPatch(c2Fl, labelId);
						labelId++;
						codeArray.push_back("pushi(0);");
						codeArray.push_back("label" + to_string(labelId) + ":");
						codeArray[pos] = "j(label" + to_string(labelId) +");"; 
						labelId++;
					}
					else
					{
						codeArray.push_back("label" + to_string(labelId) + ":");
						backPatch(c1Fl, labelId);
						backPatch(c2Tl, labelId);
						backPatch(c2Fl, labelId);
						++labelId;
					}
				}
			}
		}
		break;
		case OP_INT_PLUS:
		{
            c1->genCode(false /*garbage*/, false, iscond | onstack, &c1Tl, &c1Fl);
            c2->genCode(false, false, iscond | onstack, &c2Tl, &c2Fl);
            
            
            if(!iscond)
            {
				if(onstack)
				{
					codeArray.push_back("loadi(ind(esp, 0), eax);");
					codeArray.push_back("loadi(ind(esp, I), ebx);");
					codeArray.push_back("addi(ebx, eax);");
					codeArray.push_back("popi(2);");
					codeArray.push_back("pushi(eax);");
				}
            }
            else
            {
				codeArray.push_back("loadi(ind(esp, 0), eax);");
				codeArray.push_back("loadi(ind(esp, I), ebx);");
				codeArray.push_back("addi(ebx, eax);");
                codeArray.push_back("popi(2);");
                codeArray.push_back("cmpi(0,eax);"); 
                if(fall)
                {
                    falselist->push_back(codeArray.size());
                    codeArray.push_back("je");
                }
                else
                {
                    truelist->push_back(codeArray.size());
                    codeArray.push_back("jne");
                }
            }
		}
	}
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

void IntConst::genCode(bool fall, bool iscond, bool onstack, list<int>* tl, list<int>* fl)
{
    if(!iscond)
    {
		if(onstack)
			codeArray.push_back("pushi(" + to_string(val) + ");");
	}
	else
	{
	    if(val == 0)
	    {
	        if(fall)
	        {
	            fl->push_back(codeArray.size());
	            codeArray.push_back("j");
	        }
	    }
	    else
	    {
	        if(!fall)
	        {
	            tl->push_back(codeArray.size());
	            codeArray.push_back("j");
	        }
	    }
	}
	
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

void Identifier::genCode(int *idOffset , ValType *idValType, bool onstack, list <int> *remainingDim)
{
	// This is called by the arrayref AST.
    auto it = currentFuncTable.var_name_map.find(val);
    VarDeclaration curVarDecl = it->second;
    *idOffset = curVarDecl.offset;
    *idValType = curVarDecl.data_type.getPrimitiveType();
    *remainingDim = curVarDecl.data_type.array_dims;
    if(onstack)
    {
		codeArray.push_back("pushi(0);");
	}
}

void Identifier::genCode(bool fall, bool iscond, bool onstack, list <int> *tl, list <int> *fl)
{
	// It is just an identifier, not an array reference.
    auto it = currentFuncTable.var_name_map.find(val);
    VarDeclaration curVarDecl = it->second;
    int idOffset = curVarDecl.offset;
    ValType idValType = curVarDecl.data_type.getPrimitiveType();
    
    if(idValType == TYPE_INT)
    {
        
        if(!iscond)
        {
			if(onstack)
			{
				codeArray.push_back("loadi(ind(ebp,"+to_string(idOffset)+"),eax)");
				codeArray.push_back("pushi(eax);");
			}
        }
        else
        {
			codeArray.push_back("loadi(ind(ebp,"+to_string(idOffset)+"),eax)");
            codeArray.push_back("cmpi(0,eax)");
            if(fall)
            {
               fl->push_back(codeArray.size()); 
               codeArray.push_back("je");     
            }
            else
            {
               tl->push_back(codeArray.size()); 
               codeArray.push_back("jne");
            }
        }
    }    
    else if(idValType == TYPE_FLOAT)
    {
        
        if(!iscond)
        {
			if(onstack)
			{
				codeArray.push_back("loadf(ind(ebp,"+to_string(idOffset)+"),eax)");
				codeArray.push_back("pushf(eax);");
			}
        }
        else
        {
			codeArray.push_back("loadf(ind(ebp,"+to_string(idOffset)+"),eax)");
            codeArray.push_back("cmpf(0,eax)");
            if(fall)
            {
               fl->push_back(codeArray.size()); 
               codeArray.push_back("je");     
            }
            else
            {
               tl->push_back(codeArray.size()); 
               codeArray.push_back("jne");
            }
        }
    }
}



void Index::genCode(int *idOffset, ValType *idValType, bool onstack, list<int>* remainingDim)
{
	list<int> c2Tl, c2Fl;
	c1->genCode(idOffset, idValType, onstack, remainingDim);
	c2->genCode(false, false, onstack, &c2Tl, &c2Fl);
	int curdim = remainingDim->front();
	remainingDim->pop_front();
	if(onstack)
	{
		codeArray.push_back("loadi(ind(esp, I), eax);");
		codeArray.push_back("loadi(ind(esp, 0), ebx);");
		codeArray.push_back("muli(" + to_string(curdim) + ", eax);");
		codeArray.push_back("addi(ebx, eax);");
		codeArray.push_back("popi(2);");
		codeArray.push_back("pushi(eax)");
	}

}

void Index::genCode(bool fall, bool iscond, bool onstack, list<int>* tl, list<int>* fl)
{
    int idOffset;
    ValType idValType;
    list <int> remainingDim;
    list <int> c2Tl,c2Fl;
    c1->genCode(&idOffset, &idValType, onstack | iscond, &remainingDim);
    c2->genCode(false /*garbage*/, false, onstack | iscond, &c2Tl, &c2Fl);
    
    if(onstack | iscond)
    {
		codeArray.push_back("loadi(ind(esp, I), eax);"); // getting the partial offset calculation
		codeArray.push_back("loadi(ind(esp, 0), ebx);"); // loading the last index into ebx
		codeArray.push_back("muli(" + to_string(remainingDim.front())+ ",eax);"); //multiplying with dimension, holding in eax
		codeArray.push_back("addi(ebx,eax)"); //final offset from ebp
		codeArray.push_back("muli(" + string(((idValType == TYPE_INT)?"I" : "F")) + ", eax);");
		codeArray.push_back("addi(ebp, eax);");
		codeArray.push_back("popi(2);");
	}

	if(idValType == TYPE_INT)
	{
		
		if(iscond)
		{
			codeArray.push_back("loadi(ind(eax, "+ to_string(idOffset) + "), eax);");
			codeArray.push_back("cmpi(0, eax);");
			if(fall)
			{
				fl->push_back(codeArray.size());
				codeArray.push_back("je");
			}
			else
			{
				tl->push_back(codeArray.size());
				codeArray.push_back("jne");
			}
		}
		else
		{
			if(onstack)
			{
				codeArray.push_back("loadi(ind(eax, "+ to_string(idOffset) + "), eax);");
				codeArray.push_back("pushi(eax);");
			}
		}
	}
	
	else if(idValType == TYPE_FLOAT)
	{
		
		if(iscond)
		{
			codeArray.push_back("loadf(ind(eax, "+ to_string(idOffset) + "), eax);");
			codeArray.push_back("cmpf(0, eax);");
			if(fall)
			{
				fl->push_back(codeArray.size());
				codeArray.push_back("je");
			}
			else
			{
				tl->push_back(codeArray.size());
				codeArray.push_back("jne");
			}
		}
		else
		{
			if(onstack)
			{
				codeArray.push_back("loadf(ind(eax, "+ to_string(idOffset) + "), eax);");
				codeArray.push_back("pushf(eax);");
			}
		}
	}
	else
	{
		cerr << "BUG IN CODE" << endl;
	}
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
