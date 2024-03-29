integer-part [0-9]+
fractional-part [0-9]+
exponent-part [eE][+-]?[0-9]+

floating-point-constant (({integer-part}\.{fractional-part}?)|(\.{fractional-part})){exponent-part}?

integer-constant ([1-9][0-9]*)|(0[bB][01]*)|(0[0-7]*)|(0[xX][0-9a-fA-F]*)

identifier [_a-zA-Z][_a-zA-Z0-9]*

%%
{floating-point-constant}   {   
                                cat::token("FP: " + std::string(matched()));
                                Parser::d_val__ = std::string(matched()); 
                                return Parser::TOK_FP_CONST;
                            }

{integer-constant} {
                        cat::token("INT: " + std::string(matched()));
                        Parser::d_val__ = std::string(matched()); 
                        return Parser::TOK_INT_CONST;
                   }

\"((\\[tn\"\\])+|[^\\\"\n]+)*\" { 
                                    cat::token( "STRING_LITERAL" ); 
                                    Parser::d_val__ = std::string(matched());
                                    return Parser::TOK_STR_CONST;
                                }
                                
\/\/[^\n]* {}

void {cat::token( "VOID" ); return Parser::TOK_VOID_KW;}

int {cat::token( "INT" ); return Parser::TOK_INT_KW;}

float { cat::token( "FLOAT" ); return Parser::TOK_FLOAT_KW;}

for { cat::token( "FOR" ); return Parser::TOK_FOR_KW;}

while { cat::token( "WHILE" ); return Parser::TOK_WHILE_KW;}

if { cat::token( "IF" ); return Parser::TOK_IF_KW;}

else { cat::token( "ELSE" ); return Parser::TOK_ELSE_KW;}

return { cat::token( "RETURN" ); return Parser::TOK_RETURN_KW;}

{identifier} {  
                cat::token("ID: "+std::string(matched()));
                Parser::d_val__ = std::string(matched()); 
                return Parser::TOK_IDENTIFIER;
             }

[ \t]+ { /* Ignore hspace */}
\n {lineCount++;}

\( { cat::token( "LEFT_PAREN" ); return '(';}
\) { cat::token( "RIGHT_PAREN" ); return ')';}
\[ { cat::token( "LEFT_BOX" ); return '[';}
\] { cat::token( "RIGHT_BOX" ); return ']';}
\{ { cat::token( "LEFT_CURLY" ); return '{';}
\} { cat::token( "RIGHT_CURLY" ); return '}';}

, { cat::token( "COMMA" ); return ',';}
\. { cat::token( "DOT" ); return '.';}
; { cat::token( "SEMICOLON" ); return ';';}

= { cat::token( "ASSIGNMENT" ); return '=';}

== { cat::token( "EQUAL_TO" ); return Parser::TOK_EQ_OP;}
!= { cat::token( "NEQ_TO" ); return Parser::TOK_NEQ_OP;}
\< { cat::token( "LT_OP" ); return '<';}
\> { cat::token( "GT_OP" ); return '>';}
\<= { cat::token( "LEQ_OP" ); return Parser::TOK_LEQ_OP;}
\>= { cat::token( "GEQ_OP" ); return Parser::TOK_GEQ_OP;}
\+\+ { cat::token( "INCREMENT" ); return Parser::TOK_INCR_OP;}


! { cat::token( "EXCLAMATION" ); return '!';}
\- { cat::token( "MINUS" ); return '-';}
\+ { cat::token( "PLUS" ); return '+';}
\* { cat::token( "MULT" ); return '*';}
\/ { cat::token( "DIV" ); return '/';}
\% { cat::token( "MOD" ); return '%';}
& { cat::token( "AMPERSAND" ); return '&';}
&& { cat::token( "LOGICAL_AND" ); return Parser::TOK_LAND_OP;}
\| { cat::token( "VERT_BAR" ); return '|';}
\|\| { cat::token( "LOGICAL_OR" ); return Parser::TOK_LOR_OP;}
\^ { cat::token( "XOR" ); return '^';}
