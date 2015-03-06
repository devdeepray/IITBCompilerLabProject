#include <iostream>
#include "Scanner.h"
#include "Parser.h"
using namespace std;
int main (int argc, char** arg)
{
  Parser parser;
  parser.parse();
	parser._g_globalSymTable.print();
	for(int i = 1; i < argc ; ++i)
	{
		parser._g_globalSymTable.strFuncMap[arg[i]].print();
	}

}


