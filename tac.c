#include "tac.h"

int currentLabelIndex = 0;
int currentTempVarIndex = 0;
char hashBuffer[256];

HASH_NODE * makeTemp()
{
    sprintf(hashBuffer, "_temp%d", currentTempVarIndex++);
    return hashInsert(hashBuffer, SYMBOL_IDENTIFIER);
}

HASH_NODE * makeLabel()
{
    sprintf(hashBuffer, "_label%d", currentLabelIndex++);
    return hashInsert(hashBuffer, SYMBOL_IDENTIFIER);
}

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

    joinedTac = tacList2;

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

TAC* generateTacs(AST* astNode)
{
	int i;
	TAC* result[MAX_CHILDREN];

	if(!astNode)
		return;

	for(i=0 ; i<MAX_CHILDREN ; i++)
	{
		generateChild[i] = generateTacs(astNode->children[i]);
	}

	switch(astNode->type)
	{
		case AST_SYMBOL:
			return tacCreate(TAC_SYMBOL, astNode->hashNode, 0,0);
			break;

		case AST_SUM:
		case AST_SUB:
		case AST_MULT:
		case AST_DIV:
		case AST_LT:
		case AST_GT:
		case AST_LET:
		case AST_GET:
		case AST_EQ:
		case AST_NE:
		case AST_AND:
		case AST_OR:
		case AST_FUNCALL:
		case AST_GLOBAL_VAR_LIST:
		case AST_GLOBAL_VECTOR:
		case AST_INT:
		case AST_CHAR:
		case AST_BOOL:
		case AST_REAL:
		case AST_FUNCTION_LIST:
		case AST_FUNCTION:
		case AST_PARAMETER_LIST:
		case AST_LOCAL_VAR_LIST:
		case AST_ATTRIBUTION:
		case AST_INPUT_CMD:
		case AST_OUTPUT_CMD:
		case AST_RETURN_CMD:
		case AST_OUTPUT_LIST:
		case AST_IF:
		case AST_IFELSE:
		case AST_LOOP:
		case AST_CMD_LIST:
	}
}

generateBinaryOperation(int astType, TAC* child0, TAC* child1)
{
	tacJoin();
}
