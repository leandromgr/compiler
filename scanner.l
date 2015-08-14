
%{
	#include <stdio.h>
	#include "tokens.h"
	int lineCounter = 0;
    
	// Execution control
	int isRunning = 1;
%}

%%
"word"		{ return KW_WORD; }
"bool"		{ return KW_BOOL; }
"byte"		{ return KW_BYTE; }
"if"		{ return KW_IF; }
"then"		{ return KW_THEN; }
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

[a-zA-Z_][a-zA-Z_0-9]*  { return TK_IDENTIFIER; }

[0-9]+		{ return LIT_INTEGER; }

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
