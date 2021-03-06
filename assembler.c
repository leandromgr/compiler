#include "assembler.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>  

#include "ast.h"
#include "tac.h"
#include "semantic.h"
#include "y.tab.h"

int stackCounter;
int paramCounter;
int localVariableCounter;
int stackIndex;
//int offsetFunctionCall;
int logicOpLabelCounter;

FILE* DEST_ASM;

void avengersAssemble(AST_NODE* astTree, TAC* tacList)
{
    DEST_ASM = fopen("assembly.s", "w");
	printHeaders();
	parseGlobalVariables(astTree);
	parseStrings();
    fprintf(DEST_ASM, "\t.text\n");
	parseTAC(tacList);
	printFooter();
    fclose(DEST_ASM);
}

void printHeaders()
{
    fprintf(DEST_ASM, "\t.file\t\"test1.txt\"\n");
}

void printFooter()
{
    fprintf(DEST_ASM, ".LFE0:\n");
    fprintf(DEST_ASM, "\t.size\tmain, .-main\n");
    fprintf(DEST_ASM, "\t.ident\t\"GCC: (Ubuntu 4.8.4-2ubuntu1~14.04) 4.8.4\"\n");
    fprintf(DEST_ASM, "\t.section\t.note.GNU-stack,\"\",@progbits\n");
}

void parseStrings()
{
    int indexHash;
	int stringCounter = 0;
	HASH_NODE *currentNode;

    fprintf(DEST_ASM, "\t.section\t.rodata\n");
    fprintf(DEST_ASM, ".LC0:\n");
    fprintf(DEST_ASM, "\t.string \"%%d\"\n");
    fprintf(DEST_ASM, ".LC1:\n");
    fprintf(DEST_ASM, "\t.string \"%%f\"\n");
    fprintf(DEST_ASM, ".LC2:\n");
    fprintf(DEST_ASM, "\t.string \"%%c\"\n");

	for(indexHash=0 ; indexHash<HASH_SIZE; indexHash++)
	{
		for(currentNode=symbolTable[indexHash] ; currentNode ; currentNode = currentNode->nextNode)
		{
			if(currentNode->symbolType == LIT_STRING)
			{
                fprintf(DEST_ASM, ".LC%d:\n", stringCounter+3);
                fprintf(DEST_ASM, "\t.string \"%s\"\n", currentNode->symbol);
				stringTable[stringCounter] = currentNode;

				stringCounter++;
			}
		}
	}
}

void parseGlobalVariables(AST_NODE *astTree)
{
	AST_NODE *currentAstNode;

	if(!astTree)
		return;

	currentAstNode = astTree;
	while((currentAstNode!= NULL) && (currentAstNode->type != AST_GLOBAL_VAR_LIST))
	{
		currentAstNode = currentAstNode->children[1];
	}


	while((currentAstNode!= NULL) && (currentAstNode->type == AST_GLOBAL_VAR_LIST))
	{
		//Passing the global variable node as argument.
		printGlobalVariableASM(currentAstNode->children[0]);
		currentAstNode = currentAstNode->children[1];
	}
}

void printGlobalVariableASM(AST_NODE *variableNode)
{

    fprintf(DEST_ASM, "\t.globl %s\n", variableNode->hashNode->symbol);
    fprintf(DEST_ASM, "\t.type %s, @object\n", variableNode->hashNode->symbol);

	AST_NODE *valueOrVector = variableNode->children[0];
	if(valueOrVector->type == AST_GLOBAL_VECTOR)
	{
		int vectorSize = atoi(valueOrVector->hashNode->symbol);
		int indexInitialValue;
		AST_NODE *currentInitialValue = valueOrVector->children[0];
		switch(variableNode->type)
		{
			case AST_INT:
                fprintf(DEST_ASM, "\t.size %s, %d\n",variableNode->hashNode->symbol, 4*vectorSize);
                fprintf(DEST_ASM, "%s:\n", variableNode->hashNode->symbol);
				for(indexInitialValue=0; indexInitialValue<vectorSize ; indexInitialValue++)
				{
					if(currentInitialValue)
					{
                        fprintf(DEST_ASM, "\t.long %s\n", currentInitialValue->hashNode->symbol);
						currentInitialValue = currentInitialValue->children[0];
					}
					else
					{
                        fprintf(DEST_ASM, "\t.long 0\n");
					}
				}
				break;
			
			case AST_CHAR:
                fprintf(DEST_ASM, "\t.size %s, %d\n", variableNode->hashNode->symbol, 1*vectorSize);
                fprintf(DEST_ASM, "%s:\n", variableNode->hashNode->symbol);
				for(indexInitialValue=0; indexInitialValue<vectorSize ; indexInitialValue++)
				{
					if(currentInitialValue)
					{
                        fprintf(DEST_ASM, "\t.byte %i\n", currentInitialValue->hashNode->symbol[0]);
						currentInitialValue = currentInitialValue->children[0];
					}
					else
					{
                        fprintf(DEST_ASM, "\t.byte 0\n");
					}
				}
				break;

			case AST_BOOL:
                fprintf(DEST_ASM, "\t.size %s, %d\n", variableNode->hashNode->symbol, 1*vectorSize);
                fprintf(DEST_ASM, "%s:\n", variableNode->hashNode->symbol);

				while(currentInitialValue)
				{
					if(currentInitialValue->hashNode->symbolType == LIT_TRUE)
                        fprintf(DEST_ASM, "\t.byte 1\n");
					else
                        fprintf(DEST_ASM, "\t.byte 0\n");
	
					currentInitialValue = currentInitialValue->children[0];
				}
				for(indexInitialValue=0; indexInitialValue<vectorSize ; indexInitialValue++)
				{
					if(currentInitialValue)
					{
						if(currentInitialValue->hashNode->symbolType == LIT_TRUE)
                            fprintf(DEST_ASM, "\t.byte 1\n");
						else
                            fprintf(DEST_ASM, "\t.byte 0\n");
	
						currentInitialValue = currentInitialValue->children[0];
					}
					else
					{
                        fprintf(DEST_ASM, "\t.byte 0\n");
					}
				}
				break;

			case AST_REAL:
                fprintf(DEST_ASM, "\t.size %s, %d\n", variableNode->hashNode->symbol, 4*vectorSize);
                fprintf(DEST_ASM, "%s:\n", variableNode->hashNode->symbol);
				for(indexInitialValue=0; indexInitialValue<vectorSize ; indexInitialValue++)
				{
					if(currentInitialValue)
					{
                        fprintf(DEST_ASM, "\t.single %s\n", currentInitialValue->hashNode->symbol);
						currentInitialValue = currentInitialValue->children[0];
					}
					else
					{
                        fprintf(DEST_ASM, "\t.single 0\n");
					}
				}
				break;
			default:
                fprintf(DEST_ASM, "ERROR: Varible type not valid!");
				exit(6);
		}
	}
	else //it is a simple global variable
	{
		switch(variableNode->type)
		{
			case AST_INT:
                fprintf(DEST_ASM, "\t.size %s, 4\n", variableNode->hashNode->symbol);
                fprintf(DEST_ASM, "%s:\n", variableNode->hashNode->symbol);
                fprintf(DEST_ASM, "\t.long %s\n", valueOrVector->hashNode->symbol);
				break;
			case AST_CHAR:
                fprintf(DEST_ASM, "\t.size %s, 1\n", variableNode->hashNode->symbol);
                fprintf(DEST_ASM, "%s:\n", variableNode->hashNode->symbol);
                fprintf(DEST_ASM, "\t.byte %i\n", valueOrVector->hashNode->symbol[0]);
				break;
			case AST_BOOL:
                fprintf(DEST_ASM, "\t.size %s, 1\n", variableNode->hashNode->symbol);
                fprintf(DEST_ASM, "%s:\n", variableNode->hashNode->symbol);
				if(valueOrVector->hashNode->symbolType == LIT_TRUE)
                    fprintf(DEST_ASM, "\t.byte 1\n");
				else
                    fprintf(DEST_ASM, "\t.byte 0\n");
					
				break;

			case AST_REAL:
                fprintf(DEST_ASM, "\t.size %s, 4\n", variableNode->hashNode->symbol);
                fprintf(DEST_ASM, "%s:\n", variableNode->hashNode->symbol);
                fprintf(DEST_ASM, "\t.single %s\n", valueOrVector->hashNode->symbol);
				break;
			default:
                fprintf(DEST_ASM, "ERROR: Varible type not valid!");
				exit(6);
		}
	}

	
}

void parseTAC(TAC* tacList)
{
    TAC* currentTAC;
	int i;
    int argumentIndex = 0;
    //int offsetFunctionCall = 0;

	//TAC_SUM Variables		
	int op1StackIndex;
	int op2StackIndex;
	int resStackIndex;

	if(!tacList)
		return;

	currentTAC = tacList;

	while (currentTAC)
	{
		switch(currentTAC->type)
		{
			case TAC_FUNBEGIN:
                fprintf(DEST_ASM, "\t.globl\t%s\n", currentTAC->op1->symbol);
                fprintf(DEST_ASM, "\t.type\t%s, @function\n", currentTAC->op1->symbol);
                fprintf(DEST_ASM, "%s:\n", currentTAC->op1->symbol);
                fprintf(DEST_ASM, "\tpushq\t%%rbp\n");
                fprintf(DEST_ASM, "\tmovq\t%%rsp, %%rbp\n");
                fprintf(DEST_ASM, "\tsubq\t$16, %%rsp\n"); //GHOST TOSEE
                stackCounter = 0;
                paramCounter = 0;
                localVariableCounter = 0;
                logicOpLabelCounter = 2;
				break;

            case TAC_MOV_INIT:
				for (i = stackCounter; i > (localVariableCounter + 1); i--)
				{
					stackOffsetControl[i] = stackOffsetControl[i-1];
				}
				stackCounter++;

				stackOffsetControl[localVariableCounter] = currentTAC->res;

				int localVarInitalValue;
				switch(currentTAC->op1->symbolType)
				{
					case LIT_INTEGER:
						localVarInitalValue = atoi(currentTAC->op1->symbol);
                        fprintf(DEST_ASM, "\tmovl\t$%i, %i(%%rbp)\n", localVarInitalValue, -4*(localVariableCounter+1));
						break;
					case LIT_CHAR:
						localVarInitalValue =  (int) currentTAC->op1->symbol[0];
                        fprintf(DEST_ASM, "\tmovb\t$%i, %i(%%rbp)\n", localVarInitalValue, -4*(localVariableCounter+1));
						break;
					case LIT_TRUE:
						localVarInitalValue = 1;
                        fprintf(DEST_ASM, "\tmovl\t$%i, %i(%%rbp)\n", localVarInitalValue, -4*(localVariableCounter+1));
						break;
					case LIT_FALSE:
						localVarInitalValue = 0;
                        fprintf(DEST_ASM, "\tmovl\t$%i, %i(%%rbp)\n", localVarInitalValue, -4*(localVariableCounter+1));
						break;
					default:
						localVarInitalValue = 0;
                        fprintf(DEST_ASM, "\tmovl\t$%i, %i(%%rbp)\n", localVarInitalValue, -4*(localVariableCounter+1));
				}
				
				localVariableCounter++;
				break;

			case TAC_FUNEND:
                fprintf(DEST_ASM, "\tleave\n"); //TIRAMOS O POP, GHOST, VER O NOME DA FUNçÂO
                fprintf(DEST_ASM, "\tret\n");
				break;

            case TAC_PARAM:
                for (i = paramCounter ; i > 0; i--)
				{
                    paramOffsetControl[i] = paramOffsetControl[i-1];
				}
                paramOffsetControl[0] = currentTAC->op1;

                paramCounter++;
				break;

			case TAC_PRINT:
				if (currentTAC->op1->symbolType == LIT_STRING)
				{
					for(i=0 ; i<MAX_STRINGS ; i++)
					{
						if(currentTAC->op1 == stringTable[i])
							break;
					}
                    fprintf(DEST_ASM, "\tmovl\t$.LC%d, %%edi\n", i+3);
                    fprintf(DEST_ASM, "\tmovl\t$0, %%eax\n");
                    fprintf(DEST_ASM, "\tcall\tprintf\n");
				}
				else if(currentTAC->op1->symbolType == SYMBOL_GLOBAL_VARIABLE)
				{
					switch(currentTAC->op1->dataType)
					{
						case DATATYPE_INT:
                            fprintf(DEST_ASM, "\tmovl\t%s(%%rip), %%eax\n", currentTAC->op1->symbol);
                            fprintf(DEST_ASM, "\tmovl\t%%eax, %%esi\n");
                            fprintf(DEST_ASM, "\tmovl\t$.LC0, %%edi\n");
                            fprintf(DEST_ASM, "\tmovl\t$0, %%eax\n");
                            fprintf(DEST_ASM, "\tcall\tprintf\n");
							break;

						case DATATYPE_BOOL:
						case DATATYPE_CHAR:
                            fprintf(DEST_ASM, "\tmovl\t%s(%%rip), %%eax\n", currentTAC->op1->symbol);
                            fprintf(DEST_ASM, "\tmovl\t%%eax, %%edi\n");
                            fprintf(DEST_ASM, "\tcall\tputchar\n");
							break;

						case DATATYPE_REAL:
                            fprintf(DEST_ASM, "\tmovss\t%s(%%rip), %%xmm0\n", currentTAC->op1->symbol);
                            fprintf(DEST_ASM, "\tunpcklps\t%%xmm0, %%xmm0\n");
                            fprintf(DEST_ASM, "\tcvtps2pd\t%%xmm0, %%xmm0\n");
                            fprintf(DEST_ASM, "\tmovl\t$.LC1, %%edi\n");
                            fprintf(DEST_ASM, "\tmovl\t$1, %%eax\n");
                            fprintf(DEST_ASM, "\tcall\tprintf\n");
							break;
					}
				}	
				else if(currentTAC->op1->symbolType == SYMBOL_GLOBAL_VECTOR)
				{
					switch(currentTAC->op1->dataType)
					{
						case DATATYPE_INT:
                            fprintf(DEST_ASM, "\tmovl\t%s+%i(%%rip), %%eax\n", currentTAC->op1->symbol, 4*atoi(currentTAC->prev->op1->symbol));
                            fprintf(DEST_ASM, "\tmovl\t%%eax, %%esi\n");
                            fprintf(DEST_ASM, "\tmovl\t$.LC0, %%edi\n");
                            fprintf(DEST_ASM, "\tmovl\t$0, %%eax\n");
                            fprintf(DEST_ASM, "\tcall\tprintf\n");
							break;

						case DATATYPE_BOOL:
						case DATATYPE_CHAR:
                            fprintf(DEST_ASM, "\tmovzbl\t%s+%i(%%rip), %%eax\n", currentTAC->op1->symbol, atoi(currentTAC->prev->op1->symbol));
                            fprintf(DEST_ASM, "\tmovsbl\t%%al, %%eax\n");
                            fprintf(DEST_ASM, "\tmovl\t%%eax, %%edi\n");
                            fprintf(DEST_ASM, "\tcall\tputchar\n");
							break;

						case DATATYPE_REAL:
                            fprintf(DEST_ASM, "\tmovss\t%s+%i(%%rip), %%xmm0\n", currentTAC->op1->symbol, 4*atoi(currentTAC->prev->op1->symbol));
                            fprintf(DEST_ASM, "\tunpcklps\t%%xmm0, %%xmm0\n");
                            fprintf(DEST_ASM, "\tcvtps2pd\t%%xmm0, %%xmm0\n");
                            fprintf(DEST_ASM, "\tmovl\t$.LC1, %%edi\n");
                            fprintf(DEST_ASM, "\tmovl\t$1, %%eax\n");
                            fprintf(DEST_ASM, "\tcall\tprintf\n");
							break;
					}
				}
				else if(currentTAC->op1->symbolType == LIT_INTEGER)
				{
							
                    fprintf(DEST_ASM, "\tmovl\t$%i, %%eax\n", atoi(currentTAC->op1->symbol));
                    fprintf(DEST_ASM, "\tmovl\t%%eax, %%esi\n");
                    fprintf(DEST_ASM, "\tmovl\t$.LC0, %%edi\n");
                    fprintf(DEST_ASM, "\tmovl\t$0, %%eax\n");
                    fprintf(DEST_ASM, "\tcall\tprintf\n");
					break;
				}
				else if (currentTAC->op1->symbolType == LIT_CHAR)
				{
                    fprintf(DEST_ASM, "\tmovl\t$%i, %%eax\n", currentTAC->op1->symbol[0]);
                    fprintf(DEST_ASM, "\tmovl\t%%eax, %%edi\n");
                    fprintf(DEST_ASM, "\tcall\tputchar\n");
					break;
				}
				else if (currentTAC->op1->symbolType == LIT_TRUE)
				{
                    fprintf(DEST_ASM, "\tmovl\t$%i, %%eax\n", 1);
                    fprintf(DEST_ASM, "\tmovl\t%%eax, %%esi\n");
                    fprintf(DEST_ASM, "\tmovl\t$.LC0, %%edi\n");
                    fprintf(DEST_ASM, "\tmovl\t$0, %%eax\n");
                    fprintf(DEST_ASM, "\tcall\tprintf\n");
					break;
				}
				else if (currentTAC->op1->symbolType == LIT_FALSE)
				{
                    fprintf(DEST_ASM, "\tmovl\t$%i, %%eax\n", 0);
                    fprintf(DEST_ASM, "\tmovl\t%%eax, %%esi\n");
                    fprintf(DEST_ASM, "\tmovl\t$.LC0, %%edi\n");
                    fprintf(DEST_ASM, "\tmovl\t$0, %%eax\n");
                    fprintf(DEST_ASM, "\tcall\tprintf\n");
					break;
				}
                else if (currentTAC->op1->symbolType == SYMBOL_FUNCTION_PARAMETER)
                {
                    argumentIndex = getParamStackIndex(currentTAC->op1);
                    switch(currentTAC->op1->dataType)
                    {
                        case DATATYPE_INT:
                        case DATATYPE_BOOL:
                            fprintf(DEST_ASM, "\tmovl\t%i(%%rbp), %%eax\n", argumentIndex * 8 + 16);
                            fprintf(DEST_ASM, "\tmovl\t%%eax, %%esi\n");
                            fprintf(DEST_ASM, "\tmovl\t$.LC0, %%edi\n");
                            fprintf(DEST_ASM, "\tmovl\t$0, %%eax\n");
                            fprintf(DEST_ASM, "\tcall\tprintf\n");
                            break;


                        case DATATYPE_CHAR:
                            fprintf(DEST_ASM, "\tmovsbl\t%i(%%rbp), %%eax\n", argumentIndex * 8 + 16);
                            fprintf(DEST_ASM, "\tmovl\t%%eax, %%esi\n");
                            fprintf(DEST_ASM, "\tmovl\t$.LC2, %%edi\n");
                            fprintf(DEST_ASM, "\tmovl\t$0, %%eax\n");
                            fprintf(DEST_ASM, "\tcall\tprintf\n");
                            break;

                        case DATATYPE_REAL:
                            fprintf(DEST_ASM, "\tmovss\t%i(%%rbp), %%xmm0\n", argumentIndex * 8 + 16);
                            fprintf(DEST_ASM, "\tunpcklps\t%%xmm0, %%xmm0\n");
                            fprintf(DEST_ASM, "\tcvtps2pd\t%%xmm0, %%xmm0\n");
                            fprintf(DEST_ASM, "\tmovl\t$.LC1, %%edi\n");
                            fprintf(DEST_ASM, "\tmovl\t$1, %%eax\n");
                            fprintf(DEST_ASM, "\tcall\tprintf\n");
                            break;
                    }
                }
				else if((currentTAC->op1->symbolType == SYMBOL_LOCAL_VARIABLE) ||
                        (currentTAC->op1->symbolType == TK_IDENTIFIER))
				{
                    stackIndex = getDataStackIndex(currentTAC->op1);
					switch(currentTAC->op1->dataType)
					{
						case DATATYPE_INT:
						case DATATYPE_BOOL:
                            fprintf(DEST_ASM, "\tmovl\t%i(%%rbp), %%eax\n", (stackIndex+1) * -4);
                            fprintf(DEST_ASM, "\tmovl\t%%eax, %%esi\n");
                            fprintf(DEST_ASM, "\tmovl\t$.LC0, %%edi\n");
                            fprintf(DEST_ASM, "\tmovl\t$0, %%eax\n");
                            fprintf(DEST_ASM, "\tcall\tprintf\n");
							break;


						case DATATYPE_CHAR:
                            fprintf(DEST_ASM, "\tmovsbl\t%i(%%rbp), %%eax\n", (stackIndex+1) * -4);
                            fprintf(DEST_ASM, "\tmovl\t%%eax, %%esi\n");
                            fprintf(DEST_ASM, "\tmovl\t$.LC2, %%edi\n");
                            fprintf(DEST_ASM, "\tmovl\t$0, %%eax\n");
                            fprintf(DEST_ASM, "\tcall\tprintf\n");
							break;

						case DATATYPE_REAL:
                            fprintf(DEST_ASM, "\tmovss\t%i(%%rbp), %%xmm0\n", (stackIndex+1) * -4);
                            fprintf(DEST_ASM, "\tunpcklps\t%%xmm0, %%xmm0\n");
                            fprintf(DEST_ASM, "\tcvtps2pd\t%%xmm0, %%xmm0\n");
                            fprintf(DEST_ASM, "\tmovl\t$.LC1, %%edi\n");
                            fprintf(DEST_ASM, "\tmovl\t$1, %%eax\n");
                            fprintf(DEST_ASM, "\tcall\tprintf\n");
							break;
					}
				}	 
				break;

            case TAC_JZ:
                switch (currentTAC->op1->symbolType)
                {
                    case TK_IDENTIFIER:
                    case SYMBOL_LOCAL_VARIABLE:
                        stackIndex = getDataStackIndex(currentTAC->op1);
                        fprintf(DEST_ASM, "\tcmpl\t$1, %i(%%rbp)\n", (stackIndex+1) * -4);
                        break;
                    case SYMBOL_FUNCTION_PARAMETER:
                        argumentIndex = getParamStackIndex(currentTAC->op1);
                        fprintf(DEST_ASM, "\tcmpl\t$1, %i(%%rbp)\n", argumentIndex*8 + 16);
                        break;
                    case SYMBOL_GLOBAL_VECTOR:
                        fprintf(DEST_ASM, "\tcmpl\t$1, %s+%i(%%rip)\n", currentTAC->op1->symbol, 4*atoi(currentTAC->prev->op1->symbol));
                        break;
                    case SYMBOL_GLOBAL_VARIABLE:
                        fprintf(DEST_ASM, "\tcmpl\t$1, %s(%%rip)\n", currentTAC->op1->symbol);
                        break;
                    case LIT_INTEGER:
                        fprintf(DEST_ASM, "\tcmpl\t$1, %i\n", atoi(currentTAC->op1->symbol));
                        break;
                    case LIT_CHAR:
                        fprintf(DEST_ASM, "\tcmpl\t$1, %i\n", currentTAC->op1->symbol[0]);
                        break;
                    case LIT_TRUE:
                        fprintf(DEST_ASM, "\tcmpl\t$1, $1\n");
                        break;
                    case LIT_FALSE:
                        fprintf(DEST_ASM, "\tcmpl\t$1, $0\n");
                        break;
                    default:
                        break;
                }

                fprintf(DEST_ASM, "\tjne\t.%s\n", currentTAC->res->symbol);
                break;

            case TAC_JUMP:
                fprintf(DEST_ASM, "\tjmp\t.%s\n", currentTAC->res->symbol);
                break;

            case TAC_LABEL:
                fprintf(DEST_ASM, ".%s:\n", currentTAC->res->symbol);
                break;

            case TAC_RETURN:
                switch (currentTAC->op1->symbolType)
                {
                    case TK_IDENTIFIER:
                    case SYMBOL_LOCAL_VARIABLE:
                        stackIndex = getDataStackIndex(currentTAC->op1);
                        fprintf(DEST_ASM, "\tmovl\t%i(%%rbp), %%eax\n", (stackIndex+1) * -4);
                        break;
                    case SYMBOL_FUNCTION_PARAMETER:
                        argumentIndex = getParamStackIndex(currentTAC->op1);
                        fprintf(DEST_ASM, "\tmovl\t%i(%%rbp), %%eax\n", argumentIndex*8 + 16);
                        break;
                    case SYMBOL_GLOBAL_VECTOR:
                        fprintf(DEST_ASM, "\tmovl\t%s+%i(%%rip), %%eax\n", currentTAC->op1->symbol, 4*atoi(currentTAC->prev->op1->symbol));
                        break;
                    case SYMBOL_GLOBAL_VARIABLE:
                        fprintf(DEST_ASM, "\tmovl\t%s(%%rip), %%eax\n", currentTAC->op1->symbol);
                        break;
                    case LIT_INTEGER:
                        fprintf(DEST_ASM, "\tmovl\t$%i, %%eax\n", atoi(currentTAC->op1->symbol));
                        break;
                    case LIT_CHAR:
                        fprintf(DEST_ASM, "\tmovl\t$%i, %%eax\n", currentTAC->op1->symbol[0]);
                        break;
                    case LIT_TRUE:
                        fprintf(DEST_ASM, "\tmovl\t$1, %%eax\n");
                        break;
                    case LIT_FALSE:
                        fprintf(DEST_ASM, "\tmovl\t$0, %%eax\n");
                        break;
                    default:
                        break;
                }
                break;
            case TAC_MOV:
                switch (currentTAC->op1->symbolType)
                {
                    case TK_IDENTIFIER:
                    case SYMBOL_LOCAL_VARIABLE:
                        stackIndex = getDataStackIndex(currentTAC->op1);
                        fprintf(DEST_ASM, "\tmovl\t%i(%%rbp), %%eax\n", (stackIndex+1) * -4);
                        break;
                    case SYMBOL_FUNCTION_PARAMETER:
                        argumentIndex = getParamStackIndex(currentTAC->op1);
                        fprintf(DEST_ASM, "\tmovl\t%i(%%rbp), %%eax\n", argumentIndex*8 + 16);
                        break;
                    case SYMBOL_GLOBAL_VECTOR:
                        fprintf(DEST_ASM, "\tmovl\t%s+%i(%%rip), %%eax\n", currentTAC->op1->symbol, 4*atoi(currentTAC->prev->op1->symbol));
                        break;
                    case SYMBOL_GLOBAL_VARIABLE:
                        fprintf(DEST_ASM, "\tmovl\t%s(%%rip), %%eax\n", currentTAC->op1->symbol);
                        break;
                    case LIT_INTEGER:
                        fprintf(DEST_ASM, "\tmovl\t$%i, %%eax\n", atoi(currentTAC->op1->symbol));
                        break;
                    case LIT_CHAR:
                        fprintf(DEST_ASM, "\tmovl\t$%i, %%eax\n", currentTAC->op1->symbol[0]);
                        break;
                    case LIT_TRUE:
                        fprintf(DEST_ASM, "\tmovl\t$1, %%eax\n");
                        break;
                    case LIT_FALSE:
                        fprintf(DEST_ASM, "\tmovl\t$0, %%eax\n");
                        break;
                    default:
                        break;
                }
                switch (currentTAC->res->symbolType)
                {
                    case TK_IDENTIFIER:
                    case SYMBOL_LOCAL_VARIABLE:
                        stackIndex = getDataStackIndex(currentTAC->res);
                        fprintf(DEST_ASM, "\tmovl\t%%eax, %i(%%rbp)\n", (stackIndex+1) * -4);
                        break;
                    /*
                    case SYMBOL_FUNCTION_PARAMETER:
                        argumentIndex = getDataStackIndex(currentTAC->res);
                        fprintf(DEST_ASM, "\tmovl\t%i(%%rbp), %i(%%rsp)\n", (stackIndex+1) * -4, 8*argumentIndex);
                        argumentIndex++;*/
                    case SYMBOL_GLOBAL_VECTOR:
                        fprintf(DEST_ASM, "\tmovl\t%%eax, %s+%i(%%rip)\n", currentTAC->res->symbol, 4*atoi(currentTAC->prev->res->symbol));
                        break;
                    case SYMBOL_GLOBAL_VARIABLE:
                        fprintf(DEST_ASM, "\tmovl\t%%eax, %s(%%rip)", currentTAC->res->symbol);
                        break;
                    default:
                        break;
                }
                break;
            case TAC_ARG:
                switch (currentTAC->res->symbolType)
                {
                    case TK_IDENTIFIER:
                    case SYMBOL_LOCAL_VARIABLE:
                        stackIndex = getDataStackIndex(currentTAC->res);
                        fprintf(DEST_ASM, "\tmovl\t%i(%%rbp), %i(%%rsp)\n", (stackIndex+1) * -4, 8*argumentIndex);
                        argumentIndex++;
                        break;
                    /*
                    case SYMBOL_FUNCTION_PARAMETER:
                        argumentIndex = getDataStackIndex(currentTAC->res);
                        fprintf(DEST_ASM, "\tmovl\t%i(%%rbp), %i(%%rsp)\n", (stackIndex+1) * -4, 8*argumentIndex);
                        argumentIndex++;*/
                    case SYMBOL_GLOBAL_VECTOR:
                        fprintf(DEST_ASM, "\tmovl\t%s+%i(%%rip), %i(%%rsp)\n", currentTAC->res->symbol, 4*atoi(currentTAC->prev->res->symbol), 8*argumentIndex);
                        argumentIndex++;
                        break;
                    case SYMBOL_GLOBAL_VARIABLE:
                        fprintf(DEST_ASM, "\tmovl\t%s(%%rip), %i(%%rsp)\n", currentTAC->res->symbol, 8*argumentIndex);
                        argumentIndex++;
                        break;
                    case LIT_INTEGER:
                        fprintf(DEST_ASM, "\tmovl\t$%i, %i(%%rsp)\n", atoi(currentTAC->res->symbol), 8*argumentIndex);
                        argumentIndex++;
                        break;
                    case LIT_CHAR:
                        fprintf(DEST_ASM, "\tmovl\t$%i, %i(%%rsp)\n", currentTAC->res->symbol[0], 8*argumentIndex);
                        argumentIndex++;
                        break;
                    case LIT_TRUE:
                        fprintf(DEST_ASM, "\tmovl\t$1, %i(%%rsp)\n", 8*argumentIndex);
                        argumentIndex++;
                        break;
                    case LIT_FALSE:
                        fprintf(DEST_ASM, "\tmovl\t$0, %i(%%rsp)\n", 8*argumentIndex);
                        argumentIndex++;
                        break;
                    default:
                        break;
                }
                break;

            case TAC_CALL:
                fprintf(DEST_ASM, "\tcall\t%s\n", currentTAC->op1->symbol);
                stackIndex = getDataStackIndex(currentTAC->res);
                fprintf(DEST_ASM, "\tmovl\t%%eax, %i(%%rbp)\n", (stackIndex+1) * -4);
                argumentIndex = 0;
                break;

            case TAC_SUM:
            case TAC_SUB:
            case TAC_MULT:
            case TAC_DIV:
            case TAC_EQ:
            case TAC_LT: 		
			case TAC_GT: 
			case TAC_LET:
			case TAC_GET:
			case TAC_NE:
            	op1StackIndex = -4 * (getDataStackIndex(currentTAC->op1) +1);
				op2StackIndex = -4 * (getDataStackIndex(currentTAC->op2) +1);
				resStackIndex = -4 * (getDataStackIndex(currentTAC->res) +1);

            	switch(currentTAC->op2->dataType)
            	{
            		case DATATYPE_INT:
                        if (currentTAC->op2->symbolType == LIT_INTEGER)
                        {
                            if(currentTAC->type == TAC_DIV)
                                fprintf(DEST_ASM, "\tmovl\t$%i, %%ecx\n", atoi(currentTAC->op2->symbol));
                            else
                                fprintf(DEST_ASM, "\tmovl\t$%i, %%eax\n", atoi(currentTAC->op2->symbol));
                        }
                        else
                        {
                            if(currentTAC->type == TAC_DIV)
                                fprintf(DEST_ASM, "\tmovl\t%d(%%rbp), %%ecx\n", op2StackIndex);
                            else
                                fprintf(DEST_ASM, "\tmovl\t%d(%%rbp), %%eax\n", op2StackIndex);
                        }
                        break;
            		case DATATYPE_CHAR:
            		case DATATYPE_BOOL:
                        if ((currentTAC->op2->symbolType == LIT_CHAR) ||
                            (currentTAC->op2->symbolType == LIT_TRUE) ||
                            (currentTAC->op2->symbolType == LIT_FALSE) )
                        {
                            if(currentTAC->type == TAC_DIV)
                                fprintf(DEST_ASM, "\tmovl\t$%i, %%ecx\n", currentTAC->op2->symbol[0]);
                            else
                                fprintf(DEST_ASM, "\tmovl\t$%i, %%eax\n", currentTAC->op2->symbol[0]);
                        }
                        else
                        {
                            if(currentTAC->type == TAC_DIV)
                                fprintf(DEST_ASM, "\tmovzbl\t%d(%%rbp), %%ecx\n", op2StackIndex);
                            else
                                fprintf(DEST_ASM, "\tmovzbl\t%d(%%rbp), %%eax\n", op2StackIndex);
                        }
                        break;
            		case DATATYPE_REAL:
            			//TODO
            		default:
                        break;
            	}
            	switch(currentTAC->op1->dataType)
            	{
            		case DATATYPE_INT:
            			if (currentTAC->op1->symbolType == LIT_INTEGER)
            			{
            				if(currentTAC->type == TAC_DIV)
            					fprintf(DEST_ASM, "\tmovl\t$%i, %%eax\n", atoi(currentTAC->op1->symbol));
	        				else
            					fprintf(DEST_ASM, "\tmovl\t$%i, %%edx\n", atoi(currentTAC->op1->symbol));
            			}
            			else
            			{
	            			if(currentTAC->type == TAC_DIV)
	            				fprintf(DEST_ASM, "\tmovl\t%d(%%rbp), %%eax\n", op1StackIndex);
	        				else
	            				fprintf(DEST_ASM, "\tmovl\t%d(%%rbp), %%edx\n", op1StackIndex);
	            		}
                        break;
                    case DATATYPE_CHAR:
            		case DATATYPE_BOOL:
	            		if ((currentTAC->op1->symbolType == LIT_CHAR) ||
	            			(currentTAC->op1->symbolType == LIT_TRUE) ||
	            			(currentTAC->op1->symbolType == LIT_FALSE) )
	            		{
	            			if(currentTAC->type == TAC_DIV)
	            				fprintf(DEST_ASM, "\tmovl\t$%i, %%eax\n", currentTAC->op1->symbol[0]);
	        				else
	            				fprintf(DEST_ASM, "\tmovl\t$%i, %%edx\n", currentTAC->op1->symbol[0]);
	            		}
	            		else
	            		{
	            			if(currentTAC->type == TAC_DIV)
	            				fprintf(DEST_ASM, "\tmovzbl\t%d(%%rbp), %%eax\n", op1StackIndex);
	        				else
	            				fprintf(DEST_ASM, "\tmovzbl\t%d(%%rbp), %%edx\n", op1StackIndex);
	            		}
                        break;
            		case DATATYPE_REAL:
            			//TODO
            		default:
                        break;
            	}
            	switch(currentTAC->type)
            	{
            		case TAC_SUM:
            			fprintf(DEST_ASM, "\taddl\t%%edx, %%eax\n");
            			break;

            		case TAC_SUB:
            			fprintf(DEST_ASM, "\tsubl\t%%eax, %%edx\n");
            			fprintf(DEST_ASM, "\tmovl\t%%edx, %%eax\n");
            			break;

            		case TAC_MULT:
            			fprintf(DEST_ASM, "\timull\t%%edx, %%eax\n");
            			break;

            		case TAC_DIV:
						fprintf(DEST_ASM, "\tcltd\n");
						fprintf(DEST_ASM, "\tidivl\t%%ecx\n");
						break;

					case TAC_EQ:
						fprintf(DEST_ASM, "\tcmpl\t%%edx, %%eax\n");
						fprintf(DEST_ASM, "\tsete\t%%al\n");
						fprintf(DEST_ASM, "\tmovzbl\t%%al, %%eax\n");
						break;

            		case TAC_LT: 	
						fprintf(DEST_ASM, "\tcmpl\t%%eax, %%edx\n");
						fprintf(DEST_ASM, "\tsetl\t%%al\n");
						fprintf(DEST_ASM, "\tmovzbl\t%%al, %%eax\n");
						break;

					case TAC_GT:
						fprintf(DEST_ASM, "\tcmpl\t%%eax, %%edx\n");
						fprintf(DEST_ASM, "\tsetg\t%%al\n");
						fprintf(DEST_ASM, "\tmovzbl\t%%al, %%eax\n");
						break;
						
					case TAC_LET:
						fprintf(DEST_ASM, "\tcmpl\t%%eax, %%edx\n");
						fprintf(DEST_ASM, "\tsetle\t%%al\n");
						fprintf(DEST_ASM, "\tmovzbl\t%%al, %%eax\n");
						break;

					case TAC_GET: 
						fprintf(DEST_ASM, "\tcmpl\t%%eax, %%edx\n");
						fprintf(DEST_ASM, "\tsetge\t%%al\n");
						fprintf(DEST_ASM, "\tmovzbl\t%%al, %%eax\n");
						break;

					case TAC_NE: 
						fprintf(DEST_ASM, "\tcmpl\t%%eax, %%edx\n");
						fprintf(DEST_ASM, "\tsetne\t%%al\n");
						fprintf(DEST_ASM, "\tmovzbl\t%%al, %%eax\n");
						break;

            		default:
            			break;
            	}
            	if(currentTAC->res->symbolType == TK_IDENTIFIER)
            	{	
            		//Hyp: There is space allocated for this in the stack
            		resStackIndex = -4*(1+insertTempInStack(currentTAC->res));
            	}
            	switch(currentTAC->res->dataType)
            	{
            		case DATATYPE_INT:
                        fprintf(DEST_ASM, "\tmovl\t%%eax, %d(%%rbp)\n", resStackIndex);
                        break;
            		case DATATYPE_CHAR:
            		case DATATYPE_BOOL:
                        fprintf(DEST_ASM, "\tmovb\t%%al, %d(%%rbp)\n", resStackIndex);
                        break;
            		case DATATYPE_REAL:
            			//TODO
                        break;
            		default:
            		break;
            	}
                break;
            case TAC_AND:
            	op1StackIndex = -4 * (getDataStackIndex(currentTAC->op1) +1);
				op2StackIndex = -4 * (getDataStackIndex(currentTAC->op2) +1);
				resStackIndex = -4 * (getDataStackIndex(currentTAC->res) +1);

            	fprintf(DEST_ASM, "\tcmpl\t$0, %d(%%rbp)\n", op1StackIndex);
				fprintf(DEST_ASM, "\tje	.L%d\n", logicOpLabelCounter); 
				fprintf(DEST_ASM, "\tcmpl\t$0, %d(%%rbp)\n", op2StackIndex);
				fprintf(DEST_ASM, "\tje	.L%d\n", logicOpLabelCounter);  logicOpLabelCounter++;
				fprintf(DEST_ASM, "\tmovl\t$1, %%eax\n");
				fprintf(DEST_ASM, "\tjmp\t.L%d\n", logicOpLabelCounter); 
				fprintf(DEST_ASM, ".L%d:\n", logicOpLabelCounter-1);
				fprintf(DEST_ASM, "\tmovl\t$0, %%eax\n");
				fprintf(DEST_ASM, ".L%d:\n", logicOpLabelCounter);
				if(currentTAC->res->symbolType == TK_IDENTIFIER)
            	{	
            		//Hyp: There is space allocated for this in the stack
            		resStackIndex = -4*(1+insertTempInStack(currentTAC->res));
            	}
				fprintf(DEST_ASM, "\tmovl\t%%eax, %d(%%rbp)\n", resStackIndex);
				logicOpLabelCounter++;
				break;


			case TAC_OR:
            	op1StackIndex = -4 * (getDataStackIndex(currentTAC->op1) +1);
				op2StackIndex = -4 * (getDataStackIndex(currentTAC->op2) +1);
				resStackIndex = -4 * (getDataStackIndex(currentTAC->res) +1);

            	fprintf(DEST_ASM, "\tcmpl\t$0, %d(%%rbp)\n", op1StackIndex);
				fprintf(DEST_ASM, "\tjne	.L%d\n", logicOpLabelCounter);  logicOpLabelCounter++; //L2
				fprintf(DEST_ASM, "\tcmpl\t$0, %d(%%rbp)\n", op2StackIndex);
				fprintf(DEST_ASM, "\tje	.L%d\n", logicOpLabelCounter);  //l3
				fprintf(DEST_ASM, ".L%d:\n", logicOpLabelCounter-1); 		logicOpLabelCounter++;  //l2: 
				fprintf(DEST_ASM, "\tmovl\t$1, %%eax\n");
				fprintf(DEST_ASM, "\tjmp\t.L%d\n", logicOpLabelCounter); 
				fprintf(DEST_ASM, ".L%d:\n", logicOpLabelCounter-1);
				fprintf(DEST_ASM, "\tmovl\t$0, %%eax\n");
				fprintf(DEST_ASM, ".L%d:\n", logicOpLabelCounter);
				if(currentTAC->res->symbolType == TK_IDENTIFIER)
            	{	
            		//Hyp: There is space allocated for this in the stack
            		resStackIndex = -4*(1+insertTempInStack(currentTAC->res));
            	}
				fprintf(DEST_ASM, "\tmovl\t%%eax, %d(%%rbp)\n", resStackIndex);
				logicOpLabelCounter++;
				break;

			case TAC_READ:
				switch (currentTAC->res->symbolType)
                {
                    case SYMBOL_LOCAL_VARIABLE:
                        stackIndex = getDataStackIndex(currentTAC->res);
                        fprintf(DEST_ASM, "\tleaq\t%i(%%rbp), %%rax\n", (stackIndex+1) * -4);
                        fprintf(DEST_ASM, "\tmovq\t%%rax, %%rsi\n");
						
                        break;
                    case SYMBOL_GLOBAL_VARIABLE:
                        fprintf(DEST_ASM, "\tmovl\t%s(%%rip), %%eax\n", currentTAC->res->symbol);
                        break;
                    default:
                        break;
                }
                switch(currentTAC->res->dataType)
            	{
            		case DATATYPE_INT:
                        fprintf(DEST_ASM, "\tmovl\t$.LC0, %%edi\n");
                        break;
            		case DATATYPE_CHAR:
            		case DATATYPE_BOOL:
                        fprintf(DEST_ASM, "\tmovl\t$.LC2, %%edi\n");
                        break;
            		case DATATYPE_REAL:
            			//TODO
                        break;
            		default:
            		break;
            	}
            	fprintf(DEST_ASM, "\tmovl\t$0, %%eax\n");
				fprintf(DEST_ASM, "\tcall\t__isoc99_scanf\n");
				break;

			default:
				break;
		}
		currentTAC = currentTAC->next;
	}
}

int getDataStackIndex(HASH_NODE* hashNode)
{
	int i;
	for (i = 0; i < MAX_STACK; i++)
	{
		if(stackOffsetControl[i] == hashNode)
			return i;
	}
}

int getParamStackIndex(HASH_NODE* hashNode)
{
    int i;
    for (i = 0; i < MAX_STACK; i++)
    {
        if(paramOffsetControl[i] == hashNode)
            return i;
    }
}

int insertTempInStack(HASH_NODE* hashNode)
{
	stackOffsetControl[stackCounter+1] = hashNode;
	stackCounter++;
	return stackCounter;
}
