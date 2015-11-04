#ifndef TAC_HEADER
#define TAC_HEADER

#include "hash.h"


#define TAC_SYMBOL 	0
#define TAC_SUM     1
#define TAC_SUB   	2
#define TAC_MULT    3
#define TAC_DIV		4
#define TAC_LT 		5
#define TAC_GT 		6
#define TAC_LET 	7
#define TAC_GET 	8
#define TAC_EQ 		9
#define TAC_NE 		10
#define TAC_AND 	11
#define TAC_OR 		12

#define TAC_MOV		24
#define TAC_READ	25
#define TAC_PRINT	26
#define TAC_RETURN	27
#define

////////////////////////////

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

#define AST_OUTPUT_LIST		28
#define AST_IF 				29
#define AST_IFELSE 			30
#define AST_LOOP 			31

#define COMMA_SEPARATOR		100
#define COLON_SEPARATOR		101
#define NO_SEPARATOR		102
#define VECTOR_INDEX		103
#define VECTOR_INIT			104

typedef struct tac_node
{
	int type;
	HASH_NODE* res;
	HASH_NODE* op1;
	HASH_NODE* op2;
	struct tac_node* prev;
	struct tac_node* next;
} TAC;

HASH_NODE * makeLabel();
HASH_NODE * makeTemp();
TAC* tacCreate(int type, HASH_NODE* res, HASH_NODE* op1, HASH_NODE* op2);
void tacPrint(TAC *tac);
TAC* tacJoin(TAC* tacList1, TAC* tacList2);
TAC* generateTacs(AST* astNode);
TAC* generateIfThen(TAC* booleanExpression, TAC* code);
TAC* generateIfThenElse(TAC* booleanExpression, TAC* codeTrue, TAC* codeFalse);
TAC* generateBinaryOperation(int tacType, TAC* child0, TAC* child1);

#endif
