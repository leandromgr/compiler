

#include "tac.h"

int currentLabelIndex = 0;
int currentTempVarIndex = 0;

TAC* tacCreate(int type, HASH_NODE* res, HASH_NODE* op1, HASH_NODE* op2)
{
    TAC* newTac = NULL;

    newTac = (TAC*) calloc(1, sizeof(TAC));
    newTac->type = type;
    newTac->res = res;
    newTac->op1 = op1;
    newTac->op2 = op2;
    newTac->prev = NULL;
    newTac->next = NULL;

    return newTac;
}

void tacPrint(TAC *tac)
{
	if(!tac)
		return;
	fprintf(stderr, "TAC(");

	switch(tac->type)
	{
		default fprintf(stderr, "TAC_UNKNOWN");
	}

	fprintf(stderr, ")\n");

}