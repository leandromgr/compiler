/****INSTITUTO DE INFORMATICA UFRGS****
Leandro Mateus Giacomini Rocha
Luciano Farias Puhl
*/

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "semantic.h"

extern FILE* yyin;
extern int yylex();
extern char* yytext;

FILE *DEST_FILE = NULL;

int main (int argc, char ** argv)
{
	FILE *srcFilePointer = NULL;
	
	if (argc != 3)
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

	DEST_FILE = fopen(argv[2], "w");

	if (DEST_FILE == NULL)
	{
		fprintf(stderr, "ERROR! The output file could not be opened! \nUsage: etapa3 inputFile outputFile\n");
		exit(2);
	}

    //descompileTree(astTree);
    //astPrint(astTree, 0);
    hashPrint();

    if (semanticErrors > 0)
	{
        printf("\n\nSemantic error!\n");
		exit(4);
	}
    else
	{
        printf("\n\nProgram sucessful!\n\n");
		exit(0);
	}
}
