
%{
	#include <stdio.h>
	#include "tokens.h"
	int lineCounter = 0;
    
	// Execution control
	int isRunning = 1;
%}

%%
"int"		{ return KW_INT; }
"real"		{ return KW_REAL; }
"bool"		{ return KW_BOOL; }
"char"		{ return KW_CHAR; }
"if"		{ return KW_IF; }
"else"		{ return KW_ELSE; }
"loop"		{ return KW_LOOP; }
"input"		{ return KW_INPUT; }
"return"	{ return KW_RETURN; }
"output"	{ return KW_OUTPUT; } 

"<="		{ return OPERATOR_LE; }
">="		{ return OPERATOR_GE; }
"=="		{ return OPERATOR_EQ; }
"!="		{ return OPERATOR_NE; }
"&&"		{ return OPERATOR_AND; }
"||"		{ return OPERATOR_OR; }

[a-zA-Z][a-zA-Z_]*  { return TK_IDENTIFIER; }

[0-9]+		{ return LIT_INTEGER; }
[\0-\377]	{ return LIT_CHAR; }
TRUE		{ return LIT_TRUE; }
FALSE		{ return LIT_FALSE; }

[-+,;:()\[\]{}*/<>=!&$] { return yytext[0]; }

"\n"		{ lineCounter++; }
" "			{ }
.			{ return TOKEN_ERROR; }
%%

int yywrap()
{
	isRunning = 0;
	return 1;
}
