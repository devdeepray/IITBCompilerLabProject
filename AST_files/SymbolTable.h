#include <map>
#include <vector>
enum TypeInfo {DECL_P_INT, DECL_P_FLOAT, DECL_P_VOID, DECL_P_CHAR, DECL_ARRAY, DECL_POINTER};
enum VarDeclType {GLOBAL, LOCAL, PARAM};

struct FunctionTable;
struct VarDeclaration;
struct VarType;


struct SymTab
{
	std::map< std::string, FunctionTable > strFuncMap;
};

struct FunctionTable
{
	std::map< std::string, VarDeclaration > strVarMap;
	std::vector< std::string > paramOrdVector;
	VarType* returnType;
};

struct VarDeclaration
{
	VarDeclType declType;
	std::string name;
	VarType* varType;
	int size;
	int offset;
};

struct VarType
{
	bool primitive;
	TypeInfo type;
	int size;
	VarType* nestedVarType;
};

