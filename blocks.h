#ifndef INCLUDES
#include "ALL_INCLUDES"
#endif

/**
 * All blocks have an identifier at the front in the form of a BlockType enum.
 */
typedef enum BlockType {
    VOIDBLOCK,
    OTHER,
    EXPRESSION,
    FUNCTION,
    IDENTIFIER,
    LITERAL,
    RETURNBLOCK,
    VARIABLE,
} BlockType;

void print_block_type (BlockType type) {
    switch (type) {
        case VOIDBLOCK: printf("Void\n"); break;
        case OTHER: printf("Other\n"); break;
        case EXPRESSION: printf("Expression\n"); break;
        case FUNCTION: printf("Function\n"); break;
        case IDENTIFIER:printf("Identifier\n"); break;
        case LITERAL: printf("Literal\n"); break;
        case RETURNBLOCK: printf("Return\n"); break;
        case VARIABLE: printf("Var\n"); break;
        default: printf("Wtf\n"); break;
    }
    fflush(stdout);
}

/**
 * This is used for the return types of functions and such
 */
typedef struct BLOCK_Type {
    BlockType type;
    VarType var_type;
} Scope;

typedef struct BLOCK_Return {
    BlockType type;

    BlockType return_value; // Returns are a unary block for now so we can just stick it onto the end.
} Return;

typedef struct BLOCK_Variable {
    BlockType type;
    _String name;
    VarType variable_type;
} Variable;

typedef struct BLOCK_Function {
    BlockType type;
    string name;
    Variable* params; // Single params only right now
    BlockType returns; // Single returns only right now
    BlockType body;
} Function;

typedef struct BLOCK_Expression {
    BlockType type;

    BlockType* first_item;
    BlockType* end_item;
} Expression;

typedef struct BLOCK_Void {
    BlockType type;
} Voidblock;

//typedef struct BLOCK_FunctionParams {
//    BlockType type;
//
//    u64 param_count;
//    Variable* variable_list;
//} FunctionParams;

#include "blocks.c"