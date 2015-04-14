
#include <iostream>
#include <list>
#include <vector>
#include "ASTNodes.h"
#include "../Util/catter.h"
#include <stack>
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


// For printing with indentation
void indent_print(std::string s) 
{
  for(int i = 1; i < tab_degree; ++i)
    std::cout << "  ";
  std::cout << s;
}

// FOr initializing register stack
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
	cerr << "ExpAst: NEED TO OVERRIDE genCode(list<int>*, list<int>*)" << endl;
}

void ExpAst::calcAttributes()
{
	cerr << "ExpAst: NEED TO OVERRIDE calcAttributes()" << endl;
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

// Add a statement to the block
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
    c1->calcAttributes();
	c1->calcLabel(); // Calculates all labels in the exp tree
	
	// Generate code for the expression
    c1->genCode(NULL, NULL);
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

// Print return AST
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
	// Leave the nextlist as is
	c1->fall = false; /*garbage*/
	c1->is_cond = false;
	c1->need_val = true;
	c1->calcAttributes();
	c1->calcLabel();
	c1->genCode(NULL, NULL);
	int returnValueAdd;
	// load the value from stack into eax, we have the value to be returned in eax 
	ValType returnVal = currentFuncTable.getReturnType().getPrimitiveType();
	
	if(currentFuncTable.var_offset_map.rbegin() == currentFuncTable.var_offset_map.rend()
		|| currentFuncTable.var_offset_map.rbegin()->first < 0)
	{
		returnValueAdd = 4;
	}
	else
	{
		returnValueAdd = currentFuncTable.var_offset_map.rbegin()->first;
		returnValueAdd += currentFuncTable.var_name_map[currentFuncTable.var_offset_map[returnValueAdd]].size;
	}
	
	if(returnVal == TYPE_INT)
	{
		codeArray.push_back("loadi(ind(esp, 0), eax);");
		codeArray.push_back("storei(eax, ind(ebp," + to_string(returnValueAdd) +"));");
	}
	else if(returnVal == TYPE_FLOAT)
	{
		codeArray.push_back("loadf(ind(esp, 0), eax);");
		codeArray.push_back("storef(eax, ind(ebp," + to_string(returnValueAdd) +"));");
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
	list <int> c4Nl;
	list<int> c1Tl, c1Fl;
	list<int> c2Tl, c2Fl;
	list<int> c3Tl, c3Fl;
	
	//Initiallization
	c1->fall = false;
	c1->is_cond = false;
	c1->need_val = false;
	c1->calcAttributes();
	c1->calcLabel();
	c1->genCode(&c1Tl, &c1Fl); // Fall, is_cond, need_val,truelist, falselist
	
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
	c1->genCode(&c1Tl, &c1Fl); // Fall, is_cond, need_val,truelist, falselist
	
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
  val = _val;
}
void FloatConst::print()
{
  tab_degree++;
  
  indent_print( "(FloatConst " );
  cout << val << ")" ;
  tab_degree--;
}

void FloatConst::genCode(list<int>* tl, list<int>* fl)
{
	if(!is_cond)
    {
		if(need_val)
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

void FloatConst::calcLabel()
{
	if(need_val)
	{
		reg_label = 1;
	}
	else
	{
		reg_label = 0;
	}
}

void FloatConst::calcAttributes()
{
	return; // All attributes already set
}
///////////////////////// END FLOAT CONSTANTS //////////////////////////


///////////////////////// START INT CONSTANTS //////////////////////////
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

void IntConst::genCode(list<int>* tl, list<int>* fl)
{
    if(!is_cond)
    {
		if(need_val)
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

void IntConst::calcLabel()
{
	if(need_val)
	{
		reg_label = 1;
	}
	else
	{
		reg_label = 0;
	}
}

void IntConst::calcAttributes()
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
	if(is_cond)
	{
		reg_label = max(c1->reg_label, c2->reg_label);
	}
	else
	{
		if(need_val)
		{
			if(c1->reg_label == c2->reg_label)
			{
				reg_label = c1->reg_label + 1;
			}
			else
			{
				reg_label = max(c1->reg_label, c2->reg_label);
			}
		}
		else
		{
			reg_label = max(c1->reg_label, c2->reg_label);
		}
	}
}

void BinaryOp::calcAttributes()
{
	switch(op)
	{
		case OP_OR:
			c1->is_cond = true;
			c1->fall = false;
			c1->need_val = false;
			c2->is_cond = true;
			c2->fall = fall || !is_cond; // If i want to eval, convenient to make it fall if true
			c2->need_val = false;
			break;
		case OP_AND:
			c1->is_cond = true;
			c1->fall = true;
			c1->need_val = false;
			c2->is_cond = true || !is_cond;
			c2->fall = fall;
			c2->need_val = false;
			break;
		case OP_INT_PLUS:
			c1->is_cond = false;
			c1->fall = false;
			c1->need_val = is_cond || need_val;
			c2->is_cond = false;
			c2->fall = false;
			c2->need_val = is_cond || need_val;
			break;
		case OP_ASSIGN:
			c1->is_cond = false;
			c1->fall = false;
			c1->need_val = true;
			c2->is_cond = false;
			c2->fall = false;
			c2->need_val = true;
			break;
		case OP_INT_LT:
			c1->is_cond = false;
			c1->fall = false;
			c1->need_val = is_cond || need_val;
			c2->is_cond = false;
			c2->fall = false;
			c2->need_val = is_cond || need_val;
			break;
		case OP_INT_MINUS:
			c1->is_cond = false;
			c1->fall = false;
			c1->need_val = is_cond || need_val;
			c2->is_cond = false;
			c2->fall = false;
			c2->need_val = is_cond || need_val;
			break;
	}
	c1->calcAttributes();
	c2->calcAttributes();
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
            c1->genCode(&c1Tl, &c1Fl);
            c2->genCode(&c2Tl, &c2Fl);
            
            
            if(!is_cond)
            {
				if(need_val)
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
		break;
		case OP_ASSIGN:
		{
		
		    ValType valtype;
	        int varoffset;
	        bool isParam;
	        ((ArrayRef*)c1)->genLCode(&varoffset, &valtype, &isParam);
	        c2->genCode(&c2Tl, &c2Fl);
	
	
	        if(valtype == TYPE_INT)
	        {
		        codeArray.push_back("loadi(ind(esp, I), ebx);"); // Load offset from var
		        if(!isParam)
		        {
					codeArray.push_back("loadi(ind(esp, 0), eax);"); // Load rval
					codeArray.push_back("addi(ebp, ebx);");
					codeArray.push_back("storei(eax, ind(ebx, " + to_string(varoffset) + "));"); 
				}
				else
				{
					codeArray.push_back("loadi(ind(ebp," +  to_string(varoffset)+ "), eax);");
					codeArray.push_back("addi(eax, ebx);");
					codeArray.push_back("loadi(ind(esp, 0), eax);");
					codeArray.push_back("storei(eax, ind(ebx, 0));");
				}
				codeArray.push_back("popi(2);");
	        }
	        else
	        {
		       codeArray.push_back("loadi(ind(esp, F), ebx);"); // Load offset from var
		        if(!isParam)
		        {
					codeArray.push_back("loadf(ind(esp, 0), eax);"); // Load rval
					codeArray.push_back("addi(ebp, ebx);");
					codeArray.push_back("storef(eax, ind(ebx, " + to_string(varoffset) + "));"); 
				}
				else
				{
					codeArray.push_back("loadi(ind(ebp," +  to_string(varoffset)+ "), eax);");
					codeArray.push_back("addi(eax, ebx);");
					codeArray.push_back("loadf(ind(esp, 0), eax);");
					codeArray.push_back("storef(eax, ind(ebx, 0));");
				}
				codeArray.push_back("popf(1);");
				codeArray.push_back("popi(1);");
	        }

	        if(!is_cond)
	        {
		        if(need_val)
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
		case OP_INT_LT:
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
		
		case OP_INT_MINUS:
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

void UnaryOp::calcAttributes()
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

void UnaryOp::genCode(list<int> *truelist, list<int> *falselist)
{
	std::list<int> c1Tl, c1Fl;
	switch(op)
	{
		case OP_PP:
		{
			  ValType valtype;
				int varoffset;
				bool isParam;
				((ArrayRef*)c1)->genLCode(&varoffset, &valtype,&isParam); // getting the address of the l_expression


				if(valtype == TYPE_INT)
				{
					//assume instrn is x++
					codeArray.push_back("loadi(ind(esp, 0), eax);"); // contains addr of x[i] relative to x
					codeArray.push_back("popi(1);"); // clean the stack
					if(!isParam)
					{
						codeArray.push_back("addi(ebp, eax);"); // add ebp
						codeArray.push_back("loadi(ebx, ind(eax, " + to_string(varoffset) + "));");  // value of x is in ebx
						codeArray.push_back("addi(1,ebx);"); // ebx is x + 1
						codeArray.push_back("storei(ebx, ind(eax, " + to_string(varoffset) + "));");
						codeArray.push_back("addi(-1,ebx);");
					}
					else
					{
						codeArray.push_back("loadi(ind(ebp," +  to_string(varoffset)+ ", ebx);");
						codeArray.push_back("addi(ebx,eax);");
						codeArray.push_back("loadi(ind(eax,0),ebx);");
						codeArray.push_back("addi(ebx,1);");
						codeArray.push_back("storei(ebx,ind(eax,0));");
						codeArray.push_back("addi(-1,ebx);");
					}
					
				}
				else
				{
					//assume instrn is x++
					codeArray.push_back("loadi(ind(esp, 0), eax);"); // contains addr of x[i] relative to x
					codeArray.push_back("popi(1);"); // clean the stack
					if(!isParam)
					{
						codeArray.push_back("addi(ebp, eax);"); // add ebp
						codeArray.push_back("loadf(ebx, ind(eax, " + to_string(varoffset) + "));");  // value of x is in ebx
						codeArray.push_back("addf(1,ebx);"); // ebx is x + 1
						codeArray.push_back("storef(ebx, ind(eax, " + to_string(varoffset) + "));");
						codeArray.push_back("addf(-1,ebx);");
					}
					else
					{
						codeArray.push_back("loadi(ind(ebp," +  to_string(varoffset)+ ", ebx);");
						codeArray.push_back("addi(ebx,eax);");
						codeArray.push_back("loadi(ind(eax,0),ebx);");
						codeArray.push_back("addf(ebx,1);");
						codeArray.push_back("storef(ebx,ind(eax,0));");
						codeArray.push_back("addf(-1,ebx);");		
					}
				}

				if(!is_cond)
				{
					if(need_val)
					{
						if(valtype == TYPE_INT)
						{
							codeArray.push_back("pushi(ebx);");
						}
						else
						{
							codeArray.push_back("pushf(ebx);");
						}
					}
				}
				else
				{
					codeArray.push_back("cmpi(0,ebx);"); 
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

void UnaryOp::calcLabel()
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

void Identifier::calcAttributes()
{
	return;
}

void Identifier::genCode(int *idOffset , ValType *idValType,bool* isParam, bool need_val, list <int> *remainingDim)
{
	// It sets idOffset to symbolTable offset
	// It sets valtype to valtype in sym table
	// Sets isParam
	// Initializes remaining Dim
	// Puts 0 on stack if value is needed
	// This is called by the arrayref AST.
    auto it = currentFuncTable.var_name_map.find(val);
    
    
    VarDeclaration curVarDecl = it->second;
    
    *idOffset = curVarDecl.offset;
    
    *idValType = curVarDecl.data_type.getPrimitiveType();
    *remainingDim = curVarDecl.data_type.array_dims;
    *isParam = (curVarDecl.decl_type == PARAM) && !curVarDecl.data_type.isPrimitive();
    
    if(need_val)
    {
		codeArray.push_back("pushi(0);");
	}
}

void Identifier::genLCode(int *offset, ValType *valtype, bool* isParam)
{
	// Puts 0 on stack always, and initialize valType and isParam and offset
	// Only called for generating lcode for simple identifier
    list<int> dummy;
    genCode(offset, valtype, isParam, true, &dummy);
}

void Identifier::genCode(list <int> *tl, list <int> *fl)
{
	// Puts the 'value' of the identifier on the stack if needed
	
	// It is just an identifier, not an array reference.
    auto it = currentFuncTable.var_name_map.find(val);
    VarDeclaration curVarDecl = it->second;
    int idOffset = curVarDecl.offset;
    ValType idValType = curVarDecl.data_type.getPrimitiveType();
    
    if(idValType == TYPE_INT)
    {
        
        if(!is_cond)
        {
			if(need_val)
			{
				codeArray.push_back("loadi(ind(ebp,"+to_string(idOffset)+"),eax);");
				codeArray.push_back("pushi(eax);");
			}
        }
        else
        {
			codeArray.push_back("loadi(ind(ebp,"+to_string(idOffset)+"),eax);");
            codeArray.push_back("cmpi(0,eax);");
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
        
        if(!is_cond)
        {
			if(need_val)
			{
				codeArray.push_back("loadf(ind(ebp,"+to_string(idOffset)+"),eax);");
				codeArray.push_back("pushf(eax);");
			}
        }
        else
        {
			codeArray.push_back("loadf(ind(ebp,"+to_string(idOffset)+"),eax);");
            codeArray.push_back("cmpf(0,eax);");
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

std::string Identifier::getArrayName()
{
    return val;
}

void Identifier::calcLabel()
{
	reg_label = 1;
}

/* *********************************************************************
 * END
 * Identifier
 **********************************************************************/


/* *********************************************************************
 * BEGIN
 * Index
 **********************************************************************/

void Index::genCode(int *idOffset, ValType *idValType, bool* isParam, bool need_val, list<int>* remainingDim)
{
	// Generates the code for calculating the offset inside the array and puts it on the stack
	// Also calculates offset of the identifier from ebp, returned in idOffset, and valtype, isparam, remdim
	list<int> c2Tl, c2Fl;
	c1->genCode(idOffset, idValType, isParam, need_val, remainingDim);
	c2->genCode(&c2Tl, &c2Fl);
	int curdim = remainingDim->front();
	remainingDim->pop_front();
	if(need_val)
	{
		codeArray.push_back("loadi(ind(esp, I), eax);");
		codeArray.push_back("loadi(ind(esp, 0), ebx);");
		codeArray.push_back("muli(" + to_string(curdim) + ", eax);");
		codeArray.push_back("addi(ebx, eax);");
		codeArray.push_back("popi(2);");
		codeArray.push_back("pushi(eax);");
	}

}

void Index::genLCode(int *offset, ValType *valtype, bool* isParam)
{
	// Calculates the relative offset inside array, mult by the size of I or F
    list<int> dimarray;
    genCode(offset, valtype, isParam, true, &dimarray);
    codeArray.push_back("loadi(ind(esp, 0), eax);");
    if((*valtype) == TYPE_INT)
    {
        codeArray.push_back("muli(eax, I);");
    }
    else
    {
        codeArray.push_back("muli(eax, F);");
    }
    codeArray.push_back("storei(eax, ind(esp, 0));");
    
}

void Index::genCode(list<int>* tl, list<int>* fl)
{
	// Puts the value on the stack if needed
    int idOffset;
    ValType idValType;
    list <int> remainingDim;
    list <int> c2Tl,c2Fl;
    bool isParam;
    c1->genCode(&idOffset, &idValType, &isParam, need_val || is_cond, &remainingDim);
    c2->genCode(&c2Tl, &c2Fl);
    
    
    
    
    if((need_val || is_cond) && remainingDim.size() == 1)
    {
		// The case where fully dereferenced
		codeArray.push_back("loadi(ind(esp, I), eax);"); // getting the partial offset calculation
		codeArray.push_back("loadi(ind(esp, 0), ebx);"); // loading the last index into ebx
		codeArray.push_back("muli(" + to_string(remainingDim.front())+ ",eax);"); //multiplying with dimension, holding in eax
		codeArray.push_back("addi(ebx,eax);"); //final offset from ebp
		codeArray.push_back("muli(" + string(((idValType == TYPE_INT)?"I" : "F")) + ", eax);");
		if(!isParam)
		{
			codeArray.push_back("addi(ebp, eax);");
		}
		else
		{
			codeArray.push_back("loadi(ind(ebp, "+to_string(idOffset)+"), ebx);");
			codeArray.push_back("addi(ebx, eax);");
		}
		codeArray.push_back("popi(2);");
	}
	else if(need_val || is_cond)
	{
		// The case where partially dereferenced
		codeArray.push_back("loadi(ind(esp, I), eax);"); // getting the partial offset calculation
		codeArray.push_back("loadi(ind(esp, 0), ebx);"); // loading the last index into ebx
		codeArray.push_back("muli(" + to_string(remainingDim.front())+ ",eax);"); //multiplying with dimension, holding in eax
		codeArray.push_back("addi(ebx,eax);"); //final offset from ebp
		
		int tot_size = 1;
		auto it = remainingDim.begin();
		++it;
		for(; it != remainingDim.end(); ++it)
		{
			tot_size *= (*it);
		}
		codeArray.push_back("muli(" + string(((idValType == TYPE_INT)?("I * " + to_string(tot_size)) : ("F * " + to_string(tot_size)))) + ", eax);");
		if(!isParam)
		{
			codeArray.push_back("addi(ebp, eax);");
		}
		else
		{
			codeArray.push_back("loadi(ind(ebp, "+to_string(idOffset)+"), ebx);");
			codeArray.push_back("addi(ebx, eax);");
		}
		codeArray.push_back("popi(2);");
	}

	if(idValType == TYPE_INT)
	{
		
		if(is_cond)
		{
			if(!isParam)
			{
				codeArray.push_back("loadi(ind(eax, "+ to_string(idOffset) + "), eax);");
			}
			else
			{
				codeArray.push_back("loadi(ind(eax, 0), eax);");
			}
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
			if(need_val)
			{
				if(remainingDim.size() == 1)
				{
					if(!isParam)
					{
						codeArray.push_back("loadi(ind(eax, "+ to_string(idOffset) + "), eax);");
					}
					else
					{
						codeArray.push_back("loadi(ind(eax, 0), eax);");
					}
					codeArray.push_back("pushi(eax);");
				}
				else
				{
					if(!isParam)
					{
						codeArray.push_back("addi(" + to_string(idOffset) +", eax);");
					}
					else
					{
					}
					codeArray.push_back("pushi(eax);");
				}
			}
		}
	}
	
	else if(idValType == TYPE_FLOAT)
	{
		
		if(is_cond)
		{
			if(!isParam)
			{
				codeArray.push_back("loadf(ind(eax, "+ to_string(idOffset) + "), eax);");
			}
			else
			{
				codeArray.push_back("loadf(ind(eax, 0), eax);");
			}
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
			if(need_val)
			{
				if(remainingDim.size() == 1)
				{
					if(!isParam)
					{
						codeArray.push_back("loadf(ind(eax, "+ to_string(idOffset) + "), eax);");
					}
					else
					{
						codeArray.push_back("loadf(ind(eax, 0), eax);");
					}
					codeArray.push_back("pushf(eax);");
				}
				else
				{
					if(!isParam)
					{
						codeArray.push_back("addi(" + to_string(idOffset) +", eax);");
					}
					else
					{
					}
					codeArray.push_back("pushi(eax);");
				}
				
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
}

void Index::calcLabel()
{
	c1->calcLabel();
	c2->calcLabel();
	if(is_cond)
	{
		reg_label = max(c1->reg_label, c2->reg_label);
	}
	else
	{
		if(need_val)
		{
			if(c1->reg_label == c2->reg_label)
			{
				reg_label = c1->reg_label + 1;
			}
			else
			{
				reg_label = max(c1->reg_label, c2->reg_label);
			}
		}
		else
		{
			reg_label = max(c1->reg_label, c2->reg_label);
		}
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

void ArrayRef::genCode(int* offset, ValType* vtype, bool* isParam, bool onstack, list<int>* rdim)
{
	cerr << "ArrayRef: NEED TO OVERRIDE genCode(int* offset, ValType* vtype, bool onstack, list<int>* rdim)" << endl;
}

void ArrayRef::genLCode(int* offset, ValType* valtype , bool* isParam)
{
	cerr << "ArrayRef: NEED TO OVERRIDE genLCode(int* offset, ValType* valtype)" << endl;
}
/* *********************************************************************
 * END
 * ArrayRef 
 **********************************************************************/

