#ifndef _SYM_TAB_H_
#define _SYM_TAB_H_
#include <stdio.h>
#include <map>
#include <vector>
#include "Enums.h"
#include <list>
using namespace std;


struct DataType
{
  // Data members
  
  ValType primitive_type;
  list<int> array_dims;
  
  // Member functions
  DataType(ValType);
  DataType();
  void setPrimitive(ValType _primitive_type);
  ValType getPrimitiveType();
  bool isPrimitive() const;
  void addDimension(int dim);
  void removeDimension();
  void clearDimensions();
  void reset();
  std::string getString();
  
  bool operator<(  const DataType& dt2) const  ;
  bool operator==(  const DataType& dt2)  const ;
  bool operator!=(  const DataType& dt2) const ;
};

struct VarDeclaration
{
  // Data members
  VarDeclType decl_type; 
  std::string name;
  DataType data_type;
  int size;
  int offset;
  
  // Member functions
  void setDeclType(VarDeclType _decl_type);
  void setName( std::string _name);
  void setSize( int _size);
  void setOffset( int _offset);
  void setDataType ( DataType _data_type);
  void print();
};
struct FunctionSignature
{
  std::string fname;
  std::list<DataType> arg_types;
  
  FunctionSignature(std::string _fname, std::list<DataType> _arg_types);
  void print();
  bool operator<(const FunctionSignature& sig2)const;
};

struct FunctionTable
{
  
  std::string fname;
  std::map< std::string, VarDeclaration > var_name_map;
  std::map< int, std::string > var_offset_map;
  DataType return_type;
  
  VarDeclaration getVar(std::string var_name);
  bool existsSymbol(std::string symbol);
  void addLocal( VarDeclaration var_decl);
  void addParam( VarDeclaration var_decl);
  std::list<DataType> getArgTypeList();
  FunctionSignature getSignature();
  void setReturnType(DataType _return_type);
  DataType getReturnType();
  void setName( std::string _fname);
  std::string getName();
  void reset();
  void print();
};


struct SymTab
{
  std::map< FunctionSignature, FunctionTable > func_name_map;
  
  void addFuncTable(FunctionTable functable);
  void updateFuncTable(FunctionTable functable);
  FunctionTable getFuncTable(FunctionSignature sig);
  bool existsSignature(FunctionSignature sig);
  FunctionSignature getCompatibleSignature(FunctionSignature sig, int *match_count);
  
  void print();
  
};
#endif
