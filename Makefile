.PHONY: clean
all:    lex.cc parse.cc main.cc Scanner.h Scannerbase.h Scanner.ih Parser.h Parserbase.h Parser.ih
	g++   --std=c++0x lex.cc parse.cc main.cc;
	

lex.cc: lex.l Scanner.ih 
	flexc++ lex.l; 
	sed -i '/include/a #include "Parserbase.h"' Scanner.ih; 

parse.cc: parse.y Parser.ih Parser.h Parserbase.h
	bisonc++  parse.y; 
	sed -i '/d_scanner;/r makeTree.txt' Parser.h; 

graph:
	rm -f *graph
	echo "digraph G{ordering=out;" >> graph.gv
	./a.out < TestCases/testCase >> graph.gv
	echo "}" >> graph.gv
	dot -Tps graph.gv -o graph.ps

clean:
	rm -f Scanner*
	rm -f Parser*
	rm -f a.out
	rm -f lex.cc
	rm -f parse.cc
	rm -f graph.gv
	rm -f graph.ps

Parser.ih: parse.y
Parser.h:  parse.y
Parserbase.h: parse.y
Scanner.ih: lex.l
Scanner.h: lex.l
Scannerbase.h: lex.l
