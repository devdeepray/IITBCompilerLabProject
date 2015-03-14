
#ifndef _CATTER_H_
#define _CATTER_H_

#include "../AST_files/SymbolTable.h"
#include "../AST_files/ASTNodes.h"

#define PRINT_TOKEN false

using namespace std;
class cat
{
    
    public:
        static void token(std::string message)
        {
            if(PRINT_TOKEN)
            {
                std::cout << "[TOKEN]: " << message << std::endl;
            }
        }
        
        static std::string symtabtype(int index)
        {
            std::string symTabTypeInfo[] = {"INT", "FLOAT", "VOID", "CHAR", "ARRAY", "POINTER"};
            return symTabTypeInfo[index];
        }
        
        static std::string symtabdecltype(int index)
        {
            std::string symTabDeclTypeInfo[] = {"GLOBAL","LOCAL","PARAM"};
            return symTabDeclTypeInfo[index];
        }
public:
        class parse
        {
	public:
	  class fdecerror
	  {
	  public:
	   fdecerror(int lno, FunctionSignature fsig)
	    {
	      cout << "[Function Declaration error][Line " << lno << "]:" ;
	      fsig.print();
	      cout << endl;
	    }
	    static void funcdupsig(int lno, FunctionSignature fsig)
	    {
	      cout << "[Duplicate definition of function][Line " << lno << "]:" ;
	      fsig.print();
	      cout << endl;
	    }
	    static void badfcall(int lno, FunctionSignature fsig)
	    {
	      cout << "[Function not defined][Line " << lno << "]:" 
		<< "No function with signature: " << endl;
	      fsig.print();
	      cout << endl;
	    }
	    
	   
	  };
	  
	  class declaratorerror
	  {
	    public:
	    static void dupid(int lno, string id)
	    {
	      cout << "[Duplicate Identifier used][Line " << lno << "]:" << id << endl;
	    }
	    static void emptyarray(int lno, string id)
	    {
	      cout << "[Empty array][Line " << lno << "]:" << "Tried to declare " << id << " with some size as empty" << endl;
	    }
	    
	  };
	  
	  class stmterror
	  {
	  public:
	    static void rettypeerror(int lno, ValType expret, ValType actret)
	    {
	      cout << "[Return Type mismatch][Line " << lno << "]:Value of type " << valTypeLookup[actret] << " cannot be\
	      returned from a function returning " << valTypeLookup[expret] << endl;
	    }
	    
	    static void incompasstype(int lno, ValType lval, ValType rval)
	    {
	      cout << "[Assignment type mismatch][Line " << lno << "]:" << valTypeLookup[rval] 
					  << " cannot be assigned to " << valTypeLookup[lval] << endl;
	    }
	    
	    static void incompboptype(int lno, ValType val1, ValType val2, OpType op)
	    {
	      cout << "[Operator type mismatch][Line " << lno << "]:" << valTypeLookup[val1] << " and " << 
		      valTypeLookup[val2] << " are incompatible for " << opTypeLookup[op] << endl;
	    }
	    
	    static void invalidunop(int lno, OpType op, ValType val)
	    {
	      cout << "[Unary operator mismatch][Line " << lno << "]:" << valTypeLookup[val] << " is not compatible with operator "
		    << opTypeLookup[op] <<endl;
	    }
			
			static void symbolnotfound(int lno, string name)
			{
			  cout << "[Identifier not defined][Line " << lno << "]:"
			  << name << " not defined in scope" << endl;
			}
			
			static void arrayreferror(int lno, string arrayname)
			{
			  cout << "[Array reference error][Line " << lno << "]:"
			  << arrayname << "Wrong array reference. Check the dimensions" << endl;
			}
	      
	  };
	  
	};
	    
        
};

#endif