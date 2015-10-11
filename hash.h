/****INSTITUTO DE INFORMATICA UFRGS****
Leandro Mateus Giacomini Rocha
Luciano Farias Puhl
*/
#ifndef HASH_H
#define HASH_H

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 997

typedef struct hash_node
{
	char *symbol;
	int symbolType;
    int dataType;
	struct hash_node *nextNode;
} HASH_NODE;

HASH_NODE* symbolTable[HASH_SIZE];

void hashInitialize(void);
int hashGetAddress(char *symbol);
HASH_NODE* hashFind(char *symbol, int symbolType);
HASH_NODE* hashInsert(char *symbol, int symbolType);
void hashPrint(void);

#endif
