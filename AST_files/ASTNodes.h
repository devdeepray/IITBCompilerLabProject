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
		StmtAst* left_stmt;
		StmtAst* right_stmt;
	public:
		Seq(StmtAst* _left_stmt, StmtAst* _right_stmt)
		{
			astnode_type = SEQ;
			left_stmt = _left_stmt;
			right_stmt = _right_stmt;
		}
		void print()
		{
			std::cout << typeLookup[astnode_type] << std::endl;
			std::cout << "Left child:" << std::endl;
			left_stmt->print();
			std::cout << "Right child:" << std::endl;
			right_stmt->print();
		}
};

class Ass : public StmtAst
{
	private:
		typeExp astnode_type;
		ExpAst* left_stmt;
		ExpAst* right_stmt;
	public:
		Ass(ExpAst* _left_stmt, ExpAst* _right_stmt)
		{
			astnode_type = ASS;
			left_stmt = _left_stmt;
			right_stmt = _right_stmt;
		}
		void print()
		{
			std::cout << typeLookup[astnode_type] << std::endl;
			std::cout << "Left child: {" << std::endl;
			left_stmt->print();
			std::cout << "}\nRight child: {" << std::endl;
			right_stmt->print();
			std::cout << "}" << std::endl;
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
 
 
