
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
            return valTypeLookup[index];
        }
        
        static std::string symtabdecltype(int index)
        {
            return varDeclTypeLookup[index];
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
        static void voidtype(int lno, string id)
        {
            cout << "[Void type for variable][Line " << lno << "]:"
                    << id << " defined as void." << endl;
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
	      cout << "[Return Type mismatch][Line " << lno << "]:Value of type " << valTypeLookup[actret] << " cannot be" <<
	      " returned from a function returning " << valTypeLookup[expret] << endl;
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
          cout << "[Array reference error][Line " << lno << "]: "
          << arrayname << ": Wrong array reference. Check the dimensions" << endl;
        }

        static void ifexprerror(int lno)
        {
          cout << "[If expression error][Line "<<lno<<"]: If expression is not of correct type"<<endl;

        }
      static void forexprerror(int lno)
      {
        cout << "For expression error][Line "<<lno<<"]: For expression is not of correct type"<<endl;

      }
                  static void whileexprerror(int lno)
                  {
                    cout << "[While expression error][Line "<<lno<<"]: While expression is not of correct type"<<endl;

                  }

	      
	  };
	  
	};
	    
        
};

#endif
