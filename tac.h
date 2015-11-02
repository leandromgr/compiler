#ifndef TAC_HEADER
#define TAC_HEADER

#include "hash.h"

typedef struct tac_node
{
	int type;
	HASH_NODE* res;
	HASH_NODE* op1;
	HASH_NODE* op2;
	struct tac_node* prev;
	struct tac_node* next;
} TAC;

TAC* tacCreate(int type, HASH_NODE* res, HASH_NODE* op1, HASH_NODE* op2);
void tacPrint(TAC *tac);
TAC* tacJoin(TAC* tacList1, TAC* tacList2);

#endif