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

TAC* generateTacs(AST_NODE* astNode)
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
			return tacCreate(TAC_SYMBOL, astNode->hashNode, NULL, NULL);
			
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
		{
			TAC* funcall = tacCreate(TAC_CALL, astNode->hashNode, NULL, NULL);
			AST_NODE* currentArgument = astNode->children[0];
			while(currentArgument)
			{
				funcall = tacJoin(funcall, tacCreate(TAC_ARG, currentArgument->hashNode, NULL, NULL));

				AST_NODE* currentArgument = currentArgument->children[0];
			}
		}

		case AST_FUNCTION_LIST:
			return tacJoin(generateChild[0], generateChild[1]);

		case AST_FUNCTION:
		{
			TAC* newFunction = tacCreate(TAC_FUNBEGIN, NULL, astNode->hashNode, NULL);

			//Inserting the TACs related to the function commands
			newFunction = tacJoin(generateChild[3], newFunction);

			//Inserting the Function End TAC after the commands and returning the function.
			return  tacJoin(newFunction, tacCreate(TAC_FUNEND, NULL, NULL, NULL);
		}
			
		case AST_CMD_LIST:
			return tacJoin(generateChild[0], generateChild[1]);

		case AST_ATTRIBUTION:
		{
			//The first argument is the expression to be executed.
			//The second argument is the new node of MOV operation, moving the expression result to the desintation variable.
			return tacJoin(generateChild[1], tacCreate(TAC_MOV, generateChild[0], generateChild[1]->res, NULL));
		}
			
		case AST_INPUT_CMD:
			return tacCreate(TAC_READ, astNode->hashNode, NULL, NULL);

		case AST_OUTPUT_CMD:
			return generateChild[0];

		case AST_OUTPUT_LIST:
		{
			//The first argument in the first tacJoin can be an expression or a dummie tac_symbol.
			TAC* newPrintCmd = tacJoin(generateChild[0], tacCreate(TAC_PRINT, NULL, generateChild[0]->res));
			return tacJoin(newPrintCmd, generateChild[1]);
		}

		case AST_RETURN_CMD:
		{
			//The return is a tacJoin between the expression to be returned and the new return command.
			return tacJoin(generateChild[0], tacCreate(TAC_RETURN, NULL, generateChild[0]->res, NULL));
		}
			
		case AST_IF:
            return generateIfThen(generateChild[0], generateChild[1]);

		case AST_IFELSE:
            return generateIfThenElse(generateChild[0], generateChild[1], generateChild[2]);

		case AST_LOOP:

		//[?]case AST_GLOBAL_VAR_LIST:
		//[?]case AST_GLOBAL_VECTOR:
		//[?]case AST_INT:
		//[?]case AST_CHAR:
		//[?]case AST_BOOL:
		//[?]case AST_REAL:
		//[?]case AST_PARAMETER_LIST:
		//[?]case AST_LOCAL_VAR_LIST:
	}
}

TAC* generateIfThen(TAC* booleanExpression, TAC* code)
{
    HASH_NODE* endIfLabel = NULL;
    TAC* exprEvalJump = NULL;
    TAC* endIfPointer = NULL;

    // Create the jump label that points to the first command after the end of IF COMMAND BLOCK
    endIfLabel = makeLabel();
    endIfPointer = tacCreate(TAC_LABEL, endIfLabel, 0, 0);

    // Create a node to eval the boolean expression. If the expression is false, do not execute the IF code by
    // jumping to endIfLabel
    exprEvalJump = tacCreate(TAC_JZ, endIfLabel, booleanExpression ? booleanExpression->res : 0, 0);

    // Compute the expression, evaluate it and, if true, execute the "code"
    return tacJoin(tacJoin(tacJoin(booleanExpression, exprEvalJump), code), endIfPointer);
}

TAC* generateIfThenElse(TAC* booleanExpression, TAC* codeTrue, TAC* codeFalse)
{
    HASH_NODE* elseStartLabel = NULL;
    HASH_NODE* endIfLabel = NULL;
    TAC* exprEvalJump = NULL;
    TAC* jumpBeforeElse = NULL;
    TAC* endIfPointer = NULL;
    TAC* elseStartPointer = NULL;
    TAC* ifTrueCode = NULL;
    TAC* ifFalseCode = NULL;

    // Create the jump label that points to the first command after the end of IF COMMAND BLOCK
    endIfLabel = makeLabel();
    endIfPointer = tacCreate(TAC_LABEL, endIfLabel, 0, 0);

    // Create the label to where it should jump if the expression is false
    elseStartLabel = makeLabel();
    elseStartPointer = tacCreate(TAC_LABEL, elseStartLabel, 0, 0);

    // Create a node to eval the boolean expression. If the expression is false, jump to the ELSE code
    exprEvalJump = tacCreate(TAC_JZ, elseStartLabel, booleanExpression ? booleanExpression->res : 0, 0);
    ifFalseCode = tacJoin(elseStartPointer, codeFalse);

    // If the expression is true, it shall jump unconditionally to the end of the if command and DO NOT EXECUTE the else code
    jumpBeforeElse = tacCreate(TAC_JUMP, endIfLabel, 0, 0);
    ifTrueCode = tacJoin(codeTrue, jumpBeforeElse);

    // Compute the expression, evaluate it and execute the code accordingly
    return tacJoin(tacJoin(tacJoin(tacJoin(booleanExpression, exprEvalJump), ifTrueCode), ifFalseCode), endIfPointer);
}

TAC* generateBinaryOperation(int tacType, TAC* child0, TAC* child1)
{
	TAC* binaryOperationTac =  tacCreate(tacType ,makeTemp(), child0->res, child1->res);
	return tacJoin(child0, tacJoin(child1, binaryOperationTac));
}
