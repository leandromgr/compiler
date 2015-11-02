

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

TAC* tacJoin(TAC* tacList1, TAC* tacList2)
{
    TAC* joinedTac = NULL;

    if (!tacList1)
        return tacList2;
    if (!tacList2)
        return tacList1;

    while (tacList2->prev)
        tacList2 = tacList2->prev;

    tacList2->prev = tacList1;
    // XXX: See this
    joinedTac = tacList2;

    return joinedTac;
}

void tacPrint(TAC *tac)
{
	if(!tac)
		return;
	fprintf(stderr, "TAC(");

	switch(tac->type)
	{
        default: fprintf(stderr, "TAC_UNKNOWN");
	}

	fprintf(stderr, ")\n");

}
