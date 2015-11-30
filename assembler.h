#ifndef ASM_HEADER
#define ASM_HEADER

#include "ast.h"
#include "tac.h"
#include <stdlib.h>

#define MAX_STRINGS 1024
#define MAX_STACK 1024

extern FILE* DEST_ASM;
HASH_NODE* stringTable[MAX_STRINGS];
HASH_NODE* stackOffsetControl[MAX_STACK];


void printHeaders();
void parseStrings();
void avengersAssemble(AST_NODE *astTree, TAC *tacList);
void parseGlobalVariables(AST_NODE *astTree);
void printGlobalVariableASM(AST_NODE *variableNode);
void parseTAC(TAC *tacList);
void printFooter();
int getDataStackIndex(HASH_NODE* hashNode);

#endif
