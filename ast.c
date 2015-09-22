
#include "ast.h"

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
            descompileTree(node->children[0]); 
            fprintf(stderr, " + ");
            descompileTree(node->children[1]);
            break;
        case AST_SUB:       
            descompileTree(node->children[0]); 
            fprintf(stderr, " - ");
            descompileTree(node->children[1]);
            break;
        case AST_MULT:     
            descompileTree(node->children[0]); 
            fprintf(stderr, " * ");
            descompileTree(node->children[1]);
            break;
        case AST_DIV:      
            descompileTree(node->children[0]); 
            fprintf(stderr, " / ");
            descompileTree(node->children[1]);
            break;
        case AST_LT:      
            descompileTree(node->children[0]); 
            fprintf(stderr, " < ");
            descompileTree(node->children[1]);
            break;
        case AST_GT:      
            descompileTree(node->children[0]); 
            fprintf(stderr, " > ");
            descompileTree(node->children[1]);
            break;
        case AST_LET:      
            descompileTree(node->children[0]); 
            fprintf(stderr, " <= ");
            descompileTree(node->children[1]);
            break;
        case AST_GET:      
            descompileTree(node->children[0]); 
            fprintf(stderr, " >= ");
            descompileTree(node->children[1]);
            break;
        case AST_EQ:      
            descompileTree(node->children[0]); 
            fprintf(stderr, " == ");
            descompileTree(node->children[1]);
            break;
        case AST_NE:      
            descompileTree(node->children[0]); 
            fprintf(stderr, " != ");
            descompileTree(node->children[1]);
            break;
        case AST_AND:      
            descompileTree(node->children[0]); 
            fprintf(stderr, " && ");
            descompileTree(node->children[1]);
            break;
        case AST_OR:      
            descompileTree(node->children[0]); 
            fprintf(stderr, " || ");
            descompileTree(node->children[1]);
            break;
        
        case AST_FUNCALL:
            if (node->hashNode->symbol)
                fprintf(stderr, "%s(", node->hashNode->symbol);
            parseSymbolPrint(node->children[0], COMMA_SEPARATOR);
            fprintf(stderr, ")\n");
            break;

        case AST_FUNCTION_LIST:
            descompileTree(node->children[0]);  // Current function
            if (node->children[1])
            {
                fprintf(stderr, ";\n\n");
                descompileTree(node->children[1]);  // Next function
            }
            break;

        case AST_FUNCTION:
            descompileTree(node->children[0]);  // Function header
            descompileTree(node->children[1]);  // Local variable list
            descompileTree(node->children[2]);  // Commands
            break;

        case AST_INT:
            if (node->hashNode->symbol)
                fprintf(stderr, "int %s", node->hashNode->symbol);

            if (node->children[0])
            {
                if (node->children[0]->type == AST_SYMBOL)
                    parseSymbolPrint(node->children[0], COLON_SEPARATOR);
                else
                    descompileTree(node->children[0]);
            }
            break;
        case AST_CHAR:
            if (node->hashNode->symbol)
                fprintf(stderr, "char %s", node->hashNode->symbol);
                        
            if (node->children[0])
            {
                if (node->children[0]->type == AST_SYMBOL)
                    parseSymbolPrint(node->children[0], COLON_SEPARATOR);
                else
                    descompileTree(node->children[0]);
            }
            break;
        
        case AST_BOOL:
            if (node->hashNode->symbol)
                fprintf(stderr, "bool %s", node->hashNode->symbol);
                        
            if (node->children[0])
            {
                if (node->children[0]->type == AST_SYMBOL)
                    parseSymbolPrint(node->children[0], COLON_SEPARATOR);
                else
                    descompileTree(node->children[0]);
            }
            break;

        case AST_REAL:
            if (node->hashNode->symbol)
                fprintf(stderr, "real %s", node->hashNode->symbol);
                        
            if (node->children[0])
            {
                if (node->children[0]->type == AST_SYMBOL)
                    parseSymbolPrint(node->children[0], COLON_SEPARATOR);
                else
                    descompileTree(node->children[0]);
            }
            break;
        
        case AST_PARAMETER_LIST:
            descompileTree(node->children[0]);  // Current parameter
            if (node->children[1])
            {
                fprintf(stderr, ", ");
                descompileTree(node->children[1]);  // Next parameter
            }
            break;

        case AST_LOCAL_VAR_LIST:
            descompileTree(node->children[0]);  // Current variable
            if (node->children[1])
            {
                fprintf(stderr, ";\n");
                descompileTree(node->children[1]);  // Next variable
            }
            break;

        case AST_CMD_LIST:
            if (node->children[0])
            {
                if (node->children[0]->type == AST_CMD_LIST)
                {
                    fprintf(stderr, "{\n");
                    descompileTree(node->children[0]);  // Current command
                    fprintf(stderr, "}\n");
                }
                else
                    descompileTree(node->children[0]);  // Current command
            }
            
            if (node->children[1])
            {
                fprintf(stderr, ";\n");
                descompileTree(node->children[1]);  // Next command
            }
            break;

        case AST_INPUT_CMD:
            if (node->hashNode->symbol)
                fprintf(stderr, "input %s", node->hashNode->symbol);
            break;
        
        case AST_OUTPUT_CMD:
            fprintf(stderr, "output ");
            descompileTree(node->children[0]);
            break;
        
        case AST_OUTPUT_LIST:
            descompileTree(node->children[0]);
            if (node->children[1])
            {
                fprintf(stderr, ", ");
                descompileTree(node->children[1]);  // Next command
            }
            break;

        case AST_RETURN_CMD:
            fprintf(stderr, "return ");
            descompileTree(node->children[0]);
            break;

        case AST_ATTRIBUTION:
            parseSymbolPrint(node->children[0], NO_SEPARATOR);
            fprintf(stderr, " := ");
            descompileTree(node->children[1]);
            break;

        case AST_IF:
            fprintf(stderr, "if (");
            descompileTree(node->children[0]);
            fprintf(stderr, ")\n");
            descompileTree(node->children[1]);
            break;

        case AST_IFELSE:
            fprintf(stderr, "if (");
            descompileTree(node->children[0]);
            fprintf(stderr, ")\n");
            descompileTree(node->children[1]);
            fprintf(stderr, "\nelse\n");
            descompileTree(node->children[2]);
            break;

        case AST_LOOP:
            fprintf(stderr, "if (");
            descompileTree(node->children[0]);
            fprintf(stderr, ")\n");
            descompileTree(node->children[1]);
            fprintf(stderr, "\nloop");
            break;

        case AST_GLOBAL_VAR_LIST:
            descompileTree(node->children[0]);
            if (node->children[1])
            {
                fprintf(stderr, ";\n");
                descompileTree(node->children[1]);
            }
            break;

        case AST_GLOBAL_VECTOR:
            if (node->hashNode->symbol)
                fprintf(stderr, "[%s]", node->hashNode->symbol);

            parseSymbolPrint(node->children[0], COLON_SEPARATOR);
            break;

        case AST_SYMBOL:
            parseSymbolPrint(node, NO_SEPARATOR);
            break;

        default:
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
            if (symbolNode->hashNode->symbol)
                fprintf(stderr, "%s", symbolNode->hashNode->symbol);
            
            if (symbolNode->children[0])
            {
                fprintf(stderr, ", ");
                parseSymbolPrint(symbolNode->children[0], processingCode);
            }
            break;

        case COLON_SEPARATOR:
            if (symbolNode->hashNode->symbol)
                fprintf(stderr, ": %s", symbolNode->hashNode->symbol);

            if (symbolNode->children[0])
            {
                parseSymbolPrint(symbolNode->children[0], VECTOR_INIT);
            }
            break;

        case VECTOR_INIT:
            if (symbolNode->hashNode->symbol)
                fprintf(stderr, " %s", symbolNode->hashNode->symbol);

            if (symbolNode->children[0])
            {
                parseSymbolPrint(symbolNode->children[0], VECTOR_INIT);
            }
            break;

        case NO_SEPARATOR:
            if (symbolNode->hashNode->symbol)
                fprintf(stderr, "%s", symbolNode->hashNode->symbol);
            
            if (symbolNode->children[0])
            {
                parseSymbolPrint(symbolNode->children[0], VECTOR_INDEX);
            }
            break;

        case VECTOR_INDEX:
            fprintf(stderr, "[");
            descompileTree(symbolNode);
            fprintf(stderr, "]");
            break;

        default:
            break;
    }
}