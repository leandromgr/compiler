#include "tac.h"
#include "y.tab.h"

int currentLabelIndex = 0;
int currentTempVarIndex = 0;
char hashBuffer[256];

HASH_NODE * makeTemp(int dataType)
{
    HASH_NODE* hashNode = NULL;
    sprintf(hashBuffer, "_temp%d", currentTempVarIndex++);
    hashNode = hashInsert(hashBuffer, TK_IDENTIFIER);
    hashNode->dataType = dataType;
    return hashNode;
}

HASH_NODE * makeLabel()
{
    sprintf(hashBuffer, "_label%d", currentLabelIndex++);
    return hashInsert(hashBuffer, TK_IDENTIFIER);
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

TAC * tacReverseCode(TAC * tac)
{
    if(!tac)
        return NULL;
    if (tac->prev)
    {
        tac->prev->next = tac;
        return tacReverseCode(tac->prev);
    }
    else
        return tac;
}

void tacPrintNext(TAC * tac)
{
    tacPrint(tac);
    if (tac)
        tacPrintNext(tac->next);
}


void tacPrintPrev(TAC * tac)
{
    tacPrint(tac);
    if (tac)
        tacPrintPrev(tac->prev);
}

void tacPrint(TAC *tac)
{
	if(!tac)
		return;
	fprintf(stderr, "TAC(");

	switch(tac->type)
	{
        case TAC_SYMBOL:
            fprintf(stderr, "TAC_SYMBOL"); break;
        case TAC_SUM:
            fprintf(stderr, "TAC_SUM"); break;
        case TAC_SUB:
            fprintf(stderr, "TAC_SUB"); break;
        case TAC_MULT:
            fprintf(stderr, "TAC_MULT"); break;
        case TAC_DIV:
            fprintf(stderr, "TAC_DIV"); break;
        case TAC_LT:
            fprintf(stderr, "TAC_LT"); break;
        case TAC_GT:
            fprintf(stderr, "TAC_GT"); break;
        case TAC_LET:
            fprintf(stderr, "TAC_LET"); break;
        case TAC_GET:
            fprintf(stderr, "TAC_GET"); break;
        case TAC_EQ:
            fprintf(stderr, "TAC_EQ"); break;
        case TAC_NE:
            fprintf(stderr, "TAC_NE"); break;
        case TAC_AND:
            fprintf(stderr, "TAC_AND"); break;
        case TAC_OR:
            fprintf(stderr, "TAC_OR"); break;
        case TAC_MOV:
            fprintf(stderr, "TAC_MOV"); break;
        case TAC_READ:
            fprintf(stderr, "TAC_READ"); break;
        case TAC_PRINT:
            fprintf(stderr, "TAC_PRINT"); break;
        case TAC_RETURN:
            fprintf(stderr, "TAC_RETURN"); break;
        case TAC_LABEL:
            fprintf(stderr, "TAC_LABEL"); break;
        case TAC_JZ:
            fprintf(stderr, "TAC_JZ"); break;
        case TAC_JUMP:
            fprintf(stderr, "TAC_JUMP"); break;
        case TAC_CALL:
            fprintf(stderr, "TAC_CALL"); break;
        case TAC_ARG:
            fprintf(stderr, "TAC_ARG"); break;
        case TAC_FUNBEGIN:
            fprintf(stderr, "TAC_FUNBEGIN"); break;
        case TAC_FUNEND:
            fprintf(stderr, "TAC_FUNEND"); break;
        case TAC_PARAM:
            fprintf(stderr, "TAC_PARAM"); break;
        case TAC_MOV_INIT:
            fprintf(stderr, "TAC_MOV_INIT"); break;
        default:
            fprintf(stderr, "TAC_UNKNOWN"); break;
	}
    fprintf(stderr, ", ");
    if (tac->res)
        fprintf(stderr, "%s, ", tac->res->symbol);
    else
        fprintf(stderr, "NULL, ");
    if (tac->op1)
        fprintf(stderr, "%s, ", tac->op1->symbol);
    else
        fprintf(stderr, "NULL, ");
    if (tac->op2)
        fprintf(stderr, "%s)", tac->op2->symbol);
    else
        fprintf(stderr, "NULL)");
    fprintf(stderr, "\n");

    if (tac->type == TAC_FUNEND)
        fprintf(stderr, "\n");
}

TAC* generateTacs(AST_NODE * astNode)
{
	int i;
    TAC* generateChild[MAX_CHILDREN];

	if(!astNode)
        return NULL;

    for(i = 0; i < MAX_CHILDREN; i++)
	{
		generateChild[i] = generateTacs(astNode->children[i]);
	}

	switch(astNode->type)
	{
        case AST_SYMBOL:
            // TODO: See vector index
            if (generateChild[0])
                return tacJoin(generateChild[0], tacCreate(TAC_SYMBOL, astNode->hashNode, generateChild[0]->res, NULL));
            else
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
            AST_NODE * functionReference = (AST_NODE *) astNode->hashNode->functionLink;
            HASH_NODE* functionReturn = makeTemp(functionReference->hashNode->dataType);
            TAC* funcall = tacCreate(TAC_CALL, functionReturn, astNode->hashNode, NULL);
            AST_NODE* currentArgument = astNode->children[0];
            TAC* argumentList = NULL;
			while(currentArgument)
			{
                argumentList = tacJoin(argumentList, tacCreate(TAC_ARG, currentArgument->hashNode, NULL, NULL));

                currentArgument = currentArgument->children[0];
            }

            funcall = tacJoin(argumentList, funcall);
            return funcall;
		}

		case AST_FUNCTION_LIST:
			return tacJoin(generateChild[0], generateChild[1]);

		case AST_FUNCTION:
		{
            TAC* newFunction = tacCreate(TAC_FUNBEGIN, NULL, astNode->children[0]->hashNode, NULL);
            
            
            newFunction = tacJoin(newFunction, generateChild[0]);

            //Inserting the TAC_MOV related to the variable initialization.
            newFunction = tacJoin(newFunction, generateChild[1]);

			//Inserting the TACs related to the function commands
            newFunction = tacJoin(newFunction, generateChild[2]);

			//Inserting the Function End TAC after the commands and returning the function.
            return  tacJoin(newFunction, tacCreate(TAC_FUNEND, NULL, NULL, NULL));
		}

        //Only used in the function parameter case:
        case AST_INT:
        case AST_CHAR:
        case AST_BOOL:
        case AST_REAL:
        {
            if ((astNode->children[0]) && (astNode->children[0]->type == AST_PARAMETER_LIST))
            {
                return generateChild[0];
            }
            return NULL;
        }

        case AST_PARAMETER_LIST:
        {
            TAC* newParameter = NULL;
            if (astNode->children[0])
                newParameter = tacCreate(TAC_PARAM, NULL, astNode->children[0]->hashNode, NULL);
            return tacJoin(generateChild[1], newParameter);
        }

        case AST_LOCAL_VAR_LIST:
        {
            //TAC* initialValue = tacCreate(TAC_SYMBOL, astNode->children[0]->hashNode, NULL, NULL);
            //TAC* variableName = tacCreate(TAC_SYMBOL, astNode->hashNode, NULL, NULL);

            //TAC* localVariableInitialization = tacCreate(TAC_MOV, variableName->res, initialValue->res, NULL);
            TAC* localVariableInitialization = tacCreate(TAC_MOV_INIT, astNode->children[0]->hashNode, astNode->children[0]->children[0]->hashNode, NULL);
            //return tacJoin(localVariableInitialization, generateChild[1]);
            return tacJoin(generateChild[1], localVariableInitialization);
        }
			
		case AST_CMD_LIST:
			return tacJoin(generateChild[0], generateChild[1]);

		case AST_ATTRIBUTION:
		{
			//The first argument is the expression to be executed.
			//The second argument is the new node of MOV operation, moving the expression result to the desintation variable.
            return tacJoin(generateChild[1], tacJoin(generateChild[0], tacCreate(TAC_MOV, generateChild[0]->res, generateChild[1]->res, NULL)));
		}
			
		case AST_INPUT_CMD:
			return tacCreate(TAC_READ, astNode->hashNode, NULL, NULL);

		case AST_OUTPUT_CMD:
			return generateChild[0];

		case AST_OUTPUT_LIST:
		{
			//The first argument in the first tacJoin can be an expression or a dummie tac_symbol.
            TAC* newPrintCmd = tacJoin(generateChild[0], tacCreate(TAC_PRINT, NULL, generateChild[0]->res, NULL));
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
            return generateLoop(generateChild[0], generateChild[1]);
        default:
            return NULL;

        
		//[?]case AST_GLOBAL_VAR_LIST:
		//[?]case AST_GLOBAL_VECTOR:
	}
}

TAC * generateLoop(TAC* booleanExpression, TAC* code)
{
    HASH_NODE * beginLoop = NULL;
    HASH_NODE * endLoop = NULL;
    TAC * exprEvalLoop = NULL;
    TAC * returnToEvalLoop = NULL;
    TAC * beginLoopPointer = NULL;
    TAC * endLoopPointer = NULL;

    // Create the label where the loop start
    beginLoop = makeLabel();
    beginLoopPointer = tacCreate(TAC_LABEL, beginLoop, NULL, NULL);

    // Create the label to the next instruction after the loop
    endLoop = makeLabel();
    endLoopPointer = tacCreate(TAC_LABEL, endLoop, NULL, NULL);

    // Evaluate the expression. If the expression is false, finish the loop
    exprEvalLoop = tacCreate(TAC_JZ, endLoop, booleanExpression ? booleanExpression->res : NULL, NULL);

    // Jump back to the evaluation step
    returnToEvalLoop = tacCreate(TAC_JUMP, beginLoop, NULL, NULL);

    return tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(beginLoopPointer, booleanExpression), exprEvalLoop), code), returnToEvalLoop), endLoopPointer);
}

TAC * generateIfThen(TAC* booleanExpression, TAC* code)
{
    HASH_NODE * endIfLabel = NULL;
    TAC * exprEvalJump = NULL;
    TAC * endIfPointer = NULL;

    // Create the jump label that points to the first command after the end of IF COMMAND BLOCK
    endIfLabel = makeLabel();
    endIfPointer = tacCreate(TAC_LABEL, endIfLabel, NULL, NULL);

    // Create a node to eval the boolean expression. If the expression is false, do not execute the IF code by
    // jumping to endIfLabel
    exprEvalJump = tacCreate(TAC_JZ, endIfLabel, booleanExpression ? booleanExpression->res : NULL, NULL);

    // Compute the expression, evaluate it and, if true, execute the "code"
    return tacJoin(tacJoin(tacJoin(booleanExpression, exprEvalJump), code), endIfPointer);
}

TAC * generateIfThenElse(TAC* booleanExpression, TAC* codeTrue, TAC* codeFalse)
{
    HASH_NODE * elseStartLabel = NULL;
    HASH_NODE * endIfLabel = NULL;
    TAC * exprEvalJump = NULL;
    TAC * jumpBeforeElse = NULL;
    TAC * endIfPointer = NULL;
    TAC * elseStartPointer = NULL;
    TAC * ifTrueCode = NULL;
    TAC * ifFalseCode = NULL;

    // Create the jump label that points to the first command after the end of IF COMMAND BLOCK
    endIfLabel = makeLabel();
    endIfPointer = tacCreate(TAC_LABEL, endIfLabel, NULL, NULL);

    // Create the label to where it should jump if the expression is false
    elseStartLabel = makeLabel();
    elseStartPointer = tacCreate(TAC_LABEL, elseStartLabel, NULL, NULL);

    // Create a node to eval the boolean expression. If the expression is false, jump to the ELSE code
    exprEvalJump = tacCreate(TAC_JZ, elseStartLabel, booleanExpression ? booleanExpression->res : NULL, NULL);
    ifFalseCode = tacJoin(elseStartPointer, codeFalse);

    // If the expression is true, it shall jump unconditionally to the end of the if command and DO NOT EXECUTE the else code
    jumpBeforeElse = tacCreate(TAC_JUMP, endIfLabel, NULL, NULL);
    ifTrueCode = tacJoin(codeTrue, jumpBeforeElse);

    // Compute the expression, evaluate it and execute the code accordingly
    return tacJoin(tacJoin(tacJoin(tacJoin(booleanExpression, exprEvalJump), ifTrueCode), ifFalseCode), endIfPointer);
}

TAC * generateBinaryOperation(int tacType, TAC* child0, TAC* child1)
{
    //Get the highest Datatype of the expression
    int resultDataType = checkDataTypeCompatibility(child0->res->dataType, child1->res->dataType);
	TAC* binaryOperationTac =  tacCreate(tacType ,makeTemp(resultDataType), child0->res, child1->res);
	return tacJoin(child0, tacJoin(child1, binaryOperationTac));
}
