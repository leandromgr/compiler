
#include "semantic.h"
#include "y.tab.h"

// Count the semantic errors
int semanticErrors = 0;

void setDeclarations(AST_NODE *astNode)
{
    if (!astNode)
        return;

    // Check if it is either a new function variable or a global variable
    if (astNode->type == AST_LOCAL_VAR_LIST || astNode->type == AST_GLOBAL_VAR_LIST)
    {
        // Get the element with the actual information about the variable name and type
        AST_NODE * variableTypeNode = astNode->children[0];
        if (variableTypeNode->hashNode)
        {
            // Check for multiple declarations
            if (variableTypeNode->hashNode->symbolType != TK_IDENTIFIER)
            {
                fprintf(stderr, "Error: variable '%s' already declared!\n", variableTypeNode->hashNode->symbol);
                semanticErrors++;
            }
            else
            {
                // Check if it is a variable or vector
                if (variableTypeNode->children[0]->type == AST_SYMBOL)
                {
                    if (astNode->type == AST_LOCAL_VAR_LIST)
                    {
                        variableTypeNode->hashNode->symbolType = SYMBOL_LOCAL_VARIABLE;
                    }
                    else
                    {
                        variableTypeNode->hashNode->symbolType = SYMBOL_GLOBAL_VARIABLE;
                    }
                }
                else
                {
                    variableTypeNode->hashNode->symbolType = SYMBOL_GLOBAL_VECTOR;
                }
                // Set the variable type in the hash
                switch (variableTypeNode->type)
                {
                    case AST_INT:
                        variableTypeNode->hashNode->dataType = DATATYPE_INT;
                        break;
                    case AST_CHAR:
                        variableTypeNode->hashNode->dataType = DATATYPE_CHAR;
                        break;
                    case AST_BOOL:
                        variableTypeNode->hashNode->dataType = DATATYPE_BOOL;
                        break;
                    case AST_REAL:
                        variableTypeNode->hashNode->dataType = DATATYPE_REAL;
                        break;
                    default:
                        // error?
                        break;
                }
            }
        }
    }
    // Declare all functions
    else if (astNode->type == AST_FUNCTION)
    {
        // Get the element with the actual information about the function name and type
        AST_NODE * functionNode = astNode->children[0];
        if (functionNode->hashNode)
        {
            // Check for multiple declarations
            if (functionNode->hashNode->symbolType != TK_IDENTIFIER)
            {
                fprintf(stderr, "Error: function '%s' already declared!\n", functionNode->hashNode->symbol);
                semanticErrors++;
            }
            else
            {
                // Set node type as function identifier
                functionNode->hashNode->symbolType = SYMBOL_FUNCTION;

                // Set the variable type in the hash
                switch (functionNode->type)
                {
                    case AST_INT:
                        functionNode->hashNode->dataType = DATATYPE_INT;
                        break;
                    case AST_CHAR:
                        functionNode->hashNode->dataType = DATATYPE_CHAR;
                        break;
                    case AST_BOOL:
                        functionNode->hashNode->dataType = DATATYPE_BOOL;
                        break;
                    case AST_REAL:
                        functionNode->hashNode->dataType = DATATYPE_REAL;
                        break;
                    default:
                        // error?
                        break;
                }
            }
        }
    }
    // Declare all function parameters
    else if (astNode->type == AST_PARAMETER_LIST)
    {
        // Get the element with the actual information about the function parameter name and type
        AST_NODE * parameterNode = astNode->children[0];
        if (parameterNode)
        {
            if (parameterNode->hashNode)
            {
                // Check for multiple declarations
                if (parameterNode->hashNode->symbolType != TK_IDENTIFIER)
                {
                    fprintf(stderr, "Error: parameter '%s' already declared!\n", parameterNode->hashNode->symbol);
                    semanticErrors++;
                }
                else
                {
                    // Set node type as function identifier
                    parameterNode->hashNode->symbolType = SYMBOL_FUNCTION_PARAMETER;

                    // Set the variable type in the hash
                    switch (parameterNode->type)
                    {
                        case AST_INT:
                            parameterNode->hashNode->dataType = DATATYPE_INT;
                            break;
                        case AST_CHAR:
                            parameterNode->hashNode->dataType = DATATYPE_CHAR;
                            break;
                        case AST_BOOL:
                            parameterNode->hashNode->dataType = DATATYPE_BOOL;
                            break;
                        case AST_REAL:
                            parameterNode->hashNode->dataType = DATATYPE_REAL;
                            break;
                        default:
                            // error?
                            break;
                    }
                }
            }
        }
    }

    int indexChild;
    for (indexChild = 0; indexChild < MAX_CHILDREN; indexChild++)
    {
        setDeclarations(astNode->children[indexChild]);
    }
}

void checkUndeclared()
{
    int indexHash = 0;
    HASH_NODE * hashNode;

    // Check all hash positions
    for (indexHash = 0; indexHash < HASH_SIZE; indexHash++)
    {
        // Check all list elements inside a given position in the hash
        for (hashNode = symbolTable[indexHash]; hashNode; hashNode = hashNode->nextNode)
        {
            if (hashNode->symbolType == TK_IDENTIFIER)
            {
                fprintf(stderr, "Error: parameter '%s' undeclared!\n", hashNode->symbol);
                semanticErrors++;
            }
        }
    }
}
