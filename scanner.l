
%{
	#include <stdio.h>
	#include "tokens.h"
	int lineCounter = 0;
    
	// Execution control
	int flagIsRunning = 1;
%}

%x COMMENT
%x STRING

%%
"int"			{ return KW_INT; }
"real"			{ return KW_REAL; }
"bool"			{ return KW_BOOL; }
"char"			{ return KW_CHAR; }
"if"			{ return KW_IF; }
"else"			{ return KW_ELSE; }
"loop"			{ return KW_LOOP; }
"input"			{ return KW_INPUT; }
"return"		{ return KW_RETURN; }
"output"		{ return KW_OUTPUT; } 

"<="			{ return OPERATOR_LE; }
">="			{ return OPERATOR_GE; }
"=="			{ return OPERATOR_EQ; }
"!="			{ return OPERATOR_NE; }
"&&"			{ return OPERATOR_AND; }
"||"			{ return OPERATOR_OR; }

TRUE			{ return LIT_TRUE; }
FALSE			{ return LIT_FALSE; }
[0-9]+			{ return LIT_INTEGER; }
'[\00-\377]'	{ return LIT_CHAR; }

[a-zA-Z][a-zA-Z_]*  { return TK_IDENTIFIER; }

[-+,;:()\[\]{}*/<>=!&$] { return yytext[0]; }

"//".*			{ }
"/*"			{ BEGIN(COMMENT); }
<COMMENT>.		{ }
<COMMENT>"\n"	{ lineCounter++; }
<COMMENT>"*/"	{ BEGIN(INITIAL); }

\"[^"]			{ BEGIN(STRING); }
<STRING>.		{  }
<STRING>[^\\]\"	{ BEGIN(INITIAL); return LIT_STRING; }

"\n"			{ lineCounter++; }
" "				{ }
.				{ return TOKEN_ERROR; }
%%

int isRunning()
{
	return flagIsRunning;
}

int yywrap()
{
	flagIsRunning = 0;
	return 1;
}

int getLineNumber()
{
	return lineCounter;
}
