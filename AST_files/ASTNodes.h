#ifndef _AST_NODES_
#define _AST_NODES_
#include <iostream>
#include <list>
#include <vector>
#include "Enums.h"
#include "SymbolTable.h"
#include "../Util/catter.h"
#include "TypeChecks.h"
using namespace std;


extern int tab_degree;

extern vector<string> codeArray;

extern int labelId;

extern FunctionTable currentFuncTable;

void indent_print(std::string s);

class abstract_astnode
{
  
private:
  bool valid = true;
public:
  AstType astnode_type;
  
  virtual void print () = 0;
  bool& validAST();
  bool const& validAST() const;
  virtual void genCode();
};


class ExpAst : public abstract_astnode
{
public:
  DataType data_type;
  
  DataType& dataType();
  virtual void genCode(bool fall, bool iscond, bool onstack, list<int>* tl, list<int>* fl) ;
};


class StmtAst : public abstract_astnode
{
public:
	virtual void genCode();
	virtual void genCode(list <int> *nextList);
};

class ProgAst : public abstract_astnode
{
private:
	list<StmtAst*> funcList;
public:
	void addFunctionDef(StmtAst*);
	void genCode();
	void print();
};


class ArrayRef : public ExpAst
{
private:
  // AstType astnode_type;
public:
   virtual std::string getArrayName();
   virtual void genLCode(int* offset, ValType* valtype) = 0; // genCode for lvalue
   virtual void genCode(bool fall, bool iscond, bool onstack, list<int>* tl, list<int>* fl) = 0; 
   virtual void genCode(int*, ValType*, bool onstack,  list<int>*) = 0;
};

class Empty : public StmtAst
{
private:
  // AstType astnode_type;
public:
  Empty();
  void print();
};


class Block : public StmtAst
{
private:
  // AstType astnode_type;
  std::list<StmtAst*> clist;
public:
  Block(StmtAst* _c);
  void print();
  void insert(StmtAst* c);
  void genCode();
  void genCode(list <int> *nextList);
};


class ExpStmt : public StmtAst
{
   private:
     // AstType astnode_type;
     ExpAst* c1;
public:
    virtual void genCode();
    virtual void genCode(list<int> *nextList);
  ExpStmt(ExpAst* exp);
  void print();
};

class Ass : public StmtAst
{
private:
  // AstType astnode_type;
  ExpAst* c1;
  ExpAst* c2;
public:
  Ass(ExpAst* left_stmt, ExpAst* right_stmt);
  void print();
  void genCode(bool, bool, bool, list<int>*, list<int>*);
};

class Return : public StmtAst
{
private:
  // AstType astnode_type;
  ExpAst* c1;
public:
  Return(ExpAst* ret_exp);
  void print();
};


class If : public StmtAst
{
private:
  // AstType astnode_type;
  ExpAst* c1;
  StmtAst* c2;
  StmtAst* c3;
public:
  If(ExpAst* cond, StmtAst* if_stats, StmtAst* else_stats);
  void print();
  void genCode(list <int> *nextList);
};


class For : public StmtAst
{
private:
  // AstType astnode_type;
  ExpAst* c1;
  ExpAst* c2;
  ExpAst* c3;
  StmtAst* c4;
public:
  For(ExpAst* initialize, ExpAst* guard, ExpAst* update, StmtAst* forbody);
  void print();
  void genCode(list <int> *nextList);
  
};

class While : public StmtAst
{
private:
  // AstType astnode_type;
  ExpAst* c1;
  StmtAst* c2;
public:
  While(ExpAst* guard, StmtAst* whilebody);
  void print();
  void genCode(list <int> *nextList);
};

class FloatConst : public ExpAst
{
private:
  // AstType astnode_type;
  
public:
  float val;
  FloatConst(float _val);
  void print();
  void genCode(bool fall, bool iscond, bool onstack, list<int>* tl, list<int>* fl);
};

class BinaryOp : public ExpAst
{
private:
  // AstType astnode_type;
  ExpAst* c1;
  ExpAst* c2;
  OpType op;
public:
  BinaryOp(ExpAst* left_exp , ExpAst* right_exp, OpType _op);
  void print();
  void genCode(bool fall, bool iscond, bool onstack, list<int>* tl, list<int>* fl);
};

class UnaryOp : public ExpAst
{
private:
  // AstType astnode_type;
  ExpAst* c1;
  OpType op;
public:
  UnaryOp(ExpAst* exp, OpType _op);
  void print();
  void genCode(bool fall,bool iscond, bool onstack, list<int> *truelist, list<int> *falselist);
};

class FunCall : public ExpAst
{
private:
  // AstType astnode_type;
  
  string func_name;
public:
  list<ExpAst*> list_exp_ast;
  FunCall(ExpAst* _exp_ast);
  void setName(string fname);
  void print();
  std::list<DataType> getArgTypeList();
  void insert(ExpAst* e);
};


class IntConst : public ExpAst
{
private:
  // AstType astnode_type;
  
public:
  int val;
  IntConst(int _val);
  void print();
  void genCode(bool fall, bool iscond, bool onstack, list<int>* tl, list<int>* fl);
};

class StringConst : public ExpAst
{
private:
  // AstType astnode_type;
  std::string val;
public:
  StringConst(std::string _val);
  void print();
};


class Identifier : public ArrayRef
{
private:
  // AstType astnode_type;
  std::string val;
public:
  Identifier(std::string _val);
  std::string getArrayName();
  void print();
  void genLCode(int* offset, ValType* valtype);
  void genCode(bool fall, bool iscond, bool onstack, list<int>* tl, list<int>* fl);
  void genCode(int *idOffset , ValType *idValType, bool onstack, list <int> *remainingDim);
};

class Index : public ArrayRef
{
private:
  // AstType astnode_type;
  ArrayRef* c1;
  ExpAst* c2;
public:
  Index(ArrayRef* arrRef , ExpAst* expAst);
  std::string getArrayName();
  void print();
  void genLCode(int* offset, ValType* valtype);
  void genCode(bool fall, bool iscond, bool onstack, list<int>* tl, list<int>* fl);
  void genCode(int *idOffset , ValType *idValType, bool onstack, list <int> *remainingDim);
};


#endif
