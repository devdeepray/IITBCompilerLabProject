#ifndef _AST_NODES_
#define _AST_NODES_
#include <iostream>
#include <list>
#include <vector>
#include "Enums.h"
#include "SymbolTable.h"
#include "../Util/catter.h"
#include "TypeChecks.h"
#include <stack>
#include <map>
using namespace std;

extern int tab_degree;
extern vector<string> codeArray;
extern int labelId;
extern FunctionTable currentFuncTable;
extern vector<string> reg_stack;
extern map<string, string> reg_info;

void indent_print(std::string s);
void backPatch(list<int>, int);
void init_reg_stack();

class abstract_astnode
{
public:
  bool valid = true;
  AstType astnode_type;
  
  virtual void print ();
  bool& validAST();
};

class ExpAst : public abstract_astnode
{
public:
  int reg_label = 0; // Sethi ullman labels
  bool is_cond = false; // Whether exp is part of cond or not
  bool need_val = false; // Whether the result value is needed or not
  bool fall = false; // For fall through code
  
  bool dir_const = false; // Whether constant can be used directly
  bool is_const = false; // For constants, synth attr
  int int_val = 0; // Synth attr
  double float_val = 0; // Synth attr
  DataType data_type;  // Data type of expr
  
  DataType& dataType();
  virtual void genCode(list<int>* tl, list<int>* fl);
  virtual void calcLabel(); // Calculates sethi-ullman labels
  virtual void calcAttributes(); // calculate is_cond and need_val
  virtual void pruneAST(); // Prune constant subtrees
};


class StmtAst : public abstract_astnode
{
public:
	virtual void genCode(list <int> *nextList);
};

class ProgAst : public abstract_astnode
{
public:
	list<StmtAst*> funcList;

	void addFunctionDef(StmtAst*);
	void genCode();
	void print();
};




/* *********************************************************************
 * Inherit from StmtAst
 **********************************************************************/
 
class Empty : public StmtAst
{
public:
  Empty();
  void print();
};


class Block : public StmtAst
{
public:
  std::list<StmtAst*> clist;
  Block(StmtAst* _c);
  void print();
  void insert(StmtAst* c);
  void genCode(list <int> *nextList);
};


class ExpStmt : public StmtAst
{
public:
  ExpAst* c1;
  void genCode(list<int> *nextList);
  ExpStmt(ExpAst* exp);
  void print();
};

class Return : public StmtAst
{
public:
  ExpAst* c1;
  Return(ExpAst* ret_exp);
  void print();
  void genCode(list<int> *nextList);
};


class If : public StmtAst
{
public:
  ExpAst* c1;
  StmtAst* c2;
  StmtAst* c3;
  If(ExpAst* cond, StmtAst* if_stats, StmtAst* else_stats);
  void print();
  void genCode(list <int> *nextList);
};


class For : public StmtAst
{
public:
  ExpAst* c1;
  ExpAst* c2;
  ExpAst* c3;
  StmtAst* c4;
  For(ExpAst* initialize, ExpAst* guard, ExpAst* update, StmtAst* forbody);
  void print();
  void genCode(list <int> *nextList);
  
};

class While : public StmtAst
{
public:
  ExpAst* c1;
  StmtAst* c2;
  While(ExpAst* guard, StmtAst* whilebody);
  void print();
  void genCode(list <int> *nextList);
};


/* *********************************************************************
 * Inherit from ExpAst
 **********************************************************************/


class FloatConst : public ExpAst
{ 
public:
  FloatConst(float _val);
  void print();
  void genCode(list<int>* tl, list<int>* fl);
  void calcLabel();
  void calcAttributes();
  void pruneAST();
};


class IntConst : public ExpAst
{  
public:
  IntConst(int _val);
  void print();
  void genCode(list<int>* tl, list<int>* fl);
  void calcLabel();
  void calcAttributes();
  void pruneAST();
};


class BinaryOp : public ExpAst
{
public:
  ExpAst* c1;
  ExpAst* c2;
  OpType op;
  BinaryOp(ExpAst* left_exp , ExpAst* right_exp, OpType _op);
  void print();
  void genCode(list<int>* tl, list<int>* fl);
  void calcLabel();
  void calcAttributes();
  void pruneAST();
};


class ArrayRef : public ExpAst
{
public:
   virtual std::string getArrayName();
   virtual void genCode(list<int>* tl, list<int>* fl); 
   virtual void calcLabel(); 
   virtual void calcAttributes();
   virtual void pruneAST();
};


class UnaryOp : public ExpAst
{
public:
  ExpAst* c1;
  OpType op;
  UnaryOp(ExpAst* exp, OpType _op);
  void print();
  void genCode(list<int> *truelist, list<int> *falselist);
  void calcLabel();
  void calcAttributes();
  void pruneAST();
};

class FunCall : public ExpAst
{
public:
  string func_name;
  list<ExpAst*> list_exp_ast;
  std::string uniq_fname;
  FunCall(ExpAst* _exp_ast);
  void setName(string fname);
  void print();
  std::list<DataType> getArgTypeList();
  void insert(ExpAst* e);
  void calcLabel();
  void calcAttributes();
  void genCode(list<int>*, list<int>*);
  void pruneAST();
};



class StringConst : public ExpAst
{
public:
  std::string val;
  StringConst(std::string _val);
  void print();
  void calcLabel();
  void calcAttributes();
  void pruneAST();
};


/* *********************************************************************
 * Inherit from ArrayRef
 **********************************************************************/

class Identifier : public ArrayRef
{
public:
  std::string val;
  Identifier(std::string _val);
  std::string getArrayName();
  void print();
  void genCode(list<int>* tl, list<int>* fl);
  void calcLabel();
  void calcAttributes();
  void pruneAST();
  
};

class Index : public ArrayRef
{
public:
  ArrayRef* c1;
  ExpAst* c2;
  Index(ArrayRef* arrRef , ExpAst* expAst);
  std::string getArrayName();
  void print();
  void genOffset(list<int>* dims);
  void genCode(list<int>* tl, list<int>* fl);
  
  void calcLabel();
  void calcAttributes();
  void pruneAST();
};


#endif
