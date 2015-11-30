#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "hash.h"
#include <stdio.h>

#define DATATYPE_INT            0
#define DATATYPE_CHAR           1
#define DATATYPE_BOOL           2
#define DATATYPE_REAL           3
#define DATATYPE_EXPRESSION     4   // Used only to validate IF-ELSE-LOOP
#define DATATYPE_UNTYPED        -1
#define DATATYPE_INCOMPATIBLE   -2


#define SYMBOL_LOCAL_VARIABLE       1000
#define SYMBOL_GLOBAL_VARIABLE      1001
#define SYMBOL_GLOBAL_VECTOR        1002
#define SYMBOL_FUNCTION             1003
#define SYMBOL_FUNCTION_PARAMETER   1004


extern int semanticErrors;

void setDeclarations(AST_NODE * astNode);
void checkUndeclared();
int checkTypes(AST_NODE * astNode);
void checkAttribution(AST_NODE * astNode);
int checkFunctionCall(AST_NODE * astNode);
int checkVariable(AST_NODE * astNode);
int checkOutputCommand(AST_NODE * astNode);

char * printDataType(int dataType);
int checkDataTypeCompatibility(int dataType1, int dataType2);

#endif // SEMANTIC_H
