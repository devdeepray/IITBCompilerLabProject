integer-part [0-9]+
fractional-part [0-9]+
exponent-part [eE][+-]?[0-9]+

floating-point-constant (({integer-part}\.{fractional-part}?)|(\.{fractional-part})){exponent-part}?
integer-constant ([1-9][0-9]*)|(0[bB][01]*)|(0[0-7]*)|(0[xX][0-9a-fA-F]*)

identifier [_a-zA-Z][_a-zA-Z0-9]*

%%
{floating-point-constant} {Parser::d_val__ = std::string(matched()); return Parser::FP_CONST;}

{integer-constant} {Parser::d_val__ = std::string(matched()); return Parser::INT_CONST;}

void {/*std::cout << "VOID" << std::endl;*/return Parser::VOID;}
int {/*std::cout << "INT" << std::endl;*/return Parser::INT;}
float { /*std::cout << "FLOAT" << std::endl;*/return Parser::FLOAT;}
for { /*std::cout << "FOR" << std::endl;*/return Parser::FOR;}
while { /*std::cout << "WHILE" << std::endl;*/return Parser::WHILE;}
if { /*std::cout << "IF" << std::endl;*/return Parser::IF;}
else { /*std::cout << "ELSE" << std::endl;*/return Parser::ELSE;}
return { /*std::cout << "RETURN" << std::endl;*/return Parser::RETURN;}

{identifier} { Parser::d_val__ = std::string(matched()); return Parser::IDENTIFIER;}

[ \t\n]+ {/* Can ignore whitespace*/ /* /*std::cout << "WHITESPACE" << std::endl;return WHITESPACE;*/}
\( { /*std::cout << "LEFT_PAREN" << std::endl;*/return '(';}
\) { /*std::cout << "RIGHT_PAREN" << std::endl;*/return ')';}
\[ { /*std::cout << "LEFT_BOX" << std::endl;*/return '[';}
\] { /*std::cout << "RIGHT_BOX" << std::endl;*/return ']';}
\{ { /*std::cout << "LEFT_CURLY" << std::endl;*/return '{';}
\} { /*std::cout << "RIGHT_CURLY" << std::endl;*/return '}';}
, { /*std::cout << "COMMA" << std::endl;*/return ',';}
\. { /*std::cout << "DOT" << std::endl;*/return '.';}
; { /*std::cout << "SEMICOLON" << std::endl;*/return ';';}
= { /*std::cout << "ASSIGNMENT" << std::endl;*/return '=';}
== { /*std::cout << "EQUAL_TO" << std::endl;*/return Parser::EQUAL_TO;}
!= { /*std::cout << "NEQ_TO" << std::endl;*/return Parser::NEQ_TO;}
\< { /*std::cout << "LT_OP" << std::endl;*/return '<';}
\> { /*std::cout << "GT_OP" << std::endl;*/return '>';}
\<= { /*std::cout << "LEQ_OP" << std::endl;*/return Parser::LEQ_OP;}
\>= { /*std::cout << "GEQ_OP" << std::endl;*/return Parser::GEQ_OP;}
\+\+ { /*std::cout << "INCREMENT" << std::endl;*/return Parser::INCREMENT;}

\"((\\[tn\"\\])+|[^\\\"\n]+)*\" { /*std::cout << "STRING_LITERAL" << std::endl;*/Parser::d_val__ = std::string(matched());return Parser::STRING_LITERAL;}

! { /*std::cout << "EXCLAMATION" << std::endl;*/return '!';}
\- { /*std::cout << "MINUS" << std::endl;*/return '-';}
\+ { /*std::cout << "PLUS" << std::endl;*/return '+';}
\* { /*std::cout << "MULT" << std::endl;*/return '*';}
\/ { /*std::cout << "DIV" << std::endl;*/return '/';}
\% { /*std::cout << "MOD" << std::endl;*/return '%';}
& { /*std::cout << "AMPERSAND" << std::endl;*/return '&';}
&& { /*std::cout << "LOGICAL_AND" << std::endl;*/return Parser::LOGICAL_AND;}
\| { /*std::cout << "VERT_BAR" << std::endl;*/return '|';}
\|\| { /*std::cout << "LOGICAL_OR" << std::endl;*/return Parser::LOGICAL_OR;}
\^ { /*std::cout << "XOR" << std::endl;*/return '^';}
