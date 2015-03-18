#ifndef _SYM_TAB_H_
#define _SYM_TAB_H_
#include <stdio.h>
#include <map>
#include <vector>
#include "Enums.h"
#include <list>


struct VarType
{
  bool primitive;
  ValType type;
  int size; // Size is not initialized for primitives
  VarType* nestedVarType; // If it is an array
  
  void setPrimitive(ValType _type);
  
  void setArray(int _size);
  
  void setNestedVarType(VarType* _nestedVarType);
  
  ValType getValType();
  
  VarType* getNestedVarType();
  
  std::string getString();
};

struct VarDeclaration
{
  VarDeclType declType;
  std::string name;
  VarType* varType;
  int size;
  int offset;
  //TODO make this global
  
  void setDeclType(VarDeclType _declType);
  
  void setName( std::string _name);
  
  void setSize( int _size);
  
  void setOffset( int _offset);
  
  void setVarType ( VarType* _varType);
  
  void print();
};
struct FunctionSignature
{
  std::string fname;
  std::list<ValType> argTypeList;
  FunctionSignature(std::string name, std::list<ValType> arg);
  bool operator<(const FunctionSignature s2) const;
  void print();
};

struct FunctionTable
{
  
  std::string name;
  // Multi map needed for storing func signature even if rep. id used for params
  std::multimap< std::string, VarDeclaration > strVarMap;
  std::vector< std::string > paramOrderVector;
  VarType* returnType;
  
  VarDeclaration getVar(std::string var_name);
  
  void addParam( VarDeclaration _varDecl);
  
  void addVar( VarDeclaration _varDecl);
  void correctOffsets();
  void setReturnType(ValType _type);
  ValType getReturnType();
  std::string getName();
  void setName( std::string _name);
  
  std::list<ValType> getArgTypeList();
  
  FunctionSignature getSignature();
  
  bool existsSymbol(std::string _symbolName);
  void reset();
  
  void print();
};


struct SymTab
{
  std::map< FunctionSignature, FunctionTable > strFuncMap;
  
  void addFuncTable(FunctionTable _funcTable);
  void updateFuncTable(FunctionTable _funcTable);
  
  void print();
  FunctionTable getFuncTable(FunctionSignature sig);
  bool existsSignature(FunctionSignature sig);
  
};
#endif
