#include "tac.h"
#include "ast.h"

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
		case AST_SUM:
			return generateBinaryOperation(TAC_SUM, generateChild[0], generateChild[1]);
		case AST_SUB:
			return generateBinaryOperation(TAC_SUB, generateChild[0], generateChild[1]);
		case AST_MULT:
			return generateBinaryOperation(TAC_MULT, generateChild[0], generateChild[1]);
		case AST_DIV:
			return generateBinaryOperation(TAC_DIV, generateChild[0], generateChild[1]);
		case AST_LT:
			return generateBinaryOperation(TAC_LT, generateChild[0], generateChild[1]);
		case AST_GT:
			return generateBinaryOperation(TAC_GT, generateChild[0], generateChild[1]);
		case AST_LET:
			return generateBinaryOperation(TAC_LET, generateChild[0], generateChild[1]);
		case AST_GET:
			return generateBinaryOperation(TAC_GET, generateChild[0], generateChild[1]);
		case AST_EQ:
			return generateBinaryOperation(TAC_EQ, generateChild[0], generateChild[1]);
		case AST_NE:
			return generateBinaryOperation(TAC_NE, generateChild[0], generateChild[1]);
		case AST_AND:
			return generateBinaryOperation(TAC_AND, generateChild[0], generateChild[1]);
		case AST_OR:
			return generateBinaryOperation(TAC_OR, generateChild[0], generateChild[1]);
		case AST_FUNCALL:
			//- criar um TAC_CALL p/ HASH_NODE tipo FUNCTION que já existe na hash.
		case AST_GLOBAL_VAR_LIST:
		case AST_GLOBAL_VECTOR:
		case AST_INT:
		case AST_CHAR:
		case AST_BOOL:
		case AST_REAL:
		case AST_FUNCTION_LIST:
			//- Simplesmente itera os fillhos.
		case AST_FUNCTION:
			//- Temos que criar o TAC_BEGINFUN e o TAC_ENDFUN
			//- Temos que colocar todos os comandos da função entre essas duas Tacs.
		case AST_PARAMETER_LIST:
		case AST_LOCAL_VAR_LIST:

		case AST_CMD_LIST:
			return tacJoin(generateChild[0], generateChild[1]);

		case AST_ATTRIBUTION:
			//The first argument is the expression to be executed.
			//The second argument is the new node of MOV operation, moving the expression result to the desintation variable.
			return tacJoin(generateChild[1], tacCreate(TAC_MOV, generateChild[0], generateChild[1]->res, NULL));
		case AST_INPUT_CMD:
			return tacCreate(TAC_READ, astNode->hashNode, NULL, NULL);
		case AST_OUTPUT_CMD:
			return generateChild[0];
		case AST_OUTPUT_LIST:
			//The first argument in the first tacJoin can be an expression or a dummie tac_symbol.
			TAC* newPrintCmd = tacJoin(generateChild[0], tacCreate(TAC_PRINT, NULL, generateChild[0]->res));
			return tacJoin(newPrintCmd, generateChild[1]);
		case AST_RETURN_CMD:
			//The return is a tacJoin between the expression to be returned and the new return command.
			return tacJoin(generateChild[0], tacCreate(TAC_RETURN, NULL, generateChild[0]->res, NULL));
		case AST_IF:
		case AST_IFELSE:
		case AST_LOOP:
	}
}

generateBinaryOperation(int tacType, TAC* child0, TAC* child1)
{
	TAC* binaryOperationTac =  tacCreate(tacType ,makeTemp(), child0->res, child1->res);
	return tacJoin(child0, tacJoin(child1, binaryOperationTac));
}