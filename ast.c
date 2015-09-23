
#include "ast.h"
#include "y.tab.h"

AST_NODE * astCreate(int type, HASH_NODE *hashNode, AST_NODE *child0, AST_NODE *child1, AST_NODE *child2, AST_NODE *child3)
{
    AST_NODE * newNode;

    newNode = (AST_NODE *) calloc(1, sizeof(AST_NODE));
    newNode->type = type;
    newNode->hashNode = hashNode;
    newNode->children[0] = child0;
    newNode->children[1] = child1;
    newNode->children[2] = child2;
    newNode->children[3] = child3;

    return newNode;
}

void astPrint(AST_NODE *node, int currentLevel)
{
    int indexLevel;

    if (!node)
        return;

    for (indexLevel = 0; indexLevel < currentLevel; indexLevel++)
    {
        fprintf(stderr, "  ");
    }

    fprintf(stderr, "AST(");

    switch(node->type)
    {
        case AST_SUM:       fprintf(stderr, "SUM"); break;
        case AST_SUB:       fprintf(stderr, "SUB"); break;
        case AST_MULT:      fprintf(stderr, "MULT"); break;
        case AST_DIV:       fprintf(stderr, "DIV"); break;
        case AST_SYMBOL:    fprintf(stderr, "SYMBOL"); break;
        case AST_LT:        fprintf(stderr, "LT"); break;
        case AST_GT:        fprintf(stderr, "GT"); break;
        case AST_LET:       fprintf(stderr, "LET"); break;
        case AST_GET:       fprintf(stderr, "GET"); break;
        case AST_EQ:        fprintf(stderr, "EQ"); break;
        case AST_NE:        fprintf(stderr, "NE"); break;
        case AST_AND:       fprintf(stderr, "AND"); break;
        case AST_OR:        fprintf(stderr, "OR"); break;
                
        //Friday
        case AST_GLOBAL_VAR_LIST:   fprintf(stderr, "GLOBAL_VAR_LIST"); break;
        case AST_GLOBAL_VECTOR:     fprintf(stderr, "GLOBAL_VECTOR"); break;
        case AST_INT:       fprintf(stderr, "INT"); break;
        case AST_CHAR:      fprintf(stderr, "CHAR"); break;
        case AST_BOOL:      fprintf(stderr, "BOOL"); break;
        case AST_REAL:      fprintf(stderr, "REAL"); break;

        //Saturday
        case AST_FUNCTION:          fprintf(stderr, "FUNCTION"); break;
        case AST_FUNCALL:           fprintf(stderr, "AST_FUNCALL"); break;
        case AST_FUNCTION_LIST:     fprintf(stderr, "FUNCTION_LIST"); break;
        case AST_PARAMETER_LIST:    fprintf(stderr, "PARAMETER_LIST"); break;
        case AST_LOCAL_VAR_LIST:    fprintf(stderr, "LOCAL_VAR_LIST"); break;
        case AST_INPUT_CMD:         fprintf(stderr, "INPUT_CMD"); break;
        case AST_OUTPUT_CMD:        fprintf(stderr, "OUTPUT_CMD"); break;
        case AST_OUTPUT_LIST:       fprintf(stderr, "OUTPUT_LIST"); break;
        case AST_RETURN_CMD:        fprintf(stderr, "RETURN_CMD"); break;
        case AST_ATTRIBUTION:       fprintf(stderr, "ATTRIBUTION"); break;
        case AST_IF:                fprintf(stderr, "IF"); break;
        case AST_IFELSE:            fprintf(stderr, "IFELSE"); break;
        case AST_LOOP:              fprintf(stderr, "IFLOOP"); break;
        case AST_CMD_LIST:          fprintf(stderr, "CMD_LIST"); break;



    }
    if (node->hashNode)
    {
        if (node->hashNode->symbol)
        {
            fprintf(stderr, ", %s", node->hashNode->symbol);
        }
    }
    fprintf(stderr, ")\n");

    int indexChild;
    for (indexChild = 0; indexChild < MAX_CHILDREN; indexChild++)
        astPrint(node->children[indexChild], currentLevel+1);
}

void descompileTree(AST_NODE *node)
{
    if (!node)
        return;

    switch(node->type)
    {
        case AST_SUM: 
            printArithmeticExpression(node, "+");
            break;
        case AST_SUB:       
            printArithmeticExpression(node, "-");
            break;
        case AST_MULT:     
            printArithmeticExpression(node, "*");
            break;
        case AST_DIV:      
            printArithmeticExpression(node, "/");
            break;
        case AST_LT:      
            printArithmeticExpression(node, "<");
            break;
        case AST_GT:      
            printArithmeticExpression(node, ">");
            break;
        case AST_LET:      
            printArithmeticExpression(node, "<=");
            break;
        case AST_GET:      
            printArithmeticExpression(node, ">=");
            break;
        case AST_EQ:      
            printArithmeticExpression(node, "==");
            break;
        case AST_NE:      
            printArithmeticExpression(node, "!=");
            break;
        case AST_AND:      
            printArithmeticExpression(node, "&&");
            break;
        case AST_OR:      
            printArithmeticExpression(node, "||");
            break;
        
        case AST_FUNCALL:
            if (node->hashNode->symbol)
                fprintf(DEST_FILE, "%s(", node->hashNode->symbol);
            parseSymbolPrint(node->children[0], COMMA_SEPARATOR);
            fprintf(DEST_FILE, ")");
            break;

        case AST_FUNCTION_LIST:
            descompileTree(node->children[0]);  // Current function
            if (node->children[1])
            {
                fprintf(DEST_FILE, ";\n\n");
                descompileTree(node->children[1]);  // Next function
            }
            break;

        case AST_FUNCTION:
            descompileTree(node->children[0]);  // Function header
            descompileTree(node->children[1]);  // Local variable list
            parseCommandList(node->children[2]);
            break;

        case AST_INT:
            if (node->hashNode->symbol)
                fprintf(DEST_FILE, "int %s", node->hashNode->symbol);

            if (node->children[0])
            {
                if (node->children[0]->type == AST_SYMBOL)
                    parseSymbolPrint(node->children[0], COLON_SEPARATOR);
                else if(node->children[0]->type == AST_PARAMETER_LIST)
                {
                    fprintf(DEST_FILE, "(");
                    descompileTree(node->children[0]);
                    fprintf(DEST_FILE, ")\n");
                }
                else
                    descompileTree(node->children[0]);
            }
            break;

        case AST_CHAR:
            if (node->hashNode->symbol)
                fprintf(DEST_FILE, "char %s", node->hashNode->symbol);
                        
            if (node->children[0])
            {
                if (node->children[0]->type == AST_SYMBOL)
                    parseSymbolPrint(node->children[0], COLON_SEPARATOR);
                else if(node->children[0]->type == AST_PARAMETER_LIST)
                {
                    fprintf(DEST_FILE, "(");
                    descompileTree(node->children[0]);
                    fprintf(DEST_FILE, ")\n");
                }
                else
                    descompileTree(node->children[0]);
            }
            break;
        
        case AST_BOOL:
            if (node->hashNode->symbol)
                fprintf(DEST_FILE, "bool %s", node->hashNode->symbol);
                        
            if (node->children[0])
            {
                if (node->children[0]->type == AST_SYMBOL)
                    parseSymbolPrint(node->children[0], COLON_SEPARATOR);
                else if(node->children[0]->type == AST_PARAMETER_LIST)
                {
                    fprintf(DEST_FILE, "(");
                    descompileTree(node->children[0]);
                    fprintf(DEST_FILE, ")\n");
                }
                else
                    descompileTree(node->children[0]);
            }
            break;

        case AST_REAL:
            if (node->hashNode->symbol)
                fprintf(DEST_FILE, "real %s", node->hashNode->symbol);
                        
            if (node->children[0])
            {
                if (node->children[0]->type == AST_SYMBOL)
                    parseSymbolPrint(node->children[0], COLON_SEPARATOR);
                else if(node->children[0]->type == AST_PARAMETER_LIST)
                {
                    fprintf(DEST_FILE, "(");
                    descompileTree(node->children[0]);
                    fprintf(DEST_FILE, ")\n");
                }
                else
                    descompileTree(node->children[0]);
            }
            break;
        
        case AST_PARAMETER_LIST:
            descompileTree(node->children[0]);  // Current parameter
            if (node->children[1])
            {
                fprintf(DEST_FILE, ", ");
                descompileTree(node->children[1]);  // Next parameter
            }
            break;

        case AST_LOCAL_VAR_LIST:
            descompileTree(node->children[0]);  // Current variable
            fprintf(DEST_FILE, ";\n");
            descompileTree(node->children[1]);  // Next variable
            break;

        case AST_CMD_LIST:
            parseCommandList(node->children[0]);  // Current command
            
            if (node->children[1])
            {
                fprintf(DEST_FILE, ";\n");
                descompileTree(node->children[1]);  // Next command
            }
            break;

        case AST_INPUT_CMD:
            if (node->hashNode->symbol)
                fprintf(DEST_FILE, "input %s", node->hashNode->symbol);
            break;
        
        case AST_OUTPUT_CMD:
            fprintf(DEST_FILE, "output ");
            descompileTree(node->children[0]);
            break;
        
        case AST_OUTPUT_LIST:
            descompileTree(node->children[0]);
            if (node->children[1])
            {
                fprintf(DEST_FILE, ", ");
                descompileTree(node->children[1]);  // Next command
            }
            break;

        case AST_RETURN_CMD:
            fprintf(DEST_FILE, "return ");
            descompileTree(node->children[0]);
            break;

        case AST_ATTRIBUTION:
            parseSymbolPrint(node->children[0], NO_SEPARATOR);
            fprintf(DEST_FILE, " := ");
            descompileTree(node->children[1]);
            break;

        case AST_IF:
            fprintf(DEST_FILE, "if (");
            descompileTree(node->children[0]);
            fprintf(DEST_FILE, ")\n");
            parseCommandList(node->children[1]);
            break;

        case AST_IFELSE:
            fprintf(DEST_FILE, "if (");
            descompileTree(node->children[0]);
            fprintf(DEST_FILE, ")\n");
            parseCommandList(node->children[1]);
            fprintf(DEST_FILE, "\nelse\n");
            parseCommandList(node->children[2]);
            break;

        case AST_LOOP:
            fprintf(DEST_FILE, "if (");
            descompileTree(node->children[0]);
            fprintf(DEST_FILE, ")\n");
            parseCommandList(node->children[1]);
            fprintf(DEST_FILE, "\nloop");
            break;

        case AST_GLOBAL_VAR_LIST:
            descompileTree(node->children[0]);
            fprintf(DEST_FILE, ";\n");
            descompileTree(node->children[1]);
            break;

        case AST_GLOBAL_VECTOR:
            if (node->hashNode->symbol)
                fprintf(DEST_FILE, "[%s]", node->hashNode->symbol);

            parseSymbolPrint(node->children[0], COLON_SEPARATOR);
            break;

        case AST_SYMBOL:
            parseSymbolPrint(node, NO_SEPARATOR);
            break;

        default:
            break;
    }
}

void printArithmeticExpression(AST_NODE * currentNode, char * arithmeticSymbol)
{
    if (!currentNode)
        return;

    fprintf(DEST_FILE, "(");
    descompileTree(currentNode->children[0]); 
    fprintf(DEST_FILE, " %s ", arithmeticSymbol);
    descompileTree(currentNode->children[1]);
    fprintf(DEST_FILE, ")");
}

void parseCommandList(AST_NODE* firstCmdList)
{
    if (!firstCmdList)
        return;

    if(firstCmdList->type == AST_CMD_LIST)
    {
        fprintf(DEST_FILE, "{\n");
        descompileTree(firstCmdList);  // Commands
        fprintf(DEST_FILE, "\n}");
    }
    else
        descompileTree(firstCmdList);  // Command

}

void printSymbol(HASH_NODE *symbolHashNode)
{
    if(!symbolHashNode)
        return;

    switch(symbolHashNode->symbolType)
    {
        case LIT_CHAR: 
            fprintf(DEST_FILE, "'%s'", symbolHashNode->symbol);
            break;

        case LIT_STRING: 
            fprintf(DEST_FILE, "\"%s\"", symbolHashNode->symbol);
            break;

        default: 
            fprintf(DEST_FILE, "%s", symbolHashNode->symbol);
            break;

    }

}

void parseSymbolPrint(AST_NODE * symbolNode, int processingCode)
{
    if (!symbolNode)
        return;

    switch(processingCode)
    {
        case COMMA_SEPARATOR:
            printSymbol(symbolNode->hashNode);
            
            if (symbolNode->children[0])
            {
                fprintf(DEST_FILE, ", ");
                parseSymbolPrint(symbolNode->children[0], processingCode);
            }
            break;

        case COLON_SEPARATOR:
            fprintf(DEST_FILE, ": ");
            printSymbol(symbolNode->hashNode);

            if (symbolNode->children[0])
            {
                parseSymbolPrint(symbolNode->children[0], VECTOR_INIT);
            }
            break;

        case VECTOR_INIT:
            fprintf(DEST_FILE, " ");
            printSymbol(symbolNode->hashNode);

            if (symbolNode->children[0])
            {
                parseSymbolPrint(symbolNode->children[0], VECTOR_INIT);
            }
            break;

        case NO_SEPARATOR:
            printSymbol(symbolNode->hashNode);
            
            if (symbolNode->children[0])
            {
                parseSymbolPrint(symbolNode->children[0], VECTOR_INDEX);
            }
            break;

        case VECTOR_INDEX:
            fprintf(DEST_FILE, "[");
            descompileTree(symbolNode);
            fprintf(DEST_FILE, "]");
            break;

        default:
            break;
    }
}