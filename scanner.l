
%{
	#include <stdio.h>
	#include "tokens.h"
	int lineCounter = 0;
    
	// Execution control
	int isRunning = 1;
%}

%%
"int"		{ return KW_INT; }
"real"		{ return KW_CHAR; }
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

[-+,;:()\[\]{}*/<>=!&$] { return yytext[0]; }

[a-zA-Z][a-zA-Z_]*  { return TK_IDENTIFIER; }

[0-9]+		{ return LIT_INTEGER; }
'[\0-\377]'	{ return LIT_CHAR; }
TRUE		{ return LIT_TRUE; }
FALSE		{ return LIT_FALSE; }

"\n"		{ lineCounter++; }
" "			{ }
.			{ return TOKEN_ERROR; }
%%

int main (int argc, char ** argv)
{
	FILE *filePointer = NULL;
	int readToken;

	filePointer = fopen("text.txt", "r");
	yyin = filePointer;

	while (isRunning)
	{
		readToken = yylex();

		// Check if it is not EOF
		if (!isRunning)
		{
			break;
		}

		// Check which token was read
		switch (readToken)
		{
			case TK_IDENTIFIER:
			{
				printf("Identifier: %s\n", yytext);
				break;
			}

			case LIT_CHAR:
			{
				printf("Literal char: %c\n", yytext[0]);
				break;
			}
	
			case LIT_TRUE:
			{
				printf("Literal TRUE: %s\n", yytext);
				break;
			}
			
			case LIT_FALSE:
			{
				printf("Literal FALSE: %s\n", yytext);
				break;
			}
			
			case LIT_INTEGER:
			{
				printf("Literal number: %s\n", yytext);
				break;
			}
			
			case KW_INT:
			case KW_REAL:
			case KW_BOOL:
			case KW_CHAR:
			case KW_IF:
			case KW_ELSE:
			case KW_LOOP:
			case KW_INPUT:
			case KW_RETURN:
			case KW_OUTPUT:
			{
				printf("Reserved word found: %s\n", yytext);
				break;
			}

			case TOKEN_ERROR:
			{
				printf("Invalid token: %s\n", yytext);
				break;
			}


			default:
			{
				printf("Symbol: %c\n", yytext[0]);
				break;
			}
		}
	}
	
	printf("Number of lines in the parsed file: %i\n", lineCounter);
	return 0;
}

int yywrap()
{
	isRunning = 0;
	return 1;
}
