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
#define AST_LT 		5
#define AST_GT 		6
#define AST_LET 	7
#define AST_GET 	8
#define AST_EQ 		9
#define AST_NE 		10
#define AST_AND 	11
#define AST_OR 		12
#define AST_FUNCALL	13

#define AST_GLOBAL_VAR_LIST 14
#define AST_GLOBAL_VECTOR 15

#define AST_INT		16
#define AST_CHAR 	17
#define AST_BOOL	18
#define AST_REAL	19

#define AST_FUNCTION_LIST	20
#define AST_FUNCTION		21
#define AST_PARAMETER_LIST	22
#define AST_LOCAL_VAR_LIST	23

#define AST_ATTRIBUTION		24
#define AST_INPUT_CMD		25
#define AST_OUTPUT_CMD		26
#define AST_RETURN_CMD		27
#define AST_OUTPUT_LIST		28
#define AST_IF 				29
#define AST_IFELSE 			30
#define AST_LOOP 			31
#define AST_CMD_LIST		32

#define COMMA_SEPARATOR		100
#define COLON_SEPARATOR		101
#define NO_SEPARATOR		102
#define VECTOR_INDEX		103
#define VECTOR_INIT			104

typedef struct ast_node
{
    int type;
    HASH_NODE *hashNode;
    struct ast_node *children[MAX_CHILDREN];
} AST_NODE;

AST_NODE * astTree;

AST_NODE * astCreate(int type, HASH_NODE * hashNode, AST_NODE * child0, AST_NODE * child1, AST_NODE * child2, AST_NODE * child3);

void astPrint(AST_NODE * node, int currentLevel);

void descompileTree(AST_NODE *node);

void parseSymbolPrint(AST_NODE * symbolNode, int processingCode);

#endif // AST_H
