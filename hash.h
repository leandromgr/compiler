#define HASH_SIZE 997

typedef struct hash_node
{
	char *symbol;
	struct hash_node *nextNode;
} HASH_NODE;

HASH_NODE* symbolTable[HASH_SIZE];

void hashInitialize(void);
int hashGetAddress(char *symbol);
HASH_NODE* hashFind(char *symbol);
HASH_NODE* hashInsert(char *symbol);