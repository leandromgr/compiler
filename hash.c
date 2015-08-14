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
	int i;
	int address = 1;

	for(i=0 ; i<strlen(symbol) ; i++)
	{
		address = (address * symbol[i])%HASH_SIZE;
	}

	return address;
}

HASH_NODE* hashFind(char *symbol, int symbolType)
{
	int nodeAddress;

	//The possible address of the symbol
	nodeAddress = hashGetAddress(symbol);

	HASH_NODE *currentNode;
	for(currentNode = symbolTable[nodeAddress] ; currentNode ; currentNode = currentNode->nextNode
)	{
		int foundSymbol = !strcmp(currentNode->symbol, symbol) &&
						  (currentNode->symbolType == symbolType);

		if(foundSymbol)
		{
			//Found the symbol in the table
			return currentNode;
		}
	}
	//The symbol is not in the table.
	return NULL;
}

HASH_NODE* hashInsert(char *symbol, int symbolType)
{
	int newNodeAddress;
	HASH_NODE *newNode;

	newNode = hashFind(symbol, symbolType);

	if(newNode==NULL)
	{
		//Allocating memory
		newNode = (HASH_NODE*) calloc(1,sizeof(HASH_NODE));
		newNode->symbol = (char*) calloc(strlen(symbol)+1,sizeof(char));

		//Filling the node information content
		strcpy(newNode->symbol, symbol);
		newNode->symbolType=symbolType;

		//The address of the new symbol
		newNodeAddress = hashGetAddress(symbol);

		//Inserting the node in the first position of the table address.
		newNode->nextNode = symbolTable[newNodeAddress];
		symbolTable[newNodeAddress] = newNode;
	}

	return newNode;
}

void hashPrint(void)
{
	int i;
	HASH_NODE *currentNode;

	for(i=0 ; i<HASH_SIZE; i++)
	{
		for(currentNode=symbolTable[i] ; currentNode ; currentNode = currentNode->nextNode)
		{
			printf("symbolTable[%d]: %s\n", i, currentNode->symbol);
		}
	}
}