
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
                functionNode->hashNode->functionLink = (void *) functionNode;

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

int checkTypes (AST_NODE * astNode)
{
    if (!astNode)
        return DATATYPE_UNTYPED;

    printf("\t\tNode type: %i\n\n", astNode->type);
    // Testing types on attribution
    switch (astNode->type)
    {
        case AST_ATTRIBUTION:
            checkAttribution(astNode);
            return DATATYPE_UNTYPED;
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
            return checkDataTypeCompatibility(checkTypes(astNode->children[0]), checkTypes(astNode->children[1]));
        case AST_FUNCALL:
            return checkFunctionCall(astNode);
        case AST_INPUT_CMD:
            return DATATYPE_UNTYPED;
        case AST_OUTPUT_CMD:
            return DATATYPE_UNTYPED;
        case AST_RETURN_CMD:
            return DATATYPE_UNTYPED;
        case AST_IF:
        case AST_IFELSE:
            return checkDataTypeCompatibility(DATATYPE_BOOL, checkTypes(astNode->children[0]));
        case AST_LOOP:
            return DATATYPE_UNTYPED;
        case AST_SYMBOL:
            return astNode->hashNode->dataType;
        default:
            break;
    }

    int indexNode;
    for (indexNode = 0; indexNode < MAX_CHILDREN; indexNode++)
    {
        checkTypes(astNode->children[indexNode]);
    }

    return DATATYPE_UNTYPED;
}

char * printDataType(int dataType)
{
    switch (dataType)
    {
        case DATATYPE_INT:
            return "int";
        case DATATYPE_CHAR:
            return "char";
        case DATATYPE_BOOL:
            return "bool";
        case DATATYPE_REAL:
            return "real";
        default:
            return "unkown";
    }
}

int checkFunctionCall(AST_NODE * astNode)
{
    AST_NODE * currentArgument = astNode->children[0];
    AST_NODE * functionDefinition = (AST_NODE *) astNode->hashNode->functionLink;
    AST_NODE * currentParameterList = functionDefinition->children[0];

    while ((currentArgument && currentParameterList) && currentParameterList->children[0])
    {
        if (checkDataTypeCompatibility(currentArgument->hashNode->dataType, currentParameterList->children[0]->hashNode->dataType) == DATATYPE_INCOMPATIBLE)
        {
            fprintf(stderr, "Error: invalid argument type for function '%s'. Expected: '%s', found '%s'!\n", astNode->hashNode->symbol, printDataType(currentParameterList->children[0]->hashNode->dataType), printDataType(currentArgument->hashNode->dataType));
            semanticErrors++;
            return DATATYPE_INCOMPATIBLE;
        }
        currentArgument = currentArgument->children[0];
        currentParameterList = currentParameterList->children[1];
    }

    if (currentArgument || currentParameterList)
    {
        fprintf(stderr, "Error: number of arguments does not match number of parameters on function '%s'!\n", astNode->hashNode->symbol);
        semanticErrors++;
    }

    return functionDefinition->hashNode->dataType;
}

void checkAttribution(AST_NODE * astNode)
{
    AST_NODE * destinationNode = astNode->children[0];
    // Check if it is a variable or a vector
    if ((destinationNode->hashNode->symbolType == SYMBOL_LOCAL_VARIABLE) ||
        (destinationNode->hashNode->symbolType == SYMBOL_GLOBAL_VARIABLE))
    {
        // Check if the variable was misused, i.e, with index
        if (destinationNode->children[0])
        {
            fprintf(stderr, "Error: variable '%s' is not a vector!\n", destinationNode->hashNode->symbol);
            semanticErrors++;
        }
        else
        {
            // Check expression data type
            if (checkDataTypeCompatibility(destinationNode->hashNode->dataType, checkTypes(astNode->children[1])) == DATATYPE_INCOMPATIBLE)
            {
                fprintf(stderr, "Error: variable '%s' attribution has invalid data type!\n", destinationNode->hashNode->symbol);
                semanticErrors++;
            }
        }
    }
    else if (destinationNode->hashNode->symbolType == SYMBOL_GLOBAL_VECTOR)
    {
        // Check if the variable was misused, i.e, without index
        if (!destinationNode->children[0])
        {
            fprintf(stderr, "Error: vector '%s' used without index!\n", destinationNode->hashNode->symbol);
            semanticErrors++;
        }
        else
        {
            // Check index type
            // TODO: check array bounds?
            if (checkDataTypeCompatibility(DATATYPE_INT, checkTypes(destinationNode->children[0])) != DATATYPE_INT)
            {
                fprintf(stderr, "Error: index of vector '%s' has invalid data type!\n", destinationNode->hashNode->symbol);
                semanticErrors++;
            }
            else
            {
                // Check expression data type
                if (checkDataTypeCompatibility(destinationNode->hashNode->dataType, checkTypes(astNode->children[1])) == DATATYPE_INCOMPATIBLE)
                {
                    fprintf(stderr, "Error: vector '%s' attribution has invalid data type!\n", destinationNode->hashNode->symbol);
                    semanticErrors++;
                }
            }
        }
    }
    else
    {
        fprintf(stderr, "Error: symbol '%s' is not a variable or vector!\n", destinationNode->hashNode->symbol);
        semanticErrors++;
    }
}

// Return the 'highest' data type
int checkDataTypeCompatibility(int dataType1, int dataType2)
{
    if (dataType1 == DATATYPE_INT)
    {
        switch (dataType2)
        {
            case DATATYPE_INT:
                return DATATYPE_INT;
            case DATATYPE_CHAR:
                return DATATYPE_INT;
            case DATATYPE_BOOL:
                return DATATYPE_INCOMPATIBLE;
            case DATATYPE_REAL:
                return DATATYPE_REAL;
            default:
                return DATATYPE_INCOMPATIBLE;
        }
    }
    else if (dataType1 == DATATYPE_CHAR)
    {
        switch (dataType2)
        {
            case DATATYPE_INT:
                return DATATYPE_INT;
            case DATATYPE_CHAR:
                return DATATYPE_CHAR;
            case DATATYPE_BOOL:
                return DATATYPE_INCOMPATIBLE;
            case DATATYPE_REAL:
                return DATATYPE_REAL;
            default:
                return DATATYPE_INCOMPATIBLE;
        }
    }
    else if (dataType1 == DATATYPE_BOOL)
    {
        switch (dataType2)
        {
            case DATATYPE_INT:
                return DATATYPE_INCOMPATIBLE;
            case DATATYPE_CHAR:
                return DATATYPE_INCOMPATIBLE;
            case DATATYPE_BOOL:
                return DATATYPE_BOOL;
            case DATATYPE_REAL:
                return DATATYPE_INCOMPATIBLE;
            default:
                return DATATYPE_INCOMPATIBLE;
        }
    }
    else if (dataType1 == DATATYPE_REAL)
    {
        switch (dataType2)
        {
            case DATATYPE_INT:
                return DATATYPE_REAL;
            case DATATYPE_CHAR:
                return DATATYPE_REAL;
            case DATATYPE_BOOL:
                return DATATYPE_INCOMPATIBLE;
            case DATATYPE_REAL:
                return DATATYPE_REAL;
            default:
                return DATATYPE_INCOMPATIBLE;
        }
    }
    else
    {
        return DATATYPE_INCOMPATIBLE;
    }
}
