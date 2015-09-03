/****INSTITUTO DE INFORMATICA UFRGS****
Leandro Mateus Giacomini Rocha
Luciano Farias Puhl
*/

#include <stdio.h>
#include "tokens.h"

extern FILE* yyin;
extern int yylex();
extern char* yytext;

int main (int argc, char ** argv)
{
	FILE *filePointer = NULL;
	int readToken;

	initMe();

	filePointer = fopen("test1.txt", "r");
	yyin = filePointer;

	printf("====================--------------------------->Parsing result: %d\n ", yyparse());

	/*while (isRunning())
	{
		readToken = yylex();
		//printf("yytext: %s\n", yytext);
		// Check if it is not EOF
		if (!isRunning())
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
				printf("Literal char: %c\n", yytext[1]);
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
			
			case LIT_STRING:
			{
				printf("Literal String: %s\n", yytext);
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
	*/	
	printf("Number of lines in the parsed file: %i\n", getLineNumber());

	hashPrint();
	return 0;
}
