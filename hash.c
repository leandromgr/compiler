#include "hash.h"

void hashInitialize(void)
{
	int i;
	for(i=0 ; i<HASH_SIZE ; i++)
	{
		symbolTable[i]=0;
	}
}

int hashGetAddress(char *symbol)
{

}

HASH_NODE* hashFind(char *symbol)
{

}

HASH_NODE* hashInsert(char *symbol)
{

}