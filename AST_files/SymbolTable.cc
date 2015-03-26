#include "SymbolTable.h"
#include <map>
#include <vector>
#include "../Util/catter.h"
#include "Enums.h"



  void DataType::setPrimitive(ValType _primitive_type) // TODO: set the size here
  {
    primitive_type = _primitive_type;
  }

  void DataType::addDimension(int dim)
  {
      array_dims.push_back(dim);
  }

  void DataType::reset()
  {
    array_dims.clear();
  }
  
  void DataType::removeDimension()
  {
    array_dims.pop_front();
  }
  
  void DataType::clearDimensions()
  {
    array_dims.clear();
  }
  bool DataType::isPrimitive() const
  {
    return array_dims.size() == 0;
  }
  ValType DataType::getPrimitiveType()
  {
    return primitive_type;
  }
  DataType::DataType(ValType vt)
  {
    setPrimitive(vt);
  }
  DataType::DataType()
  {
  }
  std::string DataType::getString()
  {
    
    if(isPrimitive())
    {
      return cat::symtabtype(primitive_type);
      
    }
    else 
    {
      DataType tmp = (*this);
      int size = *(tmp.array_dims.begin());
      tmp.removeDimension();
      
      return "A(" + to_string(size) + "," + tmp.getString()+")";
    }
  }

  bool DataType::operator<(const DataType &dt2) const
  {
    if(isPrimitive()!= dt2.isPrimitive())
    {
      return dt2.isPrimitive();
    }
    else
    {
	if(isPrimitive())
	{
		return primitive_type < dt2.primitive_type;
	}
	else
	{
		return false;
	}
    }
  }
  
  bool DataType::operator==( const DataType &dt2)  const
  {
    return !((*this)<(dt2)) && !(dt2 < (*this));
  }
  
  bool DataType::operator!=(const  DataType &dt2)  const
  {
    return !( (*this) == dt2);
  }
  
  void VarDeclaration::setDeclType(VarDeclType _decl_type)
  {
    decl_type = _decl_type;
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
  
  void VarDeclaration::setDataType (DataType _data_type)
  {
    data_type = _data_type;
  } 
  
  void VarDeclaration::print()
  {
    printf("%-20s %-20s %-20s %-10d %-10d\n",name.c_str(),(data_type.getString()).c_str(), cat::symtabdecltype(decl_type).c_str(),size,offset);
  }

  FunctionSignature::FunctionSignature(std::string _fname, std::list<DataType> _arg_types)
  {
    fname = _fname;
    arg_types =  _arg_types;
  }
  
  FunctionSignature::FunctionSignature()
  {
	fname="Default";
  }
  
  void FunctionSignature::print()
  {
    cout << fname << "( ";
    for(auto it = arg_types.begin(); it != arg_types.end(); ++it)
    {
        cout << it->getString() << ", ";
    }
    cout << ")";
  }

  bool FunctionSignature::operator<(const FunctionSignature& sig2) const
  {
    if(fname != sig2.fname)
    {
      return fname < sig2.fname;
    }
    else
    {
      return arg_types < sig2.arg_types;
    }
    
  }
  
  bool FunctionSignature::operator==(const FunctionSignature& sig2) const
  {
	return !((*this) < sig2) && !(sig2 < (*this));
  }
  
  VarDeclaration FunctionTable::getVar(string var_name)
  {
    return (var_name_map.find(var_name))->second;
  }
  
  void FunctionTable::addParam(VarDeclaration var_decl)
  {
    var_name_map.insert(pair<string, VarDeclaration>(var_decl.name ,var_decl));
    var_offset_map[var_decl.offset] = var_decl.name;
  }
  
  void FunctionTable::addLocal(VarDeclaration var_decl)
  {
    var_name_map.insert(pair<string, VarDeclaration>(var_decl.name ,var_decl));
    var_offset_map[var_decl.offset] = var_decl.name;
  }

  void FunctionTable::setReturnType(DataType type)
  {
    return_type = type; 
  }
  DataType FunctionTable::getReturnType()
  {
    return return_type;
  }
  string FunctionTable::getName()
  {
    return fname;
  }
  void FunctionTable::setName(std::string _fname)
  {
    fname = _fname;
  }
  
  list<DataType> FunctionTable::getArgTypeList()
  {
    list<DataType> result;
    for(auto it = var_offset_map.begin();
	it != var_offset_map.end();
	  ++it)
	{
	  if(it->first >= 0)
	  {
	    result.push_back(var_name_map.find(it->second)->second.data_type);
	  }
	}
    return result;
  }
  
  FunctionSignature FunctionTable::getSignature()
  {
    FunctionSignature tmp(getName(), getArgTypeList());
    return tmp;
  }
  

  
  bool FunctionTable::existsSymbol(std::string symbolName)
  {
    
    for(auto it = var_name_map.begin(); it != var_name_map.end(); ++it)
    {
      if(it->first == symbolName)
	return true;
    }
    return false;
  } 
  
  void FunctionTable::reset()
  {
    fname = "";
    return_type.reset();
    var_offset_map.clear();
    var_name_map.clear();
  }
  
  void FunctionTable::print()
  {
    cout << "Symbol Table For " << fname << endl;
    printf("%-20s %-20s %-20s %-10s %-10s\n","NAME","TYPE","PARAM/LOCAL","SIZE","OFFSET");
    std::map< std::string, VarDeclaration > :: iterator it = var_name_map.begin();
    for(; it != var_name_map.end() ; it++) (it->second).print();
    cout << endl;	
  }



  void SymTab::addFuncTable(FunctionTable _funcTable)
  {
    func_name_map.insert(pair<FunctionSignature, FunctionTable>(_funcTable.getSignature() , _funcTable));
  }
  void SymTab::updateFuncTable(FunctionTable _funcTable)
  {
    
    
    func_name_map[_funcTable.getSignature()] = _funcTable;
  }
  
  void SymTab::print()
  {
    cout << "Global Symbol Table" << endl;
    auto it = func_name_map.begin();
    printf("%-20s %-20s\n","Function_Name","Return_Type");
    for( ; it != func_name_map.end() ; it++)
    {			
      printf("%-20s %-20s\n",((it->second).fname).c_str(),it->second.return_type.getString().c_str());// create a vector for the enumerated type
    }
    cout << endl;
    
    //		it = func_name_map.begin();
    //		for( ; it != func_name_map.end() ; it++) (it->second).print();
  }
  FunctionTable SymTab::getFuncTable(FunctionSignature sig)
  {
    return func_name_map.find(sig)->second;
  }
  bool SymTab::existsSignature(FunctionSignature sig)
  {
    return func_name_map.find(sig) != func_name_map.end();
  }
  
  bool weakSignatureMatch(FunctionSignature sig1, FunctionSignature sig2)
  {
    // Check if sig2 can be put into sig1
    
    if( sig1.fname != sig2.fname) return false;
    if(sig1.arg_types.size() != sig2.arg_types.size())
    {
      return false;
    }
    else
    {
      auto it1 = sig1.arg_types.begin();
      auto it2 = sig2.arg_types.begin();
      bool iscomp = true;
      for(; it1 != sig1.arg_types.end(); ++it1, ++it2)
      {
	     iscomp = iscomp && castTypeCompatible(*it1, *it2);
      }
      return iscomp;
    }
  }
  
  FunctionSignature SymTab::getCompatibleSignature(FunctionSignature sig, int *match_count)
  {
    auto exact_match = func_name_map.end();
    auto weak_match = func_name_map.end();
    int weak_match_count = 0;
    

    for(auto itr = func_name_map.begin();
	itr != func_name_map.end(); ++itr)
	{
	  if(itr->first == sig)
	  {
	    exact_match = itr;
	  }
	  else if(weakSignatureMatch(itr->first, sig))
	  {
	    weak_match = itr;
	    weak_match_count++;
	  }
	}
	
	if(exact_match != func_name_map.end())
	{
	  *match_count = 1;
	//exact_match->first.print();
	  return exact_match->first;
	}
	else
	{
	  if(weak_match_count == 0)
	  {
	    *match_count = 0;
	    return FunctionSignature();
	  }
	  else
	  {
	    *match_count = weak_match_count;
	//weak_match->first.print();
	    return weak_match->first;
	  }
	}
	
	
	
  }
  
  
  
