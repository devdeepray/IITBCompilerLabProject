#ifndef _AST_NODES_
#define _AST_NODES_
#include <iostream>
enum typeExp {STMT, SEQ, EXP, ASS, RET, IF, FOR, WHILE, FLOAT};
std::string typeLookup[] = {"STMT", "SEQ", "EXP", "ASS", "RET", "IF", "FOR", "WHILE", "FLOAT"};

int tab_degree = 0;

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

class Seq : public StmtAst
{
	private:
		typeExp astnode_type;
		StmtAst* c1;
		StmtAst* c2;
	public:
		Seq(StmtAst* left_stmt, StmtAst* right_stmt)
		{
			astnode_type = SEQ;
			c1 = left_stmt;
			c2 = right_stmt;
		}
		void print()
		{
			tab_degree++;

			indent_print( typeLookup[astnode_type]+"\n" );
			indent_print( "C1: {\n" );
			c1->print();
			indent_print( "}\nC2: {\n" );
			c2->print();
			indent_print( "}\n" );
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
			indent_print( typeLookup[astnode_type] +"\n" ); 
			indent_print( "C1: {\n" );
			c1->print();
			indent_print( "}\nC2: {\n" );
			c2->print();
			indent_print( "}\n" );
		}
};

class Return : public StmtAst
{
	private:
		typeExp astnode_type;
		ExpAst* c1;
	public:
		Ass(ExpAst* ret_exp)
		{
			astnode_type = RET;
			c1 = ret_exp;
		}
		void print()
		{
			indent_print( typeLookup[astnode_type] + "
" );
			indent_print( "C1: {\n" );
			c1.print();
			indent_print( "\n}\n" );
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
		Ass(ExpAst* cond, StmtAst* if_stats, StmtAst* else_stats)
		{
			astnode_type = ASS;
			c1 = cond;
			c2 = if_stats;
			c3 = else_stats;
		}
		void print()
		{
			indent_print( typeLookup[astnode_type] + "
" );
			indent_print( "C1: {\n" );
			c1->print();
			indent_print( "}\nC2: {\n" );
			c2->print();
			indent_print( "}\nC3: {\n" );
			c3->print();
			indent_print( "}\n" );
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
			indent_print( typeLookup[astnode_type] + "
" );
			indent_print( "Value: " << val <<  std::endl;
		}
};
#endif
 
 
