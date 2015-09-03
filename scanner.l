%{
/****INSTITUTO DE INFORMATICA UFRGS****
Leandro Mateus Giacomini Rocha
Luciano Farias Puhl
*/

	#include <stdio.h>
	#include <string.h>
	#include "y.tab.h"
	#include "hash.h"
	// Physical line counter
	int lineCounter = 0;

	// Buffer for strings
	char stringBuffer[4097];
	int stringOffset;
    
	// Execution control
	int flagIsRunning = 1;

	// Generate function prototypes
	void copyStringToBuffer(char * newString);
	void resetStringBuffer();
	char processEscapeChar(char escapeChar);
%}

%x COMMENT
%x STRING
%x STRING_ESCAPE

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

TRUE			{ hashInsert(yytext, LIT_TRUE); return LIT_TRUE; }
FALSE			{ hashInsert(yytext, LIT_FALSE); return LIT_FALSE; }
[0-9]+			{ hashInsert(yytext, LIT_INTEGER); return LIT_INTEGER; }

''				{ char processedChar[1] = {'\0'};
				  hashInsert(processedChar, LIT_CHAR); return LIT_CHAR; }
'[\00-\377]'	{ char processedChar[2] = {yytext[1], '\0'};
				  hashInsert(processedChar, LIT_CHAR); return LIT_CHAR; }
'\\[\00-\377]'	{ char processedChar[2] = {'\0', '\0'};
				  processedChar[0] = processEscapeChar(yytext[2]);
				  hashInsert(processedChar, LIT_CHAR); return LIT_CHAR; }

[a-zA-Z][a-zA-Z_]*  { hashInsert(yytext, TK_IDENTIFIER); return TK_IDENTIFIER; }

[-+,;:()\[\]{}*/<>=!&$] { return yytext[0]; }

"//".*			{ }
"/*"			{ BEGIN(COMMENT); }
<COMMENT>.		{ }
<COMMENT>"\n"	{ lineCounter++; }
<COMMENT>"*/"	{ BEGIN(INITIAL); }

\"				{ resetStringBuffer(); BEGIN(STRING); }
<STRING>[^"\\\n]*	{ copyStringToBuffer(yytext); }
<STRING>\\		{ BEGIN(STRING_ESCAPE); }
<STRING>\"		{ hashInsert(stringBuffer, LIT_STRING); 
				  BEGIN(INITIAL); 
				  return LIT_STRING; }
<STRING>"\n"	{ lineCounter++;
				  BEGIN(INITIAL);
				  return TOKEN_ERROR;
				}

<STRING_ESCAPE>.	{ char processedChar[2] = {'\0', '\0'};
					  processedChar[0] = processEscapeChar(yytext[0]);
					  copyStringToBuffer(processedChar);
					  BEGIN(STRING); }

"\n"			{ lineCounter++; }
[ \t]			{ }
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

void resetStringBuffer()
{
	memset(stringBuffer, 0, 4097);
	stringOffset = 0;
}

void copyStringToBuffer(char * newString)
{
	int newStringLength = strlen(newString);

	// Copyt the new string to the buffer already filled up to the offset
	strncpy((char *) (stringBuffer + stringOffset), newString, newStringLength);

	stringOffset += newStringLength;
}

char processEscapeChar(char escapeChar)
{
	switch(escapeChar)
	{
		case 'n':
			return '\n';
		case 't':
			return '\t';
		default:
			return escapeChar; 
	}
}

void initMe()
{
	resetStringBuffer();
	hashInitialize();
}
