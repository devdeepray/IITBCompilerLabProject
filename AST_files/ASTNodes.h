#ifndef _AST_NODES_
#define _AST_NODES_
#include <iostream>
#include <list>
#include <vector>
using namespace std;
enum typeExp {STMT, BLK, EXP, EMP_BLK, ASS, RET, IF, FOR, WHILE, FLOAT, BINARY_OP, UNARY_OP, FUNCALL, INT, STR, IDENTIFIER, INDEX};
std::string typeLookup[] = {"STMT", "BLK", "EXP", "EMP_BLK", "ASS", "RET", "IF", "FOR", "WHILE", "FLOAT", "BINARY_OP", "UNARY_OP", "FUNCALL",
	"INT", "STR", "IDENTIFIER", "INDEX"};

enum typeOp {OR, AND, EQ_OP, NE_OP, LT, GT, LE_OP, GE_OP, PLUS, MINUS, MULT, DIV, ASSIGN, UMINUS, NOT, PP};
std::string opLookup[]  = {"OR", "AND", "EQ_OP", "NE_OP", "LT","GT", "LE_OP", "GE_OP", "PLUS", "MINUS", "MULT", "DIV", "ASSIGN", "UMINUS", "NOT", "PP"};

int tab_degree = 0;


void indent_print(std::string s)
{
	for(int i = 0; i < tab_degree; ++i)
		std::cout << "  ";
	std::cout << s;
}

class abstract_astnode
{
	public:
		virtual void print () = 0;
		//virtual std::string generate_code(const symbolTable&) = 0;
		//virtual basic_types getType() = 0;
		//virtual bool checkTypeofAST() = 0;
	protected:
		//virtual void setType(basic_types) = 0;
	private:
		typeExp astnode_type;
};


class ExpAst : public abstract_astnode
{
	private:
		typeExp astnode_type;
};


class StmtAst : public abstract_astnode
{
	private:
		typeExp astnode_type;
};

class ArrayRef : public ExpAst
{
	private:
		typeExp astnode_type;
};

class Empty : public StmtAst
{
	private:
		typeExp astnode_type;
	public:
		Empty()
		{
			astnode_type = EMP_BLK;
		}
		void print()
		{
			tab_degree++;
			indent_print( typeLookup[astnode_type]+"\n" );
			tab_degree--;
		}
};


class Block : public StmtAst
{
	private:
		typeExp astnode_type;
		std::list<StmtAst*> clist;
	public:
		Block(StmtAst* _c)
		{
			astnode_type = BLK;
			clist.push_back(c);
		}
		void print()
		{
			tab_degree++;
			indent_print( typeLookup[astnode_type]+"\n" );
			for(auto it = clist.begin(); it != clist.end(); ++it)
			{
				indent_print( "C {\n" );
				(*it)->print();
				indent_print( "}\n" );
			}

			tab_degree--;
		}
		void insert(StmtAst* c)
		{
			clist.push_back(c);
		}
};

class Ass : public StmtAst
{
	private:
		typeExp astnode_type;
		ExpAst* c1;
		ExpAst* c2;
	public:
		Ass(ExpAst* left_stmt, ExpAst* right_stmt)
		{
			astnode_type = ASS;
			c1 = left_stmt;
			c2 = right_stmt;
		}
		void print()
		{
			tab_degree++;

			indent_print( typeLookup[astnode_type] +"\n" ); 
			indent_print( "C1: {\n" );
			c1->print();
			indent_print( "}\nC2: {\n" );
			c2->print();
			indent_print( "}\n" );
			tab_degree--;
		}
};

class Return : public StmtAst
{
	private:
		typeExp astnode_type;
		ExpAst* c1;
	public:
		Return(ExpAst* ret_exp)
		{
			astnode_type = RET;
			c1 = ret_exp;
		}
		void print()
		{
			tab_degree++;

			indent_print( typeLookup[astnode_type] + "\n" );
			indent_print( "C1: {\n" );
			c1->print();
			indent_print( "\n}\n" );
			tab_degree--;
		}
};


class If : public StmtAst
{
	private:
		typeExp astnode_type;
		ExpAst* c1;
		StmtAst* c2;
		StmtAst* c3;
	public:
		If(ExpAst* cond, StmtAst* if_stats, StmtAst* else_stats)
		{
			astnode_type = IF;
			c1 = cond;
			c2 = if_stats;
			c3 = else_stats;
		}
		void print()
		{
			tab_degree++;

			indent_print( typeLookup[astnode_type] + "\n" );
			indent_print( "C1: {\n" );
			c1->print();
			indent_print( "}\nC2: {\n" );
			c2->print();
			indent_print( "}\nC3: {\n" );
			c3->print();
			indent_print( "}\n" );
			tab_degree--;
		}
};


class For : public StmtAst
{
	private:
		typeExp astnode_type;
		ExpAst* c1;
		ExpAst* c2;
		ExpAst* c3;
		StmtAst* c4;
	public:
		For(ExpAst* initialize, ExpAst* guard, ExpAst* update, StmtAst* forbody)
		{
			astnode_type = FOR;
			c1 = initialize;
			c2 = guard;
			c3 = update;
			c4 = forbody;
		}
		void print()
		{
			tab_degree++;


			indent_print( typeLookup[astnode_type] + "\n" );
			indent_print( "C1: {\n" );
			c1->print();
			indent_print( "}\nC2: {\n" );
			c2->print();
			indent_print( "}\nC3: {\n" );
			c3->print();
			indent_print( "}\nC4: {\n" );
			c4->print();
			indent_print( "}\n" );
			tab_degree--;
		}
};

class While : public StmtAst
{
	private:
		typeExp astnode_type;
		ExpAst* c1;
		StmtAst* c2;
	public:
		While(ExpAst* guard, StmtAst* whilebody)
		{
			astnode_type = WHILE;
			c1 = guard;
			c2 = whilebody;
		}
		void print()
		{
			tab_degree++;


			indent_print( typeLookup[astnode_type] + "\n" );
			indent_print( "C1: {\n" );
			c1->print();
			indent_print( "}\nC2: {\n" );
			c2->print();
			indent_print( "}\n" );
			tab_degree--;
		}
};

class FloatConst : public ExpAst
{
	private:
		typeExp astnode_type;
		float val;
	public:
		FloatConst(float _val)
		{
			astnode_type = FLOAT;
			val = _val;
		}
		void print()
		{
			tab_degree++;

			indent_print( typeLookup[astnode_type] + "\n" );
			indent_print( "Value: " + std::to_string(val) + "\n" ); 
			tab_degree--;
		}
};

class BinaryOp : public ExpAst
{
	private:
		typeExp astnode_type;
		ExpAst* c1;
		ExpAst* c2;
		typeOp op;
	public:
		BinaryOp(ExpAst* left_exp , ExpAst* right_exp, typeExp _op)
		{
			astnode_type = BINARY_OP;
			op= _op;
			c1=left_exp;
			c2=right_exp;
		}
		void print()
		{
			tab_degree++;
			indent_print(typeLookup[astnode_type]+"\n");
			indent_print("C1: {\n");
			c1->print();
			indent_print("}\n");
			indent_print("C2: {\n");
			c2->print();
			indent_print("}\n");
			tab_degree--;
		}
};

class unaryOp : public ExpAst
{
	private:
		typeExp astnode_type;
		ExpAst* c1;
		typeOp op;
	public:
		unaryOp(ExpAst* exp, typeOp _op)
		{
			astnode_type = UNARY_OP;
			op = _op;
			c1=exp;
		}
		void print()
		{
			tab_degree++;
			indent_print(typeLookup[astnode_type]+"\n");
			indent_print("C1: {\n");
			c1->print();
			indent_print("}\n");
			tab_degree--;
		}
};

class FunCall : public ExpAst
{
	private:
		typeExp astnode_type;
		list<ExpAst*> list_exp_ast;
		string func_name;
	public:
		FunCall(ExpAst* _exp_ast, string _func_name)
		{
			astnode_type = FUNCALL;
			if( _exp_ast) list_exp_ast.push_back(_exp_ast);
			func_name = _func_name;
		}
		void print()
		{
			tab_degree++;
			indent_print(typeLookup[astnode_type]+"\n");
			for( auto i = list_exp_ast.begin() ; i != list_exp_ast.end() ; i++)
			{
				indent_print("C: {\n");
				(*i)->print();
				indent_print("}\n");
			}
			tab_degree--;
		}
		
		void insert(ExpAst* e)
		{
			list_exp_ast.push_back(e);
		}
};


class IntConst : public ExpAst
{
	private:
		typeExp astnode_type;
		int val;
	public:
		IntConst(int _val)
		{
			astnode_type = INT;
			val = _val;
		}
		void print()
		{
			tab_degree++;
			indent_print(typeLookup[astnode_type]+"\n");
			indent_print("Value: "+std::to_string(val)+"\n");
			tab_degree--;
		}
};

class StringConst : public ExpAst
{
	private:
		typeExp astnode_type;
		std::string val;
	public:
		StringConst(int _val)
		{
			astnode_type = STR;
			val = _val;
		}
		void print()
		{
			tab_degree++;
			indent_print(typeLookup[astnode_type]+"\n");
			indent_print("Value: "+val+"\n");
			tab_degree--;
		}
};


class Identifier : public ArrayRef
{
	private:
		typeExp astnode_type;
		std::string val;
	public:
		Identifier(std::string _val)
		{
			astnode_type = IDENTIFIER;
			val = _val;
		}
		void print()
		{
			tab_degree++;
			indent_print(typeLookup[astnode_type]+"\n");
			indent_print("Value: "+val+"\n");
			tab_degree--;
		}
};

class Index : public ArrayRef
{
	private:
		typeExp astnode_type;
		ArrayRef* c1;
		ExpAst* c2;
	public:
		Index(ArrayRef* arrRef , ExpAst* expAst)
		{
			astnode_type = INDEX;
			c1 = arrRef;
			c2 = expAst;
		}
		void print()
		{
			tab_degree++;
			indent_print(typeLookup[astnode_type]+"\n");
			indent_print("C1: {\n");
			c1->print();
			indent_print("}\n");
			indent_print("C2: {\n");
			c2->print();
			indent_print("}\n");
			tab_degree--;
		}
};
 
#endif
