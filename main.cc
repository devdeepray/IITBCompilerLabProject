#include <iostream>
#include <set>
#include "Scanner.h"
#include "Parser.h"
using namespace std;
int main (int argc, char** arg)
{
  Parser parser;
  parser.parse();
	parser._g_globalSymTable.print();

	if(argc == 1)
	{
		for(auto it = parser._g_globalSymTable.strFuncMap.begin(); 
				it != parser._g_globalSymTable.strFuncMap.end(); 
				++it)
		{
			it->second.print();
		}
	}
	else
	{	
	      
	  std::set<std::string> fnames;
	  for(int i = 1;  i < argc; ++i)
	    fnames.insert(std::string(arg[i]));
	  
	  for(auto it = parser._g_globalSymTable.strFuncMap.begin(); 
	      it != parser._g_globalSymTable.strFuncMap.end(); 
	      ++it)
	      {
		if(fnames.find(it->first.fname) != fnames.end())
		{
		  it->second.print();
		}
	      }
	}

}


