#ifndef _AST_NODES_
#define _AST_NODES_
#include <iostream>
enum typeExp {STMT, SEQ, EXP, ASS, RET, IF, FOR, WHILE, FLOAT};
std::string typeLookup[] = {"STMT", "SEQ", "EXP", "ASS", "RET", "IF", "FOR", "WHILE", "FLOAT"};



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
			std::cout << typeLookup[astnode_type] << std::endl;
			std::cout << "C1: {\n";
			c1->print();
			std::cout << "}\nC2: {\n";
			c2->print();
			std::cout << "}\n";
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
			std::cout << typeLookup[astnode_type] << std::endl;
			std::cout << "Left_child: {" << std::endl;
			c1->print();
			std::cout << "}\nRight_child: {" << std::endl;
			c2->print();
			std::cout << "}" << std::endl;
		}
};

class Return : public StmtAst
{
	private:
		typeExp astnode_type;
		ExpAst* ret_exp;
	public:
		Ass(ExpAst* _ret_exp)
		{
			astnode_type = RET;
			exp_ast = _exp_ast;
		}
		void print()
		{
			std::cout << typeLookup[astnode_type] << std::endl;
			std::cout << "Child: {\n";
			ret_exp.print();
			std::cout << "
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
			std::cout << typeLookup[astnode_type] << std::endl;
			std::cout << "Value: " << val <<  std::endl;
		}
};
#endif
 
 
