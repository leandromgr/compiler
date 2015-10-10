
#include "semantic.h"

// Count the semantic errors
int semanticErrors = 0;

void setDeclarations(AST_NODE *astNode)
{
    if (!astNode)
        return;

    // Check if it is either a new function variable or a global variable
    if (astNode->type == AST_LOCAL_VAR_LIST || astNode->type == AST_GLOBAL_VAR_LIST)
    {
        if (astNode->hashNode)
        {
            // Check for mul
            if
        }
    }


}
