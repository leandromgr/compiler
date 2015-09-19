#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include "hash.h"

#define MAX_CHILDREN 4

#define AST_SYMBOL 	0
#define AST_SUM     1
#define AST_SUB   	2
#define AST_MULT    3
#define AST_DIV		4
#define AST_IF 		5
#define AST_IFELSE 	16
#define AST_LOOP 	17
#define AST_ATTRIBUTION	6
#define AST_LT 		7
#define AST_GT 		8
#define AST_LET 	9
#define AST_GET 	10
#define AST_EQ 		11
#define AST_NE 		12
#define AST_AND 	13
#define AST_OR 		14
#define AST_FUNCALL	15

//Friday
#define AST_GLOBAL_VAR_LIST 18

#define AST_GLOBAL_VECTOR 19

#define AST_INT		21
#define AST_CHAR 	22
#define AST_BOOL	23
#define AST_REAL	24

//Saturday
#define AST_FUNCTION_LIST	25
#define AST_FUNCTION		26
#define AST_PARAMETER_LIST	27

#define AST_LOCAL_VAR_LIST 28


typedef struct ast_node
{
    int type;
    HASH_NODE *hashNode;
    struct ast_node *children[MAX_CHILDREN];
} AST_NODE;

AST_NODE * astCreate(int type, HASH_NODE * hashNode, AST_NODE * child0, AST_NODE * child1, AST_NODE * child2, AST_NODE * child3);

void astPrint(AST_NODE * node, int currentLevel);

#endif // AST_H
