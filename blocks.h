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


typedef struct BLOCK_Function {
    BlockType type;

    string name;
    BlockType* params; // Single params only right now
    BlockType returns; // Single returns only right now
    BlockType body;
} Function;

/**
 * This is used for the return types of functions and such
 */
typedef struct BLOCK_TypeInfo {
    BlockType type;

    VarType var_type;
} Scope;

typedef struct BLOCK_Return {
    BlockType type;

    BlockType return_value; // Returns are a unary block for now so we can just stick it onto the end.
} Return;

typedef struct BLOCK_Variable {
    BlockType type;

    string name;
    VarType variable_type;
} Variable;

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

BlockType* parse_params(arena block_arena, Atom** atom_list, u64 item_access) {
    Atom current_atom;
    Variable current_var = {VARIABLE, NULL, TYPE_VOID};
    int colon_encountered = 0;
    
    //printf("%p", (*atom_list)[0]); fflush(stdout);
    //printf("%d", ((current_atom = (*atom_list)[0]).token != CLOSE_PAREN)); fflush(stdout);
    while ((current_atom = (*atom_list)[item_access]).token != CLOSE_PAREN) {
        switch (current_atom.token) {
            case CUSTOM: {
                printf("Custom\n"); fflush(stdout);
                if (colon_encountered) {
                    printf("Parsing error on line %d: Found second identifier when type was expected\n", current_atom.line_number);
                    exit(1);
                }
                if (current_var.name) {
                    printf("Parsing error on line %d: conflicting variable identifiers:\n", current_atom.line_number);
                    print(current_var.name); printf(" vs "); println(current_atom.extra_data);
                    exit(1);
                }
                current_var.name = current_atom.extra_data;
                break;
            }
            case COLON: {
                printf("Colon\n"); fflush(stdout);
                if (colon_encountered) {
                    printf("Parsing error on line %d: Extra ':' separator detected. Only one ':' is needed between the identifier and type of a variable\n", current_atom.line_number);
                    exit(1);
                }
                colon_encountered = 1;
                break;
            }
            case I32: {
                if (!colon_encountered) {
                    printf("Parsing error on line %d: No ':' separator detected between variable name and type\n", current_atom.line_number); fflush(stdout);
                    exit(1);
                }
                colon_encountered = 0;
                break;
            }
            case CLOSE_PAREN: {
                printf("Closed!"); fflush(stdout);
                if (current_var.variable_type == TYPE_VOID && !current_var.name) {
                    Voidblock empty = {0};
                    return apush(block_arena, empty);
                } else {
                    if (current_var.name) {
                        printf("Parsing error on line %d: No type specified", current_atom.line_number);
                    }
                }
            }
            default: {
                printf("Parsing error on line %d: Unexpected token\n", current_atom.line_number); fflush(stdout);
                exit(1);
            }
        }
        item_access++;
    }
}

Function* function_block(arena block_arena, string name, Atom** atom_list) {
    Function* new_func = NULL;
    {
        Function _new_func = {FUNCTION, name, 0, 0, 0};
        new_func = apush(block_arena, _new_func);
    }
    new_func->params = parse_params(block_arena, atom_list, 2); // Hardcoded
}
