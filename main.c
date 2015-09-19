/****INSTITUTO DE INFORMATICA UFRGS****
Leandro Mateus Giacomini Rocha
Luciano Farias Puhl
*/

#include <stdio.h>
#include <stdlib.h>

extern FILE* yyin;
extern int yylex();
extern char* yytext;

int main (int argc, char ** argv)
{
	FILE *filePointer = NULL;
	
	if (argc != 2)
	{
		fprintf(stderr, "ERROR! No file was specified! \nUsage: etapa3 testFile\n");
		exit(2);
	}

	initMe();

	filePointer = fopen(argv[1], "r");

	if (filePointer == NULL)
	{
		fprintf(stderr, "ERROR! The file could not be opened! \nUsage: etapa3 testFile\n");
		exit(2);
	}
	
	yyin = filePointer;
	if (yyparse() != 0)
	{
		fprintf(stderr, "Syntax error in line: %i\n\n", getLineNumber()+1);
		exit(3);
	}
	
	printf("Program sucessful!\n\n");
	exit(0);
}
