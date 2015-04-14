
#include <iostream>
#include <list>
#include <vector>
#include "ASTNodes.h"
#include "../Util/catter.h"
#include <stack>

#define HW_ADDR_SIZE 4
using namespace std;

// Global helper variables //
int tab_degree = 0; // Number of tabs, for pretty printing

int labelId = 0; // Global for generating unique labels
vector<string> codeArray; // Array of generated code
FunctionTable currentFuncTable; // Function table of the current function
stack<string> reg_stack;

/* *********************************************************************
 * BEGIN
 * Global helper functions
 **********************************************************************/
 
// Backpatch the code array at positions specified in indeices
void backPatch(list<int> indices, int label)
{
    for(auto it = indices.begin(); it != indices.end(); ++it)
    {
        codeArray[*it] += ("(label" + to_string(label) + ");");
    }
}
void indent_print(std::string s) 
{
  for(int i = 1; i < tab_degree; ++i)
    std::cout << "  ";
  std::cout << s;
}
void init_reg_stack()
{		
	reg_stack.push("edx");
	reg_stack.push("ecx");
	reg_stack.push("ebx");
	reg_stack.push("eax");
}
/* *********************************************************************
 * END
 * Global helper functions
 **********************************************************************/


/* *********************************************************************
 * BEGIN
 * abstract_astnode
 **********************************************************************/
 
bool& abstract_astnode::validAST()  { return valid; }
void abstract_astnode::print()
{
	cerr <<  "abstract_astnode: NEED TO OVERRIDE print()" << endl;
}

/* *********************************************************************
 * END
 * abstract_astnode
 **********************************************************************/


/* *********************************************************************
 * BEGIN
 * ExpAst
 **********************************************************************/

DataType& ExpAst::dataType()
{
  return data_type;
}
void ExpAst::calcLabel()
{
	cerr << "ExpAst: NEED TO OVERRIDE calcLabel()" << endl;
}
void ExpAst::genCode(list<int>* tl, list<int>* fl)
{
	cerr << "ExpAst: NEED TO OVERRIDE genCode(list<int>*, list<int>*)" 
																<< endl;
}
void ExpAst::calcAttributes()
{
	cerr << "ExpAst: NEED TO OVERRIDE calcAttributes()" << endl;
}

void ExpAst::pruneAST()
{
	cerr << "ExpAst: NEED TO OVERRIDE pruneAST()" << endl;
}

/* *********************************************************************
 * END
 * ExpAst
 **********************************************************************/



/* *********************************************************************
 * BEGIN
 * StmtAst
 **********************************************************************/

void StmtAst::genCode(list <int> *nextList)
{
	cerr << "StmtAst: NEED TO OVERRIDE genCode(list<int>*);" << endl;
}

/* *********************************************************************
 * END
 * StmtAst
 **********************************************************************/



/* *********************************************************************
 * BEGIN
 * ProgAst
 **********************************************************************/

void ProgAst::addFunctionDef(StmtAst* funcDef)
{
	funcList.push_back(funcDef);
}
void ProgAst::print()
{
	cerr << "ProgAst: ADD DEFINITION print" << endl;
}
void ProgAst::genCode()
{
    for(auto it = funcList.begin(); it != funcList.end(); ++it)
    {
		list<int> func_next_list;
        (*it)->genCode(&func_next_list);
        if(func_next_list.size() != 0)
        {
			cerr << "CODE FOR FUNCTION HAS UNFILLED JUMPS" << endl;
		}
    }
}

/* *********************************************************************
 * END
 * ProgAst
 **********************************************************************/



/* *********************************************************************
 * BEGIN
 * Empty ast node
 **********************************************************************/

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

/* *********************************************************************
 * END
 * Empty ast node
 **********************************************************************/
 
 
/* *********************************************************************
 * BEGIN
 * Block ast
 **********************************************************************/
 
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
void Block::insert(StmtAst* c)
{
  clist.push_back(c);
}
void Block::genCode(list <int> *nextList)
{
	list<int> newNextList;
	for(auto it = clist.begin(); it != clist.end();)
	{	
		newNextList.clear();
		(*it)->genCode(&newNextList);
		++it;
		if(it != clist.end() && newNextList.size()!=0)
		{	
			codeArray.push_back("label"+to_string(labelId)+":");
			backPatch(newNextList,labelId);
			labelId++;
		}
	}
    *nextList = newNextList;
}

/* *********************************************************************
 * END
 * Block ast
 **********************************************************************/



/* *********************************************************************
 * BEGIN
 * ExpStmt ast
 **********************************************************************/

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
void ExpStmt::genCode(list <int> *nextList)
{   
    // Calc code generation attributes
    c1->fall = false;
    c1->is_cond = false;
    c1->need_val = false;
    c1->dir_const = false;
    c1->calcAttributes(); // Calculates the attributes recursively
	c1->calcLabel(); // Calculates all labels in the exp tree
	c1->pruneAST();
	// Generate code for the expression
    c1->genCode(NULL, NULL); // No tl, fl for is_cond == false
}

/* *********************************************************************
 * END
 * ExpStmt
 **********************************************************************/


/* *********************************************************************
 * BEGIN
 * Return ast
 **********************************************************************/
 
Return::Return(ExpAst* ret_exp)
{
  astnode_type = AST_RET;
  c1 = ret_exp;
}
void Return::print()
{
  tab_degree++;
  
  indent_print( "(Return\n" );
  c1->print();
  cout << ")" ;
  tab_degree--;
}
void Return::genCode(list<int>* nextList)
{
	//! CLEAN ENOUGH FOR MY LIKING
	//! SETHI-ULLMAN version
	// Leave the nextlist as is
	c1->fall = false; 
	c1->is_cond = false;
	c1->need_val = true;
	c1->calcAttributes();
	c1->calcLabel();
	c1->genCode(NULL, NULL);
	
	// Need to put return val, and clean stack
	int returnValueAdd;
	
	ValType returnType = 
			currentFuncTable.getReturnType().getPrimitiveType();
	
	if((currentFuncTable.var_offset_map.rbegin() 
							== currentFuncTable.var_offset_map.rend())
		|| (currentFuncTable.var_offset_map.rbegin()->first < 0))
	{
		// If nothing at all, or no params, only locals
		returnValueAdd = HW_ADDR_SIZE; 
	}
	else
	{
		// Get the last param, and add its size, to get loc of ret.
		int offset = currentFuncTable.var_offset_map.rbegin()->first;
		string vName = currentFuncTable.var_offset_map[offset];
		int size = currentFuncTable.var_name_map[vName].size;
		returnValueAdd = offset + size;
	}
	
	if(returnType == TYPE_INT)
	{
		string reg_name = reg_stack.top();
		codeArray.push_back("storei(" + reg_name + ", ind(ebp," 
									+ to_string(returnValueAdd) +"));");
	}
	else if(returnType == TYPE_FLOAT)
	{
		string reg_name = reg_stack.top();
		codeArray.push_back("storef(" + reg_name + ", ind(ebp," 
									+ to_string(returnValueAdd) +"));");
	}
	
	// shrink stack
	codeArray.push_back("move(ebp,esp);");
	codeArray.push_back("addi(I,esp);");
	
	// restore ebp
	codeArray.push_back("loadi(ind(ebp, 0), ebp);");
	
	// return
	codeArray.push_back("return;");
}

/* *********************************************************************
 * END
 * Return ast
 **********************************************************************/




/* *********************************************************************
 * BEGIN
 * If statement 
 **********************************************************************/

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
void If::genCode(list <int> *nextList)
{
	//! CLEAN ENOUGH FOR MY LIKING
	//! SETHI-ULLMAN version
	list<int> c1Tl, c1Fl, c2Nl, c3Nl;
	
	// Generate code for c1
	c1->fall = true;
	c1->is_cond = true;
	c1->need_val = false;
	c1->calcAttributes();
	c1->calcLabel();
	c1->genCode(&c1Tl, &c1Fl); 
	
	codeArray.push_back("label" + to_string(labelId) + ":");
	int trueLabel = labelId;
	++labelId;
	
	c2->genCode(&c2Nl);
	
	nextList->push_back(codeArray.size());
	codeArray.push_back("j"); // Jump over else part
	
	codeArray.push_back("label" + to_string(labelId) + ":");
	int falseLabel = labelId;
	++labelId;
	
	c3->genCode(&c3Nl);
	
	backPatch(c1Tl, trueLabel);
	backPatch(c1Fl, falseLabel);
	
	// Compute nextList
	nextList->insert(nextList->end(), c2Nl.begin(), c2Nl.end());
	nextList->insert(nextList->end(), c3Nl.begin(), c3Nl.end());
}

/* *********************************************************************
 * END
 * If statement
 **********************************************************************/
 
 
 /* *********************************************************************
 * BEGIN
 * For statement
 **********************************************************************/
 
void For::genCode(list <int> *nextList)
{
	//! CLEAN ENOUGH FOR MY LIKING
	//! SETHI-ULLMAN version
	list <int> c4Nl, c1Tl, c1Fl, c2Tl, c2Fl, c3Tl, c3Fl;
	
	//Initiallization
	c1->fall = false;
	c1->is_cond = false;
	c1->need_val = false;
	c1->calcAttributes();
	c1->calcLabel();
	c1->genCode(&c1Tl, &c1Fl); 
	
	//Condition
	codeArray.push_back("label" + to_string(labelId)+ ":");
	int conditionLabel = labelId;
	++labelId;
	
	c2->fall = true;
	c2->is_cond = true;
	c2->need_val = false;
	c2->calcAttributes(); 
	c2->calcLabel();
	c2->genCode(&c2Tl, &c2Fl);
	
	//Start of FOR body
	codeArray.push_back("label" + to_string(labelId) + ":");
	int forBodyLabel = labelId;
	++labelId;
	c4->genCode(&c4Nl);
	
	//Update Body
	codeArray.push_back("label" + to_string(labelId) + ":");
	int updateLabel = labelId;
	++labelId;
	
	c3->fall = false;
	c3->is_cond = false;
	c3->need_val = false;
	c3->genCode(&c3Tl, &c3Fl);
	
	// Jumping to condition check
	codeArray.push_back("j(label"+to_string(conditionLabel)+");");
	
	//Backpatching true list and false list of condition
	backPatch(c2Tl,forBodyLabel);
	*nextList = c2Fl;
	
	//Backpatching nextlist of the for body
	backPatch(c4Nl,updateLabel);
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

/* *********************************************************************
 * END
 * For statement
 **********************************************************************/
 
 

/* *********************************************************************
 * BEGIN
 * While statement
 **********************************************************************/

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
void While::genCode(list <int> *nextList)
{

	//! CLEAN ENOUGH FOR MY LIKING
	//! SETHI-ULLMAN version
	list<int> c1Tl, c1Fl;
	list <int> c2Nl;
		
	//Condition
	codeArray.push_back("label" + to_string(labelId) + ":");
	int conditionLabel = labelId;
	++labelId;
	
	c1->fall = true;
	c1->is_cond = true;
	c1->need_val = false;
	c1->calcAttributes();
	c1->calcLabel();
	c1->genCode(&c1Tl, &c1Fl); 
	
	//Body
	codeArray.push_back("label" + to_string(labelId) + ":");
	int bodyLabel = labelId;
	++labelId;
	c2->genCode(&c2Nl);
	
	//Unconditional jump to condition
	codeArray.push_back("j(label" + to_string(conditionLabel) + ");");
	
	//Backpatching TL and FL of condition
	backPatch(c1Tl,bodyLabel);
	*nextList=c1Fl;
	
	//Backpatching next list of body
	backPatch(c2Nl,conditionLabel);
}


/* *********************************************************************
 * END
 * While statement
 **********************************************************************/


/* *********************************************************************
 * BEGIN
 * Constants
 **********************************************************************/

///////////////////////// START FLOAT CONSTANTS ////////////////////////
FloatConst::FloatConst(float _val)
{
  astnode_type = AST_FLOAT;
  float_val = _val;
}
void FloatConst::print()
{
  tab_degree++;
  
  indent_print( "(FloatConst " );
  cout << float_val << ")" ;
  tab_degree--;
}
void FloatConst::genCode(list<int>* tl, list<int>* fl)
{
	if(!is_cond)
    {
		// Is not a condition
		if(need_val)
		{
			// Value is needed
			string reg_name = reg_stack.top();
			codeArray.push_back("move(" + to_string(float_val) + ", " + reg_name + ");");
		}
		else
		{
			// Value is not needed, dont do anything
		}
	}
	else
	{
	    if(float_val == 0.0)
	    {
			// If constant is zero, ie expression is false
	        if(fall)
	        {
	            fl->push_back(codeArray.size());
	            codeArray.push_back("j");
	        }
	    }
	    else
	    {
			// If constant is non-zero, ie expression is true
	        if(!fall)
	        {
	            tl->push_back(codeArray.size());
	            codeArray.push_back("j");
	        }
	    }
	}
}
void FloatConst::calcLabel()
{	
	// Assumes is_cond, need_val, fall, dir_const, is_const is set
	if(is_cond)
	{
		// No need for a register if it is a condition
		reg_label = 0;
	}
	else
	{
		if(need_val && !dir_const)
		{
			reg_label = 1;
		}
		else
		{
			reg_label = 0;
		}
	}
}
void FloatConst::calcAttributes()
{
	is_const = true;
	return; // All attributes already set
}
void FloatConst::pruneAST()
{
	return;
}

///////////////////////// END FLOAT CONSTANTS //////////////////////////


///////////////////////// START INT CONSTANTS //////////////////////////
IntConst::IntConst(int _val)
{
  astnode_type = AST_INT;
  int_val = _val;
}
void IntConst::print()
{
  tab_degree++;
  indent_print("(IntConst ");
  cout << int_val << ")";
  tab_degree--;
}

void IntConst::genCode(list<int>* tl, list<int>* fl)
{
    if(!is_cond)
    {
		// Is not a condition
		if(need_val)
		{
			// Value is needed
			string reg_name = reg_stack.top();
			codeArray.push_back("move(" + to_string(int_val) + ", " + reg_name + ");");
		}
		else
		{
			// Value is not needed, dont do anything
		}
	}
	else
	{
	    if(int_val == 0)
	    {
			// If constant is zero, ie expression is false
	        if(fall)
	        {
	            fl->push_back(codeArray.size());
	            codeArray.push_back("j");
	        }
	    }
	    else
	    {
			// If constant is non-zero, ie expression is true
	        if(!fall)
	        {
	            tl->push_back(codeArray.size());
	            codeArray.push_back("j");
	        }
	    }
	}
	
}

void IntConst::calcLabel()
{
	// Assumes is_cond, need_val, fall, dir_const, is_const is set
	if(is_cond)
	{
		// No need for a register if it is a condition
		reg_label = 0;
	}
	else
	{
		if(need_val && !dir_const)
		{
			reg_label = 1;
		}
		else
		{
			reg_label = 0;
		}
	}
}

void IntConst::calcAttributes()
{
	is_const = true;
	return;
}

void IntConst::pruneAST()
{
	return;
}

/////////////////////// END INT CONSTANT ///////////////////////////////


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

/* *********************************************************************
 * END
 * Constants
 **********************************************************************/



/* *********************************************************************
 * BEGIN
 * BinaryOp
 **********************************************************************/

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
void BinaryOp::calcLabel()
{
	c1->calcLabel();
	c2->calcLabel();
	
	switch(op)
	{
		case OP_INT_PLUS:
		case OP_FLOAT_PLUS:
		case OP_INT_MINUS:
		case OP_FLOAT_MINUS:
		case OP_INT_MULT:
		case OP_FLOAT_MULT:
		case OP_INT_DIV:
		case OP_FLOAT_DIV:
		// Case for simple binary operators
		if(is_cond)
		{
			if(is_const)
			{
				// Is compile time constant
				reg_label = 0;
			}
			else
			{
				// Needs to compute from l and r
				if(c1->reg_label == c2->reg_label)
				{
					// L and R are same, need one more
					reg_label = c1->reg_label + 1;
				}
				else
				{
					// L and R are different
					reg_label = max(c1->reg_label, c2->reg_label);
				}
			}
		}
		else
		{
			if(need_val)
			{
				if(is_const)
				{
					// It is a compile time constant
					if(dir_const) reg_label = 0;
					else reg_label = 1;
				}
				else
				{
					// Needs to compute from l and r
					if(c1->reg_label == c2->reg_label)
					{
						reg_label = c1->reg_label + 1;
					}
					else
					{
						reg_label = max(c1->reg_label, c2->reg_label);
					}
				}
			}
			else
			{
				// Dont need val, just eval l and r.
				reg_label = max(c1->reg_label, c2->reg_label);
			}
		}
		break;
		case OP_OR:
		case OP_AND:
		// Case for boolean logical operators
		if(is_cond)
		{
			if(is_const)
			{
				// Is compile time constant
				reg_label = 0;
			}
			else
			{
				// L and R are different
				reg_label = max(c1->reg_label, c2->reg_label);
			}
		}
		else
		{
			if(need_val)
			{
				if(is_const)
				{
					// It is a compile time constant
					if(dir_const) reg_label = 0;
					else reg_label = 1;
				}
				else
				{
					reg_label = max(c1->reg_label, c2->reg_label);
					if(reg_label == 0) reg_label = 1;
				}
			}
			else
			{
				// Dont need val, just eval l and r.
				reg_label = max(c1->reg_label, c2->reg_label);
			}
		}
		break;
		case OP_INT_EQ:
		case OP_FLOAT_EQ:
		case OP_INT_LT:
		case OP_FLOAT_LT:
		case OP_INT_LE:
		case OP_FLOAT_LE:
		case OP_INT_GT:
		case OP_FLOAT_GT:
		case OP_INT_GE:
		case OP_FLOAT_GE:
		// Case for comparison operators
		
		if(is_cond)
		{
			if(is_const)
			{
				// Is compile time constant
				reg_label = 0;
			}
			else
			{
				// Needs to compute from l and r
				if(c1->reg_label == c2->reg_label)
				{
					// L and R are same, need one more
					reg_label = c1->reg_label + 1;
				}
				else
				{
					// L and R are different
					reg_label = max(c1->reg_label, c2->reg_label);
				}
			}
		}
		else
		{
			if(need_val)
			{
				if(is_const)
				{
					// It is a compile time constant
					if(dir_const) reg_label = 0;
					else reg_label = 1;
				}
				else
				{
					// Needs to compute from l and r
					if(c1->reg_label == c2->reg_label)
					{
						reg_label = c1->reg_label + 1;
					}
					else
					{
						reg_label = max(c1->reg_label, c2->reg_label);
					}
				}
			}
			else
			{
				// Dont need val, just eval l and r.
				reg_label = max(c1->reg_label, c2->reg_label);
			}
		}
		break;
		case OP_ASSIGN:
		if(is_cond)
		{
			if(is_const)
			{
				// NOT YET IMPLEMENTED. WILL NEVER COMEHERE
				cerr << "OPASSIGN calcLabel error" << endl;
			}
			else
			{
				if(c1->reg_label == c2->reg_label)
				{
					reg_label = c1->reg_label + 1;
				}
				else
				{
					c1->reg_label = max(c1->reg_label, c2->reg_label);
				}
			}
		}
		else
		{
			if(is_const)
			{
				// NOT YET IMPLEMENTED. WILL NEVER COMEHERE
				cerr << "OPASSIGN calcLabel error" << endl;
			}
			else
			{
				if(c1->reg_label == c2->reg_label)
				{
					reg_label = c1->reg_label + 1;
				}
				else
				{
					c1->reg_label = max(c1->reg_label, c2->reg_label);
				}
			}
		}
	}
}
void BinaryOp::calcAttributes()
{
	cout << "Calc binop attr" << endl;
	switch(op)
	{
		case OP_OR:
			c1->is_cond = true;
			c1->fall = false;
			c1->need_val = false;
			c1->dir_const = false;
			
			c2->is_cond = true;
			c2->fall = fall || !is_cond; // If i want to eval, convenient to make it fall if true
			c2->need_val = false;
			c2->dir_const = false;
			
			c1->calcAttributes();
			c2->calcAttributes();
			
			if(c1->is_const && c2->is_const)
			{
				is_const = true;
				int_val = c1->int_val && c2->int_val;
			}
			break;
		case OP_AND:
			c1->is_cond = true;
			c1->fall = true;
			c1->need_val = false;
			c1->dir_const = false;
			
			c2->is_cond = true;
			c2->fall = fall || !is_cond; // Make fall true for easy evaluation
			c2->need_val = false;
			c2->dir_const = false;
			
			c1->calcAttributes();
			c2->calcAttributes();
			
			if(c1->is_const && c2->is_const)
			{
				is_const = true;
				int_val = c1->int_val || c2->int_val;
			}
			break;
		case OP_INT_PLUS:
			c1->is_cond = false;
			c1->fall = false;
			c1->need_val = is_cond || need_val;
			c1->dir_const = true;
			c2->is_cond = false;
			c2->fall = false;
			c2->need_val = is_cond || need_val;
			c2->dir_const = true;
			c1->calcAttributes();
			c2->calcAttributes();
			
			if(c1->is_const && c2->is_const)
			{
				is_const = true;
				int_val = c1->int_val + c2->int_val;
			}
			break;
		case OP_ASSIGN:
			c1->is_cond = false;
			c1->fall = false;
			c1->need_val = true;
			c1->dir_const = false;
			
			c2->is_cond = false;
			c2->fall = false;
			c2->need_val = true;
			c2->dir_const = true;
			
			c1->calcAttributes();
			c2->calcAttributes();
			
			is_const = false;
			
			// TODO: CONST OPTIMIZATION FOR ASSIGN
			break;
		case OP_INT_LT:
			c1->is_cond = false;
			c1->fall = false;
			c1->need_val = is_cond || need_val;
			c1->dir_const = true;
			
			c2->is_cond = false;
			c2->fall = false;
			c2->need_val = is_cond || need_val;
			c2->dir_const = true;
			
			c1->calcAttributes();
			c2->calcAttributes();
			
			if(c1->is_const && c2->is_const)
			{
				is_const = true;
				int_val = (c1->int_val < c1->int_val);
			}
			break;
		case OP_INT_MINUS:
			c1->is_cond = false;
			c1->fall = false;
			c1->need_val = is_cond || need_val;
			c1->dir_const = true;
			
			c2->is_cond = false;
			c2->fall = false;
			c2->need_val = is_cond || need_val;
			c2->dir_const = false;
			
			c1->calcAttributes();
			c2->calcAttributes();
			
			if(c1->is_const && c2->is_const)
			{
				is_const = true;
				int_val = c1->int_val - c2->int_val;
			}
			break;
	}
}

void BinaryOp::pruneAST()
{
	if(c1->is_const)
	{
		ExpAst* tmp = c1;
		ExpAst* newc1;
		if(c1->data_type.getPrimitiveType() == TYPE_FLOAT)
		{
			newc1 = new FloatConst(c1->float_val);
		}
		else
		{
			newc1 = new IntConst(c1->int_val);
		}
		newc1->is_cond = c1->is_cond;
		newc1->reg_label = c1->reg_label;
		newc1->fall = c1->fall;
		newc1->need_val = c1->need_val;
		newc1->dir_const = c1->dir_const;
		newc1->is_const = c1->is_const;
		c1 = newc1;
	}
	
	if(c2->is_const)
	{
		ExpAst* tmp = c2;
		ExpAst* newc2;
		if(c2->data_type.getPrimitiveType() == TYPE_FLOAT)
		{
			newc2 = new FloatConst(c2->float_val);
		}
		else
		{
			newc2 = new IntConst(c2->int_val);
		}
		newc2->is_cond = c2->is_cond;
		newc2->reg_label = c2->reg_label;
		newc2->need_val = c2->need_val;
		newc2->fall = c2->fall;
		newc2->dir_const = c2->dir_const;
		newc2->is_const = c2->is_const;
		c2 = newc2;
	}
}

void BinaryOp::genCode(list<int> *truelist, list<int> *falselist)
{

    
    std::list<int> c1Tl, c1Fl, c2Tl, c2Fl;
	switch(op)
	{
		case OP_OR:
		case OP_AND:
		{
			
			if(op == OP_OR)
			{
				
				// Here it is pretty standard.
				c1->genCode(&c1Tl, &c1Fl);
				codeArray.push_back("label" + to_string(labelId) + ":");
				backPatch(c1Fl, labelId);
				labelId++;
				c2->genCode(&c2Tl, &c2Fl);
				if(is_cond)
				{
					(*truelist) = c1Tl;
					truelist->insert(truelist->end(), c2Tl.begin(), c2Tl.end());
					(*falselist) = c2Fl;
				}
				else
				{
					if(need_val)
					{
						codeArray.push_back("label" + to_string(labelId) + ":");
						backPatch(c1Tl, labelId);
						backPatch(c2Tl, labelId);
						labelId++;
						codeArray.push_back("move(1, " + reg_stack.top() + ");");
						int pos = codeArray.size();
						codeArray.push_back("j");
						codeArray.push_back("label" + to_string(labelId) + ":");
						backPatch(c2Fl, labelId);
						labelId++;
						codeArray.push_back("move(0, " + reg_stack.top() + ");");
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
				c1->genCode(&c1Tl, &c1Fl);
				codeArray.push_back("label" + to_string(labelId) + ":");
				backPatch(c1Tl, labelId);
				labelId++;
				c2->genCode(&c2Tl, &c2Fl);
				if(is_cond)
				{
				    (*truelist) = c2Tl;
				    (*falselist) = c1Fl;
				    falselist->insert(falselist->end(), c2Fl.begin(), c2Fl.end());
				}
				else
				{
					if(need_val)
					{
						codeArray.push_back("label" + to_string(labelId) + ":");
						backPatch(c2Tl, labelId);
						labelId++;
						codeArray.push_back("move(1, " + reg_stack.top() + ");");
						int pos = codeArray.size();
						codeArray.push_back("j");
						codeArray.push_back("label" + to_string(labelId) + ":");
						backPatch(c1Fl, labelId);
						backPatch(c2Fl, labelId);
						labelId++;
						codeArray.push_back("move(0, " + reg_stack.top() + ");");
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
			string reg1, reg2;
			if(need_val || is_cond)
			{
				// Need the value of + on reg top
				if(c1->is_const){ // Second is not constant
					c2->genCode(&c2Tl, &c2Fl);
					codeArray.push_back("addi(" + to_string(c1->int_val) + ", " + reg_stack.top() + ");");
				}
				else if(c2->is_const){ // First is not const
					c1->genCode(&c1Tl, &c1Fl);
					codeArray.push_back("addi(" + to_string(c2->int_val) + ", " + reg_stack.top() + ");");
				}
				else{
					// Need to eval both
					
					reg1 = reg_stack.top();
					reg_stack.pop();
					reg2 = reg_stack.top();
					reg_stack.pop();
					if(c1->reg_label > c2->reg_label){
						reg_stack.push(reg2);
						reg_stack.push(reg1);
						c1->genCode(&c1Tl, &c1Fl);
						reg_stack.pop();
						reg_stack.pop();
						if(c2->reg_label > reg_stack.size() + 1){
							codeArray.push_back("pushi(" + reg1 + ");");
							reg_stack.push(reg1);
							reg_stack.push(reg2);
							c2->genCode(&c2Tl, &c2Fl);
							reg_stack.pop();
							reg_stack.pop();
							codeArray.push_back("loadi(ind(esp,0), " + reg1 + ");");
							codeArray.push_back("popi(1);");
						}
						else{
							reg_stack.push(reg2);
							c2->genCode(&c2Tl, &c2Fl);
							reg_stack.pop();
						}	
					}
					else
					{
						reg_stack.push(reg1);
						reg_stack.push(reg2);
						c2->genCode(&c2Tl, &c2Fl);
						reg_stack.pop();
						reg_stack.pop();
						
						if(c1->reg_label > reg_stack.size() + 1)
						{
							codeArray.push_back("pushi(" + reg2 + ");");
							reg_stack.push(reg2);
							reg_stack.push(reg1);
							c1->genCode(&c1Tl, &c1Fl);
							reg_stack.pop();
							reg_stack.pop();
							codeArray.push_back("loadi(ind(esp,0), " + reg2 + ");");
							codeArray.push_back("popi(1);");
						}
						else
						{
							reg_stack.push(reg1);
							c1->genCode(&c1Tl, &c1Fl);
							reg_stack.pop();
						}
					}
					codeArray.push_back("addi(" + reg2 + ", " + reg1 + ");");
					reg_stack.push(reg2);
					reg_stack.push(reg1);
				}
			}
			else
			{
				c1->genCode(&c1Tl, &c1Fl);
				c2->genCode(&c1Tl, &c1Fl);
			}
			
			if(is_cond)
			{
				codeArray.push_back("cmpi(0," + reg1 + ");"); 
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
		break;
		case OP_ASSIGN: // NEED TO DO
		{
			
			string idName = ((ArrayRef*)c1)->getArrayName();
			VarDeclaration vdecl = currentFuncTable.getVar(idName);
			int offset = vdecl.offset;
			
	        
	        c2->genCode(&c2Tl, &c2Fl); // R value
			// Assuming identifier
			codeArray.push_back("storei(" + reg_stack.top() + ", ind(ebp, " 
									+ to_string(offset) + ");" );
			

	        if(!is_cond)
	        {
		        if(need_val)
		        {
			        // Need not do anything special here
		        }
	        }
	        else
	        {
		        codeArray.push_back("cmpi(0," + reg_stack.top() + ");"); 
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
		break;
		case OP_INT_LT: // NEED TO DO
		{
			 c1->genCode(&c1Tl, &c1Fl);
			 c2->genCode(&c2Tl, &c2Fl);
            
            
				if(!is_cond)
				{
					if(need_val)
					{
						//assume intrn is a < b
						codeArray.push_back("loadi(ind(esp, 0), eax);"); // eax contains b
						codeArray.push_back("loadi(ind(esp, I), ebx);"); // ebx contains a
						codeArray.push_back("cmpi(ebx, eax);");
						codeArray.push_back("popi(2);");
						
						int trueLabel = labelId;
						++labelId;
						int nextLabel = labelId;
						++labelId;
						codeArray.push_back("jl(label"+to_string(trueLabel)+");");
						codeArray.push_back("pushi(0);");
						codeArray.push_back("j(label"+to_string(nextLabel)+");");
						codeArray.push_back("label" + to_string(trueLabel) + ":");
						codeArray.push_back("pushi(1);");
						codeArray.push_back("label" + to_string(nextLabel) +":");
					}
				}
				else
				{
						codeArray.push_back("loadi(ind(esp, 0), eax);");
						codeArray.push_back("loadi(ind(esp, I), ebx);");
						codeArray.push_back("cmpi(ebx, eax);");
						codeArray.push_back("popi(2);");
										 
						if(fall)
						{
								falselist->push_back(codeArray.size());
								codeArray.push_back("jge");
						}
						else
						{
								truelist->push_back(codeArray.size());
								codeArray.push_back("jl");
						}
				}
		}
		break;	
		
		case OP_INT_MINUS: // NEED TO DO
		{
			  c1->genCode(&c1Tl, &c1Fl);
			  c2->genCode(&c2Tl, &c2Fl);
            
				if(!is_cond)
				{
					if(need_val)
					{
						//assume intrn is a - b
						codeArray.push_back("loadi(ind(esp, 0), eax);"); // eax contains b
						codeArray.push_back("loadi(ind(esp, I), ebx);"); // ebx contains a
						codeArray.push_back("muli(-1, eax);"); // eax contains -b
						codeArray.push_back("addi(ebx, eax);"); // eax contains a-b
						codeArray.push_back("popi(2);");
						codeArray.push_back("pushi(eax);");
					}
				}
				else
				{
						codeArray.push_back("loadi(ind(esp, 0), eax);");
						codeArray.push_back("loadi(ind(esp, I), ebx);");
						codeArray.push_back("muli(-1, eax);"); // eax contains -b
						codeArray.push_back("addi(ebx, eax);"); // eax contains a-b
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
		break;
		
		}
}


/* *********************************************************************
 * END
 * BinaryOp
 **********************************************************************/


/* *********************************************************************
 * BEGIN
 * UnaryOp
 **********************************************************************/

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

void UnaryOp::calcAttributes() // TODO
{
	switch(op)
	{
		case OP_PP:
			c1->is_cond = false;
			c1->fall = false;
			c1->need_val = true;
			c1->calcAttributes();
		break;
	}
}

void UnaryOp::genCode(list<int> *truelist, list<int> *falselist) // TODO
{
	std::list<int> c1Tl, c1Fl;
	switch(op)
	{
		case OP_PP:
		{
			  //~ ValType valtype;
				//~ int varoffset;
				//~ bool isParam;
				//~ ((ArrayRef*)c1)->genLCode(&varoffset, &valtype,&isParam); // getting the address of the l_expression
//~ 
//~ 
				//~ if(valtype == TYPE_INT)
				//~ {
					//~ //assume instrn is x++
					//~ codeArray.push_back("loadi(ind(esp, 0), eax);"); // contains addr of x[i] relative to x
					//~ codeArray.push_back("popi(1);"); // clean the stack
					//~ if(!isParam)
					//~ {
						//~ codeArray.push_back("addi(ebp, eax);"); // add ebp
						//~ codeArray.push_back("loadi(ebx, ind(eax, " + to_string(varoffset) + "));");  // value of x is in ebx
						//~ codeArray.push_back("addi(1,ebx);"); // ebx is x + 1
						//~ codeArray.push_back("storei(ebx, ind(eax, " + to_string(varoffset) + "));");
						//~ codeArray.push_back("addi(-1,ebx);");
					//~ }
					//~ else
					//~ {
						//~ codeArray.push_back("loadi(ind(ebp," +  to_string(varoffset)+ ", ebx);");
						//~ codeArray.push_back("addi(ebx,eax);");
						//~ codeArray.push_back("loadi(ind(eax,0),ebx);");
						//~ codeArray.push_back("addi(ebx,1);");
						//~ codeArray.push_back("storei(ebx,ind(eax,0));");
						//~ codeArray.push_back("addi(-1,ebx);");
					//~ }
					//~ 
				//~ }
				//~ else
				//~ {
					//~ //assume instrn is x++
					//~ codeArray.push_back("loadi(ind(esp, 0), eax);"); // contains addr of x[i] relative to x
					//~ codeArray.push_back("popi(1);"); // clean the stack
					//~ if(!isParam)
					//~ {
						//~ codeArray.push_back("addi(ebp, eax);"); // add ebp
						//~ codeArray.push_back("loadf(ebx, ind(eax, " + to_string(varoffset) + "));");  // value of x is in ebx
						//~ codeArray.push_back("addf(1,ebx);"); // ebx is x + 1
						//~ codeArray.push_back("storef(ebx, ind(eax, " + to_string(varoffset) + "));");
						//~ codeArray.push_back("addf(-1,ebx);");
					//~ }
					//~ else
					//~ {
						//~ codeArray.push_back("loadi(ind(ebp," +  to_string(varoffset)+ ", ebx);");
						//~ codeArray.push_back("addi(ebx,eax);");
						//~ codeArray.push_back("loadi(ind(eax,0),ebx);");
						//~ codeArray.push_back("addf(ebx,1);");
						//~ codeArray.push_back("storef(ebx,ind(eax,0));");
						//~ codeArray.push_back("addf(-1,ebx);");		
					//~ }
				//~ }
//~ 
				//~ if(!is_cond)
				//~ {
					//~ if(need_val)
					//~ {
						//~ if(valtype == TYPE_INT)
						//~ {
							//~ codeArray.push_back("pushi(ebx);");
						//~ }
						//~ else
						//~ {
							//~ codeArray.push_back("pushf(ebx);");
						//~ }
					//~ }
				//~ }
				//~ else
				//~ {
					//~ codeArray.push_back("cmpi(0,ebx);"); 
					//~ if(fall)
					//~ {
						//~ falselist->push_back(codeArray.size());
						//~ codeArray.push_back("je");
					//~ }
					//~ else
					//~ {
						//~ truelist->push_back(codeArray.size());
						//~ codeArray.push_back("jne");
					//~ }
				//~ }
		}
		break;
	
	}
}

void UnaryOp::calcLabel() // TODO
{
	switch(op)
	{
		case OP_PP:
		if(c1->astnode_type == AST_IDENTIFIER)
		{
			reg_label = 1;
		}
		else
		{
			reg_label = max(2, c1->reg_label);
		}
		break;
	}
}

/* *********************************************************************
 * END
 * UnaryOp
 **********************************************************************/


/* *********************************************************************
 * BEGIN
 * FunCall 
 **********************************************************************/

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

void FunCall::calcLabel()
{
	cerr << "NEED TO DEFINE calcLabel in Funcall" << endl;
}

void FunCall::calcAttributes()
{
	for(auto it = list_exp_ast.begin(); it != list_exp_ast.end(); ++it)
	{
		(*it)->fall = false;
		(*it)->is_cond = false;
		(*it)->need_val = true;
		(*it)->calcAttributes();
	}
}

void FunCall::genCode(list<int>* tl, list<int>* fl)
{
	
	if(data_type.getPrimitiveType() == TYPE_INT)
	{
		codeArray.push_back("pushi(0);");
	}
	else if(data_type.getPrimitiveType() == TYPE_FLOAT)
	{
		codeArray.push_back("pushf(0.0);");
	}
	
	
		
	// First gen code for params.
	for(auto it = list_exp_ast.rbegin(); it != list_exp_ast.rend(); ++it)
	{
		(*it)->genCode(NULL, NULL);
	}
	FunctionSignature fsig(func_name, getArgTypeList());
	codeArray.push_back(uniq_fname + "();");
	
	for(auto it = list_exp_ast.begin(); it != list_exp_ast.end(); ++it)
	{
		if((*it)->data_type.isPrimitive())
		{
			if((*it)->data_type.getPrimitiveType() == TYPE_INT)
			{
				codeArray.push_back("popi(1);");
			}
			else if((*it)->data_type.getPrimitiveType() == TYPE_FLOAT)
			{
				codeArray.push_back("popf(1);");
			}
		}
		else
		{
			codeArray.push_back("popi(1);");
		}
	}
	
	
	if(!is_cond)
	{
		if(need_val)
		{
			
		}
		else
		{
			if(data_type.getPrimitiveType() == TYPE_INT)
			{
				codeArray.push_back("popi(1);");
			}
			else if(data_type.getPrimitiveType() == TYPE_FLOAT)
			{
				codeArray.push_back("popf(1);");
			}
		}
	}
	else
	{
		if(data_type.getPrimitiveType() == TYPE_INT)
		{
			codeArray.push_back("loadi(esp, eax);");
			codeArray.push_back("popi(1);");
			codeArray.push_back("cmpi(0,eax);"); 
		}
		else if(data_type.getPrimitiveType() == TYPE_FLOAT)
		{
			codeArray.push_back("loadf(esp, eax);");
			codeArray.push_back("popf(1);");
			codeArray.push_back("cmpf(0,eax);"); 
		}
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

/* *********************************************************************
 * END
 * FunCall 
 **********************************************************************/


/* *********************************************************************
 * BEGIN
 * Identifier
 **********************************************************************/

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


void Identifier::genCode(list<int>* tl, list<int>* fl)
{
	// Gets value in identifier. If lvalue, should not be called
	auto it = currentFuncTable.var_name_map.find(val);
	VarDeclaration curVarDecl = it->second;
	int idOffset = curVarDecl.offset;
	ValType idValType = curVarDecl.data_type.getPrimitiveType();
	if(idValType == TYPE_INT)
	{
		
		if(is_cond)
		{
			string reg = reg_stack.top();
			codeArray.push_back("loadi(ind(ebp,"+to_string(idOffset)+")," + reg + ");");
            codeArray.push_back("cmpi(0," + reg + ");");
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
			if(need_val)
			{
				string reg = reg_stack.top();
				codeArray.push_back("loadi(ind(ebp,"+to_string(idOffset)+")," + reg + ");");
			}
		}
	}
	else
	{
		if(is_cond)
		{
			string reg = reg_stack.top();
			codeArray.push_back("loadf(ind(ebp,"+to_string(idOffset)+")," + reg + ");");
            codeArray.push_back("cmpf(0," + reg + ");");
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
			if(need_val)
			{
				string reg = reg_stack.top();
				codeArray.push_back("loadf(ind(ebp,"+to_string(idOffset)+")," + reg + ");");
			}
		}
		
	}
}

std::string Identifier::getArrayName()
{
    return val;
}

void Identifier::calcAttributes()
{
	is_const = false;
	return;
}

void Identifier::calcLabel()
{
	if(is_cond || need_val)
	{
		reg_label = 1;
	}
	else
	{
		reg_label = 0;
	}
}

/* *********************************************************************
 * END
 * Identifier
 **********************************************************************/


/* *********************************************************************
 * BEGIN
 * Index
 **********************************************************************/


void Index::genOffset(list<int>* dimensions)
{
	
	// Generates the internal offset for an array
	if(need_val) // Actually need the offset
	{
		auto it = currentFuncTable.var_name_map.find(c1->getArrayName());
		VarDeclaration curVarDecl = it->second;
		DataType dt = curVarDecl.data_type;
		if(c1->astnode_type == AST_IDENTIFIER)
		{
			// First indexing
			
			*dimensions = dt.array_dims;
			dimensions->pop_front(); // Drop first dimension
			int multFactor;
			for(auto it = dimensions->begin();
				it != dimensions->end(); ++it)
			{
				multFactor *= (*it);
			}
			string typeSize;
			if(dt.getPrimitiveType() == TYPE_INT)
			{
				typeSize = "I";
			}
			else
			{
				typeSize = "F";
			}
			if(c2->is_const)
			{
				codeArray.push_back("move(" + typeSize + "*" + to_string(multFactor)
							+ "*" +to_string(c2->int_val) + ", " + reg_stack.top() + ");");
			}
			else
			{
				c2->genCode(NULL, NULL); // Get val
				codeArray.push_back("muli(" + typeSize + "*" + to_string(multFactor)
								+ ", " + reg_stack.top() + ");");
			}
			
		}
		else
		{
			string reg1, reg2;
			if(c2->is_const)
			{
				
				((Index*)c1)->genOffset(dimensions);
				int multFactor;
				
				for(auto it = dimensions->begin();
					it != dimensions->end(); ++it)
				{
					multFactor *= (*it);
				}
				string typeSize;
				if(dt.getPrimitiveType() == TYPE_INT)
				{
					typeSize = "I";
				}
				else
				{
					typeSize = "F";
				}
				
				codeArray.push_back("addi(" + typeSize + "*" + to_string(multFactor)
							+ "*" +to_string(c2->int_val) + ", " + reg_stack.top() + ");");
			}
			else
			{
				reg1 = reg_stack.top();
				reg_stack.pop();
				reg2 = reg_stack.top();
				reg_stack.pop();
				if(c1->reg_label > c2->reg_label){
					reg_stack.push(reg2);
					reg_stack.push(reg1);
					((Index*)c1)->genOffset(dimensions);
					reg_stack.pop();
					reg_stack.pop();
					if(c2->reg_label > reg_stack.size() + 1){
						codeArray.push_back("pushi(" + reg1 + ");");
						reg_stack.push(reg1);
						reg_stack.push(reg2);
						c2->genCode(NULL, NULL);
						reg_stack.pop();
						reg_stack.pop();
						codeArray.push_back("loadi(ind(esp,0), " + reg1 + ");");
						codeArray.push_back("popi(1);");
					}
					else{
						reg_stack.push(reg2);
						c2->genCode(NULL, NULL);
						reg_stack.pop();
					}	
				}
				else
				{
					reg_stack.push(reg1);
					reg_stack.push(reg2);
					c2->genCode(NULL, NULL);
					reg_stack.pop();
					reg_stack.pop();
					
					if(c1->reg_label > reg_stack.size() + 1)
					{
						codeArray.push_back("pushi(" + reg2 + ");");
						reg_stack.push(reg2);
						reg_stack.push(reg1);
						((Index*)c1)->genOffset(dimensions);
						reg_stack.pop();
						reg_stack.pop();
						codeArray.push_back("loadi(ind(esp,0), " + reg2 + ");");
						codeArray.push_back("popi(1);");
					}
					else
					{
						reg_stack.push(reg1);
						((Index*)c1)->genOffset(dimensions);
						reg_stack.pop();
					}
				}
				int multFactor;
				
				for(auto it = dimensions->begin();
					it != dimensions->end(); ++it)
				{
					multFactor *= (*it);
				}
				string typeSize;
				if(dt.getPrimitiveType() == TYPE_INT)
				{
					typeSize = "I";
				}
				else
				{
					typeSize = "F";
				}
				dimensions->pop_front();
				codeArray.push_back("muli(" + typeSize + "*" + to_string(multFactor)
									+ ", " + reg2 + ");");
				codeArray.push_back("addi(" + reg2 + ", " + reg1 + ");");
				reg_stack.push(reg2);
				reg_stack.push(reg1);
			}
		}
	}
	else
	{
		// Offset not required
		if(c1->astnode_type != AST_IDENTIFIER)
		{
			((Index*)c1)->genOffset(dimensions);
		}	
		c2->genCode(NULL, NULL);
	}
}


void Index::genCode(list<int>* tl, list<int>* fl)
{
	// Loads the value.
	if(need_val || is_cond)
	{
		std::list<int> dims;
		genOffset(&dims);
		// Now I have base relative offset on top
		string idName = getArrayName();
		auto it = currentFuncTable.var_name_map.find(idName);
		VarDeclaration curVarDecl = it->second;
		DataType dt = curVarDecl.data_type;
		if(curVarDecl.decl_type == PARAM) // NEED AN EXTRA REG
		{
			
			string reg1 = reg_stack.top(); // Has the offset
			reg_stack.pop();
			string reg2 = reg_stack.top(); // Can be used for loading
			reg_stack.pop();
			codeArray.push_back("loadi(ind(ebp, " + to_string(curVarDecl.offset )
							+ "), " + reg2 + ");");
			codeArray.push_back("addi(" + reg2 + ", " + reg1 + ");");
			if(dt.getPrimitiveType() == TYPE_INT)
			{
				codeArray.push_back("loadi(ind(" + reg1 + ", 0), " + reg1 + ");");
			}
			else
			{
				codeArray.push_back("loadf(ind(" + reg1 + ", 0), " + reg1 + ");");
			}
			reg_stack.push(reg2);
			reg_stack.push(reg1);
		}
		else 
		{
			codeArray.push_back("addi(ebp, " + reg_stack.top() + ");");
			if(dt.getPrimitiveType() == TYPE_INT)
			{
				codeArray.push_back("loadi(ind(" + reg_stack.top() + "," 
							+ to_string(curVarDecl.offset) + "), " + reg_stack.top() + ");");
			}
			else
			{
				codeArray.push_back("loadf(ind(" + reg_stack.top() + "," 
							+ to_string(curVarDecl.offset) + "), " + reg_stack.top() + ");");
			}
		}
	}
	else
	{
		list<int> dims;
		genOffset(&dims);
	}
	if(is_cond)
	{
		codeArray.push_back("cmpi(0," + reg_stack.top() + ");"); 
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


std::string Index::getArrayName()
{
    return (c1)->getArrayName();
}

void Index::calcAttributes()
{
	c1->is_cond = false;
	c1->fall = false;
	c1->need_val = is_cond || need_val;
	c2->is_cond = false;
	c2->fall = false;
	c2->need_val = is_cond || need_val;
	c1->calcAttributes();
	c2->calcAttributes();
	c1->is_const = false;
	c2->is_const = false;
}

void Index::calcLabel()
{
	c1->calcLabel();
	c2->calcLabel();
	if(is_cond || need_val)
	{
		if(c1->reg_label == c2->reg_label)
		{
			reg_label = c1->reg_label + 1;
		}
		else
		{
			reg_label = max(c1->reg_label, c2->reg_label);
		}
		if(reg_label <2) reg_label = 2;
	}
	else
	{
		max(c1->reg_label, c2->reg_label);
	}
}

/* *********************************************************************
 * END
 * Index
 **********************************************************************/


/* *********************************************************************
 * BEGIN
 * ArrayRef 
 **********************************************************************/

std::string ArrayRef::getArrayName()
{
    return "Arbit";
}

void ArrayRef::calcLabel()
{
	cerr << "ArrayRef: NEED TO OVERRIDE calcLabel()" << endl;
}

void ArrayRef::calcAttributes()
{
	cerr << "ArrayRef: NEED TO OVERRIDE calcAttributes()" << endl;
}

void ArrayRef::genCode(list<int>* tl, list<int>*fl)
{
	cerr << "ArrayRef: NEED TO OVERRIDE genCode(list<int>*, list<int>*)" << endl;
}

/* *********************************************************************
 * END
 * ArrayRef 
 **********************************************************************/

