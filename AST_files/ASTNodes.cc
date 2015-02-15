
#include <iostream>
#include <list>
#include <vector>
#include "ASTNodes.h"
using namespace std;
//enum typeExp {STMT, BLK, EXP, EMP_BLK, ASS, RET, IF, FOR, WHILE, FLOAT, BINARY_OP, UNARY_OP, FUNCALL, INT, STR, IDENTIFIER, INDEX};
std::string typeLookup[] = {"STMT", "BLK", "EXP", "EMP_BLK", "ASS", "RET", "IF", "FOR", "WHILE", "FLOAT", "BINARY_OP", "UNARY_OP", "FUNCALL",
	"INT", "STR", "IDENTIFIER", "INDEX"};

//enum typeOp {OR, AND, EQ_OP, NE_OP, LT, GT, LE_OP, GE_OP, PLUS, MINUS, MULT, DIV, ASSIGN, UMINUS, NOT, PP};
std::string opLookup[]  = {"OR", "AND", "EQ_OP", "NE_OP", "LT","GT", "LE_OP", "GE_OP", "PLUS", "MINUS", "MULT", "DIV", "ASSIGN", "UMINUS", "NOT", "PP"};

int tab_degree = 0;


void indent_print(std::string s)
{
	for(int i = 1; i < tab_degree; ++i)
		std::cout << "  ";
	std::cout << s;
}





		Empty::Empty()
		{
			astnode_type = EMP_BLK;
		}
		void Empty::print()
		{
			tab_degree++;
			indent_print( "(Empty)" );
			tab_degree--;
		}



		Block::Block(StmtAst* _c)
		{
			astnode_type = BLK;
			clist.push_back(_c);
		}
		void Block::print()
		{
			tab_degree++;
			indent_print( "(Block [" );
			for(auto it = clist.begin(); it != clist.end(); ++it)
			{
				cout << "\n" ;
				(*it)->print();
				
			}
			cout << "])" ;

			tab_degree--;
		}
		void Block::insert(StmtAst* c)
		{
			clist.push_back(c);
		}



		Ass::Ass(ExpAst* left_stmt, ExpAst* right_stmt)
		{
			astnode_type = ASS;
			c1 = left_stmt;
			c2 = right_stmt;
		}
		void Ass::print()
		{
			tab_degree++;

			indent_print( "(Assign_exp \n" ); 
			c1->print();
			indent_print( "\n" );
			c2->print();
			cout << ")" ;
			tab_degree--;
		}



		Return::Return(ExpAst* ret_exp)
		{
			astnode_type = RET;
			c1 = ret_exp;
		}
		void Return::print()
		{
			tab_degree++;

			indent_print( "(Return\n" );
			c1->print();
			cout << ")" ;
			tab_degree--;
		}




		If::If(ExpAst* cond, StmtAst* if_stats, StmtAst* else_stats)
		{
			astnode_type = IF;
			c1 = cond;
			c2 = if_stats;
			c3 = else_stats;
		}
		void If::print()
		{
			tab_degree++;

			indent_print( "(If\n" );
			c1->print();
			indent_print( "\n" );
			c2->print();
			indent_print( "\n" );
			c3->print();
			cout << ")";
			tab_degree--;
		}




		For::For(ExpAst* initialize, ExpAst* guard, ExpAst* update, StmtAst* forbody)
		{
			astnode_type = FOR;
			c1 = initialize;
			c2 = guard;
			c3 = update;
			c4 = forbody;
		}
		void For::print()
		{
			tab_degree++;


			indent_print( "(For\n" );
			c1->print();
			indent_print( "\n" );
			c2->print();
			indent_print( "\n" );
			c3->print();
			indent_print( "\n" );
			c4->print();
			cout << ")";
			tab_degree--;
		}



		While::While(ExpAst* guard, StmtAst* whilebody)
		{
			astnode_type = WHILE;
			c1 = guard;
			c2 = whilebody;
		}
		void While::print()
		{
			tab_degree++;
			indent_print( "(While\n" );
			c1->print();
			indent_print( "\n" );
			c2->print();
			cout << ")" ;
			tab_degree--;
		}


		FloatConst::FloatConst(float _val)
		{
			astnode_type = FLOAT;
			val = _val;
		}
		void FloatConst::print()
		{
			tab_degree++;

			indent_print( "(FloatConst " );
			cout << val << ")" ;
			tab_degree--;
		}


		BinaryOp::BinaryOp(ExpAst* left_exp , ExpAst* right_exp, typeOp _op)
		{
			astnode_type = BINARY_OP;
			op= _op;
			c1=left_exp;
			c2=right_exp;
		}
		void BinaryOp::print()
		{
			tab_degree++;
			indent_print("(");
			cout << opLookup[op] << "\n";
			c1->print();
			indent_print("\n");
			c2->print();
			cout << ")" ;
			tab_degree--;
		}


		UnaryOp::UnaryOp(ExpAst* exp, typeOp _op)
		{
			astnode_type = UNARY_OP;
			op = _op;
			c1=exp;
		}
		void UnaryOp::print()
		{
			tab_degree++;
			indent_print("(");
			cout << opLookup[op] << "\n";
			c1->print();
			cout << ")";
			tab_degree--;
		}


		FunCall::FunCall(ExpAst* _exp_ast)
		{
			astnode_type = FUNCALL;
			if( _exp_ast) list_exp_ast.push_back(_exp_ast);
		}
		void FunCall::setName(string fname)
		{
			func_name = fname;
		}
		void FunCall::print()
		{
			tab_degree++;
			indent_print( "("+func_name );
			for( auto i = list_exp_ast.begin() ; i != list_exp_ast.end() ; i++)
			{
				cout<<"\n";
				(*i)->print();
				
			}
			cout << ")";
			tab_degree--;
		}
		
		void FunCall::insert(ExpAst* e)
		{
			list_exp_ast.push_back(e);
		}




		IntConst::IntConst(int _val)
		{
			astnode_type = INT;
			val = _val;
		}
		void IntConst::print()
		{
			tab_degree++;
			indent_print("(IntConst ");
			cout << val << ")";
			tab_degree--;
		}



		StringConst::StringConst(std::string _val)
		{
			astnode_type = STR;
			val = _val;
		}
		void StringConst::print()
		{
			tab_degree++;
			indent_print("(StrConst ");
			cout << val << ")";
			tab_degree--;
		}




		Identifier::Identifier(std::string _val)
		{
			astnode_type = IDENTIFIER;
			val = _val;
		}
		void Identifier::print()
		{
			tab_degree++;
			indent_print("(Identifier ");
			cout << val << ")";
			tab_degree--;
		}



		Index::Index(ArrayRef* arrRef , ExpAst* expAst)
		{
			astnode_type = INDEX;
			c1 = arrRef;
			c2 = expAst;
		}
		void Index::print()
		{
			tab_degree++;
			indent_print("(ArrayRef\n");
			c1->print();
			indent_print("\n");
			c2->print();
			cout << ")";
			tab_degree--;
		}

 
