#include <stdio.h>
#include <map>
#include <vector>
enum TypeInfo {DECL_P_INT, DECL_P_FLOAT, DECL_P_VOID, DECL_P_CHAR, DECL_ARRAY, DECL_POINTER};
enum VarDeclType {GLOBAL, LOCAL, PARAM};

struct VarType
{
	bool primitive;
	TypeInfo type;
	int size;
	VarType* nestedVarType;
	std::string typeInfoString[6] = {"INT", "FLOAT", "VOID", "CHAR", "ARRAY", "POINTER"}; //TODO make this global

	void setPrimitive(TypeInfo _type) // TODO: set the size here
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
		if(primitive) return typeInfoString[type];
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
	std:: string declTypeString[3] = {"GLOBAL","LOCAL","PARAM"}; //TODO make this global

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
		printf("%-20s %-20s %-20s %-10d %-10d\n",name.c_str(),(varType->getString()).c_str(), declTypeString[declType].c_str(),size,offset);
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
	
	void setName( std::string _name)
	{
		name = _name;
	}

	bool existsSymbol(std::string _symbolName)
	{
		for( int i  = 0 ; i < paramOrderVector.size() ; i++)
		{	
			if( _symbolName == paramOrderVector[i]) return true;
		}
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
};

