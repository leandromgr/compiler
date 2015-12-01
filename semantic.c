
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
            return checkDataTypeCompatibility(checkTypes(astNode->children[0]), checkTypes(astNode->children[1]));
        case AST_LT:
        case AST_GT:
        case AST_LET:
        case AST_GET:
        case AST_EQ:
        case AST_NE:
        case AST_AND:
        case AST_OR:
            if( checkDataTypeCompatibility(checkTypes(astNode->children[0]), checkTypes(astNode->children[1])) >= 0)
            {
                return DATATYPE_BOOL;
            }
            else
            {
                semanticErrors++;
                fprintf(stderr, "Error: the expression must be boolean!");
                return DATATYPE_INCOMPATIBLE;
            }
        case AST_FUNCALL:
            return checkFunctionCall(astNode);
        case AST_INPUT_CMD:
            if (checkVariable(astNode) == DATATYPE_UNTYPED)
            {
                semanticErrors++;
                fprintf(stderr, "Error: invalid variable!");
                return DATATYPE_INCOMPATIBLE;
            }
            else
            {
                return DATATYPE_UNTYPED;
            }
        case AST_OUTPUT_CMD:
            return checkOutputCommand(astNode);
        case AST_RETURN_CMD:
            if (checkDataTypeCompatibility(DATATYPE_EXPRESSION, checkTypes(astNode->children[0])) < 0)
            {
                semanticErrors++;
                fprintf(stderr, "Error: invalid return value!");
                return DATATYPE_INCOMPATIBLE;
            }
            return DATATYPE_UNTYPED;
        case AST_IF:
        case AST_IFELSE:
        case AST_LOOP:
            if (checkDataTypeCompatibility(DATATYPE_EXPRESSION, checkTypes(astNode->children[0])) < 0)
            {
                semanticErrors++;
                fprintf(stderr, "Error: invalid expression in the conditional command!");
                return DATATYPE_INCOMPATIBLE;
            }
            else
            {
                if(astNode->children[1])
                {
                    checkTypes(astNode->children[1]);
                }
                if (astNode->children[2])
                {
                    checkTypes(astNode->children[2]);
                }
                return DATATYPE_UNTYPED;
            }
        case AST_SYMBOL:
            switch (astNode->hashNode->symbolType)
            {
                case LIT_CHAR:
                    astNode->hashNode->dataType = DATATYPE_CHAR;
                    return DATATYPE_CHAR;
                case LIT_INTEGER:
                    astNode->hashNode->dataType = DATATYPE_INT;
                    return DATATYPE_INT;
                case LIT_FALSE:
                case LIT_TRUE:
                    astNode->hashNode->dataType = DATATYPE_CHAR;
                    return DATATYPE_BOOL;
                case LIT_STRING:
                    return DATATYPE_UNTYPED;
                default:
                    return checkVariable(astNode);
            }
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

int checkOutputCommand(AST_NODE * astNode)
{
    AST_NODE * currentOutputList = astNode->children[0];
    while(currentOutputList)
    { 
        // Check if there is things to check
        if (currentOutputList->children[0])
        {
            if (!(currentOutputList->children[0]->hashNode) || 
                (currentOutputList->children[0]->hashNode && currentOutputList->children[0]->hashNode->symbolType != LIT_STRING))
            {
                if (checkTypes(currentOutputList->children[0]) < 0)
                {
                    semanticErrors++;
                    fprintf(stderr, "Error: invalid expression in 'output' command!");
                    return DATATYPE_INCOMPATIBLE;
                }
            }
            currentOutputList = currentOutputList->children[1];
        }
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

int checkVariable(AST_NODE * astNode)
{
    if ((astNode->hashNode->symbolType == SYMBOL_LOCAL_VARIABLE) ||
        (astNode->hashNode->symbolType == SYMBOL_GLOBAL_VARIABLE) ||
        (astNode->hashNode->symbolType == SYMBOL_FUNCTION_PARAMETER))
    {
        // Check if the variable was misused, i.e, with index
        if (astNode->children[0])
        {
            fprintf(stderr, "Error: variable '%s' is not a vector!\n", astNode->hashNode->symbol);
            semanticErrors++;
            return DATATYPE_UNTYPED;
        }
        else
        {
            // Return the variable type
            return astNode->hashNode->dataType;
        }
    }
    else if (astNode->hashNode->symbolType == SYMBOL_GLOBAL_VECTOR)
    {
        // Check if the variable was misused, i.e, without index
        if (!astNode->children[0])
        {
            fprintf(stderr, "Error: vector '%s' used without index!\n", astNode->hashNode->symbol);
            semanticErrors++;
            return DATATYPE_UNTYPED;
        }
        else
        {
            // Check index type
            // TODO: check array bounds?
            if (checkDataTypeCompatibility(DATATYPE_INT, checkTypes(astNode->children[0])) != DATATYPE_INT)
            {
                fprintf(stderr, "Error: index of vector '%s' has invalid data type!\n", astNode->hashNode->symbol);
                semanticErrors++;
                return DATATYPE_UNTYPED;
            }
            else
            {
                // Return the variable type
                return astNode->hashNode->dataType;
            }
        }
    }
}

int checkFunctionCall(AST_NODE * astNode)
{
    AST_NODE * currentArgument = astNode->children[0];
    AST_NODE * functionDefinition = (AST_NODE *) astNode->hashNode->functionLink;
    
    //test the function link, if this link does not exist, the function called is not defined.
    if(functionDefinition)
    {
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
    else
    {
            fprintf(stderr, "Error: the function '%s'! is not defined!\n", astNode->hashNode->symbol);
            semanticErrors++;
            return DATATYPE_INCOMPATIBLE;
    }
    
}

void checkAttribution(AST_NODE * astNode)
{
    AST_NODE * destinationNode = astNode->children[0];

    // Check expression data type
    if (checkDataTypeCompatibility(checkVariable(destinationNode), checkTypes(astNode->children[1])) == DATATYPE_INCOMPATIBLE)
    {
        fprintf(stderr, "Error: variable '%s' attribution has invalid data type!\n", destinationNode->hashNode->symbol);
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
    else if (dataType1 == DATATYPE_EXPRESSION)
    {
        switch (dataType2)
        {
            case DATATYPE_INT:
            case DATATYPE_CHAR:
            case DATATYPE_BOOL:
            case DATATYPE_REAL:
                return DATATYPE_EXPRESSION;
            default:
                return DATATYPE_INCOMPATIBLE;
        }
    }
    else
    {
        return DATATYPE_INCOMPATIBLE;
    }
}
