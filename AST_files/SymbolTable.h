#include <stdio.h>
#include <map>
#include <vector>
#include "../Util/catter.h"
enum VarDeclType {GLOBAL, LOCAL, PARAM};
enum ValType {TYPE_INT, TYPE_FLOAT, TYPE_VOID, TYPE_CHAR, TYPE_ARRAY, TYPE_POINTER};

struct VarType
{
	bool primitive;
	ValType type;
	int size; // Size is not initialized for primitives
	VarType* nestedVarType; // If it is an array

	void setPrimitive(ValType _type) // TODO: set the size here
	{
		primitive = true;	
		nestedVarType = NULL;
		type = _type;
	}

	void setArray(int _size)
	{
		primitive = false;
		size = _size;
		type = DECL_ARRAY;
	}

	void setNestedVarType(VarType* _nestedVarType)
	{
		nestedVarType= _nestedVarType;
	}

	VarType* getNestedVarType()
	{
		return nestedVarType;
	}
	
	std::string getString()
	{
		if(primitive) return cat::symtabtype(type);
		else return "A(" + to_string(size) + "," + nestedVarType->getString()+")";
	}
};

struct VarDeclaration
{
	VarDeclType declType;
	std::string name;
	VarType* varType;
	int size;
	int offset;
	 //TODO make this global

	void setDeclType(VarDeclType _declType)
	{
		declType = _declType;
	}

	void setName( std::string _name)
	{
		name = _name;
	}

	void setSize( int _size)
	{
		size = _size;
	}

	void setOffset( int _offset)
	{
		offset = _offset;
	}
	
	void setVarType ( VarType* _varType)
	{
		varType = _varType;
	} 

	void print()
	{
		printf("%-20s %-20s %-20s %-10d %-10d\n",name.c_str(),(varType->getString()).c_str(), cat::symtabdecltype(declType).c_str(),size,offset);
	}
};

struct FunctionTable
{
	std::string name;
	std::map< std::string, VarDeclaration > strVarMap;
	std::vector< std::string > paramOrderVector;
	VarType* returnType;

	void addParam( VarDeclaration _varDecl)
	{
		strVarMap[_varDecl.name] = _varDecl;
		paramOrderVector.push_back(_varDecl.name);
	}
	
	void addVar( VarDeclaration _varDecl)
	{
		strVarMap[_varDecl.name] = _varDecl;
	}
	void correctOffsets()
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
	void setReturnType(TypeInfo _type)
	{
		returnType = new VarType();
		returnType->setPrimitive(_type);	
	}
	string getName()
	{
		return name;
	}
	void setName( std::string _name)
	{
		name = _name;
	}

	bool existsSymbol(std::string _symbolName)
	{
		for( int i  = 0 ; i < paramOrderVector.size() ; i++)
		{	
			if( _symbolName == paramOrderVector[i]) return true;
		}EXP_VAL_INT
		return false;
	} 

	void reset()
	{
		name = "";
		returnType = NULL;
		paramOrderVector.clear();
		strVarMap.clear();	
	}

	void print()
	{
		cout << "Symbol Table For " << name << endl;
		printf("%-20s %-20s %-20s %-10s %-10s\n","NAME","TYPE","PARAM/LOCAL","SIZE","OFFSET");
		std::map< std::string, VarDeclaration > :: iterator it = strVarMap.begin(); 
		for(; it != strVarMap.end() ; it++) (it->second).print();
		cout << endl;	
	}
};

struct SymTab
{
	std::map< std::string, FunctionTable > strFuncMap;
	
	void addFuncTable(FunctionTable _funcTable)
	{
		strFuncMap[_funcTable.name] = _funcTable;
	}

	void print()
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
	FunctionTable getFuncTable(std::string fname)
	{
	   return strFuncMap[fname];
	}
	bool existsSymbol(string id)
	{
		return strFuncMap.find(id) != strFuncMap.end();
	}
};

