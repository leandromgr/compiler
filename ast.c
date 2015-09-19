
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
        case AST_ATTRIBUTION:  fprintf(stderr, "AST_ATTRIBUTION"); break;
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
        case AST_FUNCTION_LIST:     fprintf(stderr, "FUNCTION_LIST"); break;
        case AST_PARAMETER_LIST:    fprintf(stderr, "PARAMETER_LIST"); break;

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
