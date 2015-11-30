#include "assembler.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>  

#include "ast.h"
#include "tac.h"
#include "semantic.h"
#include "y.tab.h"

void avengersAssemble(AST_NODE* astTree, TAC* tacList)
{
	printHeaders();
	parseGlobalVariables(astTree);
	parseStrings();
	fprintf(stderr, "\t.text\n");
	parseTAC(tacList);
	printFooter();
}

void printHeaders()
{
	fprintf(stderr, "\t.file\t\"test1.txt\"\n");
}

void printFooter()
{
	fprintf(stderr, ".LFE0:\n");
	fprintf(stderr, "\t.size\tmain, .-main\n");
	fprintf(stderr, "\t.ident\t\"GCC: (Ubuntu 4.8.4-2ubuntu1~14.04) 4.8.4\"\n");
	fprintf(stderr, "\t.section\t.note.GNU-stack,\"\",@progbits\n");
}

void parseStrings()
{
	int indexHash;
	int stringCounter = 0;
	HASH_NODE *currentNode;

	fprintf(stderr, "\t.section\t.rodata\n");
	fprintf(stderr, ".LC0:\n");
	fprintf(stderr, "\t.string \"%%d\"\n");
	fprintf(stderr, ".LC1:\n");
	fprintf(stderr, "\t.string \"%%f\"\n");
	fprintf(stderr, ".LC2:\n");
	fprintf(stderr, "\t.string \"%%c\"\n");

	for(indexHash=0 ; indexHash<HASH_SIZE; indexHash++)
	{
		for(currentNode=symbolTable[indexHash] ; currentNode ; currentNode = currentNode->nextNode)
		{
			if(currentNode->symbolType == LIT_STRING)
			{
				fprintf(stderr, ".LC%d:\n", stringCounter+3);
				fprintf(stderr, "\t.string \"%s\"\n", currentNode->symbol);
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

	fprintf(stderr, "\t.globl %s\n", variableNode->hashNode->symbol);
	fprintf(stderr, "\t.type %s, @object\n", variableNode->hashNode->symbol);

	AST_NODE *valueOrVector = variableNode->children[0];
	if(valueOrVector->type == AST_GLOBAL_VECTOR)
	{
		int vectorSize = atoi(valueOrVector->hashNode->symbol);
		int indexInitialValue;
		AST_NODE *currentInitialValue = valueOrVector->children[0];
		switch(variableNode->type)
		{
			case AST_INT:
				fprintf(stderr, "\t.size %s, %d\n",variableNode->hashNode->symbol, 4*vectorSize);
				fprintf(stderr, "%s:\n", variableNode->hashNode->symbol);
				for(indexInitialValue=0; indexInitialValue<vectorSize ; indexInitialValue++)
				{
					if(currentInitialValue)
					{
						fprintf(stderr, "\t.long %s\n", currentInitialValue->hashNode->symbol);
						currentInitialValue = currentInitialValue->children[0];
					}
					else
					{
						fprintf(stderr, "\t.long 0\n");
					}
				}
				break;
			
			case AST_CHAR:
				fprintf(stderr, "\t.size %s, %d\n", variableNode->hashNode->symbol, 1*vectorSize);
				fprintf(stderr, "%s:\n", variableNode->hashNode->symbol);
				for(indexInitialValue=0; indexInitialValue<vectorSize ; indexInitialValue++)
				{
					if(currentInitialValue)
					{
						fprintf(stderr, "\t.byte %i\n", currentInitialValue->hashNode->symbol[0]);
						currentInitialValue = currentInitialValue->children[0];
					}
					else
					{
						fprintf(stderr, "\t.byte 0\n");
					}
				}
				break;

			case AST_BOOL:
				fprintf(stderr, "\t.size %s, %d\n", variableNode->hashNode->symbol, 1*vectorSize);
				fprintf(stderr, "%s:\n", variableNode->hashNode->symbol);

				while(currentInitialValue)
				{
					if(currentInitialValue->hashNode->symbolType == LIT_TRUE)
						fprintf(stderr, "\t.byte 1\n");
					else
						fprintf(stderr, "\t.byte 0\n");
	
					currentInitialValue = currentInitialValue->children[0];
				}
				for(indexInitialValue=0; indexInitialValue<vectorSize ; indexInitialValue++)
				{
					if(currentInitialValue)
					{
						if(currentInitialValue->hashNode->symbolType == LIT_TRUE)
							fprintf(stderr, "\t.byte 1\n");
						else
							fprintf(stderr, "\t.byte 0\n");
	
						currentInitialValue = currentInitialValue->children[0];
					}
					else
					{
						fprintf(stderr, "\t.byte 0\n");
					}
				}
				break;

			case AST_REAL:
				fprintf(stderr, "\t.size %s, %d\n", variableNode->hashNode->symbol, 4*vectorSize);
				fprintf(stderr, "%s:\n", variableNode->hashNode->symbol);
				for(indexInitialValue=0; indexInitialValue<vectorSize ; indexInitialValue++)
				{
					if(currentInitialValue)
					{
						fprintf(stderr, "\t.single %s\n", currentInitialValue->hashNode->symbol);
						currentInitialValue = currentInitialValue->children[0];
					}
					else
					{
						fprintf(stderr, "\t.single 0\n");
					}
				}
				break;
			default:
				fprintf(stderr, "ERROR: Varible type not valid!");
				exit(6);
		}
	}
	else //it is a simple global variable
	{
		switch(variableNode->type)
		{
			case AST_INT:
				fprintf(stderr, "\t.size %s, 4\n", variableNode->hashNode->symbol);
				fprintf(stderr, "%s:\n", variableNode->hashNode->symbol);
				fprintf(stderr, "\t.long %s\n", valueOrVector->hashNode->symbol);
				break;
			case AST_CHAR:
				fprintf(stderr, "\t.size %s, 1\n", variableNode->hashNode->symbol);
				fprintf(stderr, "%s:\n", variableNode->hashNode->symbol);
				fprintf(stderr, "\t.byte %i\n", valueOrVector->hashNode->symbol[0]);
				break;
			case AST_BOOL:
				fprintf(stderr, "\t.size %s, 1\n", variableNode->hashNode->symbol);
				fprintf(stderr, "%s:\n", variableNode->hashNode->symbol);
				if(valueOrVector->hashNode->symbolType == LIT_TRUE)
					fprintf(stderr, "\t.byte 1\n");
				else
					fprintf(stderr, "\t.byte 0\n");
					
				break;

			case AST_REAL:
				fprintf(stderr, "\t.size %s, 4\n", variableNode->hashNode->symbol);
				fprintf(stderr, "%s:\n", variableNode->hashNode->symbol);
				fprintf(stderr, "\t.single %s\n", valueOrVector->hashNode->symbol);
				break;
			default:
				fprintf(stderr, "ERROR: Varible type not valid!");
				exit(6);
		}
	}

	
}

void parseTAC(TAC* tacList)
{
	TAC * currentTAC;
	int stackCounter;
	int localVariableCounter;
    int stackIndex;
	int i;

	if(!tacList)
		return;

	currentTAC = tacList;

	while (currentTAC)
	{
		switch(currentTAC->type)
		{
			case TAC_FUNBEGIN:
				fprintf(stderr, "\t.globl\t%s\n", currentTAC->op1->symbol);
				fprintf(stderr, "\t.type\t%s, @function\n", currentTAC->op1->symbol);
				fprintf(stderr, "%s:\n", currentTAC->op1->symbol);
				fprintf(stderr, "\tpushq\t%%rbp\n");
				fprintf(stderr, "\tmovq\t%%rsp, %%rbp\n");
				fprintf(stderr, "\tsubq\t$16, %%rsp\n"); //GHOST TOSEE
				stackCounter = 0;
				localVariableCounter = 0;
				break;

			case TAC_MOV_INIT:
				if((localVariableCounter == 0)&&(stackCounter != 0))
				{	
					//Shifting the already existent parameters by 2 spaces
					for (i = stackCounter ; i > 0; i--)
					{
						stackOffsetControl[i+1] = stackOffsetControl[i-1];
					}
					stackCounter+=2;
				}

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
						fprintf(stderr, "\tmovl\t$%i, %i(%%rbp)\n", localVarInitalValue, -4*(localVariableCounter+1));
						break;
					case LIT_CHAR:
						localVarInitalValue =  (int) currentTAC->op1->symbol[0];
						fprintf(stderr, "\tmovb\t$%i, %i(%%rbp)\n", localVarInitalValue, -4*(localVariableCounter+1));
						break;
					case LIT_TRUE:
						localVarInitalValue = 1;
						fprintf(stderr, "\tmovl\t$%i, %i(%%rbp)\n", localVarInitalValue, -4*(localVariableCounter+1));
						break;
					case LIT_FALSE:
						localVarInitalValue = 0;
						fprintf(stderr, "\tmovl\t$%i, %i(%%rbp)\n", localVarInitalValue, -4*(localVariableCounter+1));
						break;
					default:
						localVarInitalValue = 0;
						fprintf(stderr, "\tmovl\t$%i, %i(%%rbp)\n", localVarInitalValue, -4*(localVariableCounter+1));
				}
				
				localVariableCounter++;
				break;

			case TAC_FUNEND:
				fprintf(stderr, "\tleave\n"); //TIRAMOS O POP, GHOST, VER O NOME DA FUNçÂO
				fprintf(stderr, "\tret\n");
				break;

			case TAC_PARAM:
				for (i = stackCounter ; i > 0; i--)
				{
					stackOffsetControl[i] = stackOffsetControl[i-1];
				}
				stackOffsetControl[0] = currentTAC->op1;

				stackCounter++;
				break;

			case TAC_PRINT:
				if (currentTAC->op1->symbolType == LIT_STRING)
				{
					for(i=0 ; i<MAX_STRINGS ; i++)
					{
						if(currentTAC->op1 == stringTable[i])
							break;
					}
					fprintf(stderr, "\tmovl\t$.LC%d, %%edi\n", i+3);
					fprintf(stderr, "\tmovl\t$0, %%eax\n");
					fprintf(stderr, "\tcall\tprintf\n");
				}
				else if(currentTAC->op1->symbolType == SYMBOL_GLOBAL_VARIABLE)
				{
					switch(currentTAC->op1->dataType)
					{
						case DATATYPE_INT:
							fprintf(stderr, "\tmovl\t%s(%%rip), %%eax\n", currentTAC->op1->symbol);
							fprintf(stderr, "\tmovl\t%%eax, %%esi\n");
							fprintf(stderr, "\tmovl\t$.LC0, %%edi\n");
							fprintf(stderr, "\tmovl\t$0, %%eax\n");
							fprintf(stderr, "\tcall\tprintf\n");
							break;

						case DATATYPE_BOOL:
						case DATATYPE_CHAR:
							fprintf(stderr, "\tmovl\t%s(%%rip), %%eax\n", currentTAC->op1->symbol);
							fprintf(stderr, "\tmovl\t%%eax, %%edi\n");
							fprintf(stderr, "\tcall\tputchar\n");
							break;

						case DATATYPE_REAL:
							fprintf(stderr, "\tmovss\t%s(%%rip), %%xmm0\n", currentTAC->op1->symbol);
							fprintf(stderr, "\tunpcklps\t%%xmm0, %%xmm0\n");
							fprintf(stderr, "\tcvtps2pd\t%%xmm0, %%xmm0\n");
							fprintf(stderr, "\tmovl\t$.LC1, %%edi\n");
							fprintf(stderr, "\tmovl\t$1, %%eax\n");
							fprintf(stderr, "\tcall\tprintf\n");
							break;
					}
				}	
				else if(currentTAC->op1->symbolType == SYMBOL_GLOBAL_VECTOR)
				{
					switch(currentTAC->op1->dataType)
					{
						case DATATYPE_INT:
							fprintf(stderr, "\tmovl\t%s+%i(%%rip), %%eax\n", currentTAC->op1->symbol, 4*atoi(currentTAC->prev->op1->symbol));
							fprintf(stderr, "\tmovl\t%%eax, %%esi\n");
							fprintf(stderr, "\tmovl\t$.LC0, %%edi\n");
							fprintf(stderr, "\tmovl\t$0, %%eax\n");
							fprintf(stderr, "\tcall\tprintf\n");
							break;

						case DATATYPE_BOOL:
						case DATATYPE_CHAR:
							fprintf(stderr, "\tmovzbl\t%s+%i(%%rip), %%eax\n", currentTAC->op1->symbol, atoi(currentTAC->prev->op1->symbol));
							fprintf(stderr, "\tmovsbl\t%%al, %%eax\n");
							fprintf(stderr, "\tmovl\t%%eax, %%edi\n");
							fprintf(stderr, "\tcall\tputchar\n");
							break;

						case DATATYPE_REAL:
							fprintf(stderr, "\tmovss\t%s+%i(%%rip), %%xmm0\n", currentTAC->op1->symbol, 4*atoi(currentTAC->prev->op1->symbol));
							fprintf(stderr, "\tunpcklps\t%%xmm0, %%xmm0\n");
							fprintf(stderr, "\tcvtps2pd\t%%xmm0, %%xmm0\n");
							fprintf(stderr, "\tmovl\t$.LC1, %%edi\n");
							fprintf(stderr, "\tmovl\t$1, %%eax\n");
							fprintf(stderr, "\tcall\tprintf\n");
							break;
					}
				}
				else if(currentTAC->op1->symbolType == LIT_INTEGER)
				{
							
					fprintf(stderr, "\tmovl\t$%i, %%eax\n", atoi(currentTAC->op1->symbol));
					fprintf(stderr, "\tmovl\t%%eax, %%esi\n");
					fprintf(stderr, "\tmovl\t$.LC0, %%edi\n");
					fprintf(stderr, "\tmovl\t$0, %%eax\n");
					fprintf(stderr, "\tcall\tprintf\n");
					break;
				}
				else if (currentTAC->op1->symbolType == LIT_CHAR)
				{
					fprintf(stderr, "\tmovl\t$%i, %%eax\n", currentTAC->op1->symbol[0]);
					fprintf(stderr, "\tmovl\t%%eax, %%edi\n");
					fprintf(stderr, "\tcall\tputchar\n");
					break;
				}
				else if (currentTAC->op1->symbolType == LIT_TRUE)
				{
					fprintf(stderr, "\tmovl\t$%i, %%eax\n", 1);
					fprintf(stderr, "\tmovl\t%%eax, %%esi\n");
					fprintf(stderr, "\tmovl\t$.LC0, %%edi\n");
					fprintf(stderr, "\tmovl\t$0, %%eax\n");
					fprintf(stderr, "\tcall\tprintf\n");
					break;
				}
				else if (currentTAC->op1->symbolType == LIT_FALSE)
				{
					fprintf(stderr, "\tmovl\t$%i, %%eax\n", 0);
					fprintf(stderr, "\tmovl\t%%eax, %%esi\n");
					fprintf(stderr, "\tmovl\t$.LC0, %%edi\n");
					fprintf(stderr, "\tmovl\t$0, %%eax\n");
					fprintf(stderr, "\tcall\tprintf\n");
					break;
				}
				else if((currentTAC->op1->symbolType == SYMBOL_LOCAL_VARIABLE) ||
						(currentTAC->op1->symbolType == SYMBOL_FUNCTION_PARAMETER))
				{
                    stackIndex = getDataStackIndex(currentTAC->op1);
					switch(currentTAC->op1->dataType)
					{
						case DATATYPE_INT:
						case DATATYPE_BOOL:
							fprintf(stderr, "\tmovl\t%i(%%rbp), %%eax\n", (stackIndex+1) * -4);
							fprintf(stderr, "\tmovl\t%%eax, %%esi\n");
							fprintf(stderr, "\tmovl\t$.LC0, %%edi\n");
							fprintf(stderr, "\tmovl\t$0, %%eax\n");
							fprintf(stderr, "\tcall\tprintf\n");
							break;


						case DATATYPE_CHAR:
							fprintf(stderr, "\tmovsbl\t%i(%%rbp), %%eax\n", (stackIndex+1) * -4);
							fprintf(stderr, "\tmovl\t%%eax, %%esi\n");
							fprintf(stderr, "\tmovl\t$.LC2, %%edi\n");
							fprintf(stderr, "\tmovl\t$0, %%eax\n");
							fprintf(stderr, "\tcall\tprintf\n");
							break;

						case DATATYPE_REAL:
							fprintf(stderr, "\tmovss\t%i(%%rbp), %%xmm0\n", (stackIndex+1) * -4);
							fprintf(stderr, "\tunpcklps\t%%xmm0, %%xmm0\n");
							fprintf(stderr, "\tcvtps2pd\t%%xmm0, %%xmm0\n");
							fprintf(stderr, "\tmovl\t$.LC1, %%edi\n");
							fprintf(stderr, "\tmovl\t$1, %%eax\n");
							fprintf(stderr, "\tcall\tprintf\n");
							break;
					}
				}	 
				break;

            case TAC_JZ:
                switch (currentTAC->op1->symbolType)
                {
                    case SYMBOL_LOCAL_VARIABLE:
                    case SYMBOL_FUNCTION_PARAMETER:
                        stackIndex = getDataStackIndex(currentTAC->op1);
                        fprintf(stderr, "\tcmpl\t$1, %i(%%rbp)\n", (stackIndex+1) * -4);
                        break;
                    case SYMBOL_GLOBAL_VECTOR:
                        fprintf(stderr, "\tcmpl\t$1, %s+%i(%%rip)\n", currentTAC->op1->symbol, 4*atoi(currentTAC->prev->op1->symbol));
                        break;
                    case SYMBOL_GLOBAL_VARIABLE:
                        fprintf(stderr, "\tcmpl\t$1, %s(%%rip)\n", currentTAC->op1->symbol);
                        break;
                    case LIT_INTEGER:
                        fprintf(stderr, "\tcmpl\t$1, %i\n", atoi(currentTAC->op1->symbol));
                        break;
                    case LIT_CHAR:
                        fprintf(stderr, "\tcmpl\t$1, %i\n", currentTAC->op1->symbol[0]);
                        break;
                    case LIT_TRUE:
                        fprintf(stderr, "\tcmpl\t$1, $1\n");
                        break;
                    case LIT_FALSE:
                        fprintf(stderr, "\tcmpl\t$1, $0\n");
                        break;
                    default:
                        break;
                }

                fprintf(stderr, "\tjne\t.%s\n", currentTAC->res->symbol);
                break;

            case TAC_LABEL:
                fprintf(stderr, ".%s:\n", currentTAC->res->symbol);
                break;
			/*case TAC_SYMBOL:
			case TAC_SUM:
				fprintf(stderr, "\tmovl %s\n", );
				fprintf();
			case TAC_SUM     
			case TAC_SUB   	
			case TAC_MULT    
			case TAC_DIV		
			case TAC_LT 		
			case TAC_GT 		
			case TAC_LET 	
			case TAC_GET 	
			case TAC_EQ 		
			case TAC_NE 		
			case TAC_AND 	
			case TAC_OR 		

			case TAC_CALL	
			case TAC_ARG		
		
			case TAC_READ	
			case TAC_RETURN	
			case TAC_LABEL         
			case TAC_JUMP*/    
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
