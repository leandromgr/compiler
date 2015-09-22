/****INSTITUTO DE INFORMATICA UFRGS****
Leandro Mateus Giacomini Rocha
Luciano Farias Puhl
*/

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern FILE* yyin;
extern int yylex();
extern char* yytext;

int main (int argc, char ** argv)
{
	FILE *srcFilePointer = NULL;
	FILE *dstFilePointer = NULL;
	
	if (argc != 2)
	{
		fprintf(stderr, "ERROR! Input and output files must be defined! \nUsage: etapa3 inputFile outputFile\n");
		exit(2);
	}

	initMe();

	srcFilePointer = fopen(argv[1], "r");

	if (srcFilePointer == NULL)
	{
		fprintf(stderr, "ERROR! The input file could not be opened! \nUsage: etapa3 inputFile outputFile\n");
		exit(2);
	}
	
	yyin = srcFilePointer;
	if (yyparse() != 0)
	{
		fprintf(stderr, "Syntax error in line: %i\n\n", getLineNumber()+1);
		exit(3);
	}

	/*dstFilePointer = fopen(argv[2], "w");

	if (dstFilePointer == NULL)
	{
		fprintf(stderr, "ERROR! The output file could not be opened! \nUsage: etapa3 inputFile outputFile\n");
		exit(2);
	}*/

	descompileTree(astTree);

	printf("Program sucessful!\n\n");
	exit(0);
}
