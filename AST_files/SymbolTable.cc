#include "SymbolTable.h"
#include <map>
#include <vector>
#include "../Util/catter.h"
#include "Enums.h"



  void VarType::setPrimitive(ValType _type) // TODO: set the size here
  {
    primitive = true;	
    nestedVarType = NULL;
    type = _type;
  }
  
  void VarType::setArray(int _size)
  {
    primitive = false;
    size = _size;
    type = TYPE_ARRAY;
  }
  
  void VarType::setNestedVarType(VarType* _nestedVarType)
  {
    nestedVarType= _nestedVarType;
  }
  
  ValType VarType::getValType()
  {
    return type;
  }
  
  VarType* VarType::getNestedVarType()
  {
    return VarType::nestedVarType;
  }
  
  std::string VarType::getString()
  {
    if(primitive) return cat::symtabtype(type);
    else return "A(" + to_string(size) + "," + nestedVarType->getString()+")";
  }

  
  void VarDeclaration::setDeclType(VarDeclType _declType)
  {
    declType = _declType;
  }
  
  void VarDeclaration::setName( std::string _name)
  {
    name = _name;
  }
  
  void VarDeclaration::setSize( int _size)
  {
    size = _size;
  }
  
  void VarDeclaration::setOffset( int _offset)
  {
    offset = _offset;
  }
  
  void VarDeclaration::setVarType ( VarType* _varType)
  {
    varType = _varType;
  } 
  
  void VarDeclaration::print()
  {
    printf("%-20s %-20s %-20s %-10d %-10d\n",name.c_str(),(varType->getString()).c_str(), cat::symtabdecltype(declType).c_str(),size,offset);
  }

  FunctionSignature::FunctionSignature(std::string name, std::list<ValType> arg)
  {
    fname = name;
    argTypeList =  arg;
  }
  bool FunctionSignature::operator<(const FunctionSignature s2) const
  {
    if(fname != s2.fname) return fname < s2.fname;
    else
      return argTypeList < s2.argTypeList;
  }
  
  void FunctionSignature::print()
  {
    cout << fname << "(";
    for(auto it = argTypeList.begin(); it != argTypeList.end(); ++it)
    {
      cout << valTypeLookup[*it];
    }
    cout << ")";
  }


  VarDeclaration FunctionTable::getVar(string var_name)
  {
    return (strVarMap.find(var_name))->second;
  }
  
  void FunctionTable::addParam( VarDeclaration _varDecl)
  {
    strVarMap.insert(pair<string, VarDeclaration>(_varDecl.name ,_varDecl));
    paramOrderVector.push_back(_varDecl.name);
  }
  
  void FunctionTable::addVar( VarDeclaration _varDecl)
  {
    strVarMap.insert(pair<string, VarDeclaration>(_varDecl.name ,_varDecl));
  }
  void FunctionTable::correctOffsets()
  {
    if(strVarMap.size() != paramOrderVector.size()) // If some params are present
    {
      // Get the last param
      auto lastParam = strVarMap.begin();
      for(auto it = strVarMap.begin(); it != strVarMap.end(); ++it)
      {
	if(it->second.declType == PARAM)
	{
	  if(it -> second.offset > lastParam -> second.offset)
	  {
	    lastParam = it;
	  }
	}
      }
      int offsetCorrection = lastParam->second.offset+ lastParam->second.size;
      for(auto it = strVarMap.begin(); it != strVarMap.end(); ++it)
      {
	it->second.offset -= offsetCorrection;
      }
    }
  }
  void FunctionTable::setReturnType(ValType _type)
  {
    returnType = new VarType();
    returnType->setPrimitive(_type);
  }
  ValType FunctionTable::getReturnType()
  {
    return returnType->getValType();
  }
  string FunctionTable::getName()
  {
    return name;
  }
  void FunctionTable::setName( std::string _name)
  {
    name = _name;
  }
  
  list<ValType> FunctionTable::getArgTypeList()
  {
    list<ValType> result;
    for(auto it = paramOrderVector.begin();
	it != paramOrderVector.end();
	  ++it)
	{
	  result.push_back(strVarMap.find(*it)->second.varType->type);
	}
    return result;
  }
  
  FunctionSignature FunctionTable::getSignature()
  {
    FunctionSignature tmp(getName(), getArgTypeList());
    return tmp;
  }
  

  
  bool FunctionTable::existsSymbol(std::string _symbolName)
  {
    
    for(auto it = strVarMap.begin(); it != strVarMap.end(); ++it)
    {
      if(it->first == _symbolName)
	return true;
    }
    return false;
  } 
  
  void FunctionTable::reset()
  {
    name = "";
    returnType = NULL;
    paramOrderVector.clear();
    strVarMap.clear();	
  }
  
  void FunctionTable::print()
  {
    cout << "Symbol Table For " << name << endl;
    printf("%-20s %-20s %-20s %-10s %-10s\n","NAME","TYPE","PARAM/LOCAL","SIZE","OFFSET");
    std::map< std::string, VarDeclaration > :: iterator it = strVarMap.begin(); 
    for(; it != strVarMap.end() ; it++) (it->second).print();
    cout << endl;	
  }



  void SymTab::addFuncTable(FunctionTable _funcTable)
  {
    strFuncMap.insert(pair<FunctionSignature, FunctionTable>(_funcTable.getSignature() , _funcTable));
  }
  
  void SymTab::print()
  {
    cout << "Global Symbol Table" << endl;
    
    auto it = strFuncMap.begin();
    printf("%-20s %-20s\n","Function_Name","Return_Type");
    for( ; it != strFuncMap.end() ; it++)
    {			
      printf("%-20s %-20d\n",((it->second).name).c_str(),((it->second).returnType)->type);// create a vector for the enumerated type
    }
    cout << endl;
    
    //		it = strFuncMap.begin();
    //		for( ; it != strFuncMap.end() ; it++) (it->second).print();
  }
  FunctionTable SymTab::getFuncTable(FunctionSignature sig)
  {
    return strFuncMap.find(sig)->second;
  }
  bool SymTab::existsSignature(FunctionSignature sig)
  {
    return strFuncMap.find(sig) != strFuncMap.end();
  }
  
  