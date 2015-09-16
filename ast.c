
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
        case AST_SUM:   fprintf(stderr, "SUM"); break;
        case AST_SUB:   fprintf(stderr, "SUB"); break;
        case AST_MULT:  fprintf(stderr, "MULT"); break;
        case AST_DIV:   fprintf(stderr, "DIV"); break;
        case AST_TKID:  fprintf(stderr, "TKID"); break;
        case AST_ASST:  fprintf(stderr, "AST_ASST"); break;

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
}
