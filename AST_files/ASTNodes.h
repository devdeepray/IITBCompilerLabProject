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


void indent_print(std::string s);

class abstract_astnode
{
  
private:
  AstType astnode_type;
  bool valid = true;
public:
  
  virtual void print () = 0;
  bool& validAST();
  bool const& validAST() const;
  //virtual std::string generate_code(const symbolTable&) = 0;
  //virtual basic_types getType() = 0;
  //virtual bool checkTypeofAST() = 0;
protected:
  //virtual void setType(basic_types) = 0;

};


class ExpAst : public abstract_astnode
{
public:
  AstType astnode_type;
  ValType val_type;
  ValType& valType();
};


class StmtAst : public abstract_astnode
{
private:
  AstType astnode_type;
};

class ArrayRef : public ExpAst
{
private:
  AstType astnode_type;
};

class Empty : public StmtAst
{
private:
  AstType astnode_type;
public:
  Empty();
  void print();
};


class Block : public StmtAst
{
private:
  AstType astnode_type;
  std::list<StmtAst*> clist;
public:
  Block(StmtAst* _c);
  void print();
  void insert(StmtAst* c);
};

class Ass : public StmtAst
{
private:
  AstType astnode_type;
  ExpAst* c1;
  ExpAst* c2;
public:
  Ass(ExpAst* left_stmt, ExpAst* right_stmt);
  void print();
};

class Return : public StmtAst
{
private:
  AstType astnode_type;
  ExpAst* c1;
public:
  Return(ExpAst* ret_exp);
  void print();
};


class If : public StmtAst
{
private:
  AstType astnode_type;
  ExpAst* c1;
  StmtAst* c2;
  StmtAst* c3;
public:
  If(ExpAst* cond, StmtAst* if_stats, StmtAst* else_stats);
  void print();
};


class For : public StmtAst
{
private:
  AstType astnode_type;
  ExpAst* c1;
  ExpAst* c2;
  ExpAst* c3;
  StmtAst* c4;
public:
  For(ExpAst* initialize, ExpAst* guard, ExpAst* update, StmtAst* forbody);
  void print();
};

class While : public StmtAst
{
private:
  AstType astnode_type;
  ExpAst* c1;
  StmtAst* c2;
public:
  While(ExpAst* guard, StmtAst* whilebody);
  void print();
};

class FloatConst : public ExpAst
{
private:
  AstType astnode_type;
  float val;
public:
  FloatConst(float _val);
  void print();
};

class BinaryOp : public ExpAst
{
private:
  AstType astnode_type;
  ExpAst* c1;
  ExpAst* c2;
  OpType op;
public:
  BinaryOp(ExpAst* left_exp , ExpAst* right_exp, OpType _op);
  void print();
};

class UnaryOp : public ExpAst
{
private:
  AstType astnode_type;
  ExpAst* c1;
  OpType op;
public:
  UnaryOp(ExpAst* exp, OpType _op);
  void print();
};

class FunCall : public ExpAst
{
private:
  AstType astnode_type;
  list<ExpAst*> list_exp_ast;
  string func_name;
public:
  FunCall(ExpAst* _exp_ast);
  void setName(string fname);
  void print();
  std::list<ValType> getArgTypeList();
  void insert(ExpAst* e);
};


class IntConst : public ExpAst
{
private:
  AstType astnode_type;
  int val;
public:
  IntConst(int _val);
  void print();
};

class StringConst : public ExpAst
{
private:
  AstType astnode_type;
  std::string val;
public:
  StringConst(std::string _val);
  void print();
};


class Identifier : public ArrayRef
{
private:
  AstType astnode_type;
  std::string val;
public:
  Identifier(std::string _val);
  void print();
};

class Index : public ArrayRef
{
private:
  AstType astnode_type;
  ArrayRef* c1;
  ExpAst* c2;
public:
  Index(ArrayRef* arrRef , ExpAst* expAst);
  void print();
};

#endif
