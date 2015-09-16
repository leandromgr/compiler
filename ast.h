#ifndef AST_H
#define AST_H

#include "hash.h"
#include <stdlib.h>

#define MAX_CHILDREN 4

#define AST_INTEGER 0
#define AST_TRUE    1
#define AST_FALSE   2
#define AST_CHAR    3
#define AST_SUM     4
#define AST_SUB     5
#define AST_MULT    6
#define AST_DIV     7
#define AST_IF      8
#define AST_ASST    9
#define AST_TKID    10

typedef struct ast_node
{
    int type;
    HASH_NODE *hashNode;
    struct ast_node *children[MAX_CHILDREN];
} AST_NODE;

AST_NODE * astCreate(int type, HASH_NODE * hashNode, AST_NODE * child0, AST_NODE * child1, AST_NODE * child2, AST_NODE * child3);

void astPrint(AST_NODE * node, int currentLevel);

#endif // AST_H
