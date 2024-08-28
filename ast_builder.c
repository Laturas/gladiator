#ifndef INCLUDES
#include "ALL_INCLUDES"
#endif

#define rawptr void*
typedef struct AST {
    rawptr start;
    rawptr end;
} AST;

// FUNCTION NODE SYNTAX
// [type] -> [Params] -> [Body]

struct FunctionList {
    u64 length;
    Function* list;
};

AST* build_tree(arena ast_arena, AtomList* atom_list) {
    enum NextExpectation {
        NONE, // No idea what's happening
        FUNCTION,
        OTHER,
    };

    arena dynamic_list_arena = aalloc(8388608);

    AST* syntree;
    struct FunctionList* func_ls;
    {
        struct FunctionList _fls = {0};
        AST _syntree = {0};
        syntree = apush(ast_arena, _syntree);
        func_ls = apush(dynamic_list_arena, _fls);
        //func_ls->list = func_ls + sizeof(struct FunctionList);
    }
    enum NextExpectation expect = NONE;
    Atom* atoms = atom_list->list;
    Atom* last = atom_list->listlen + atom_list->list;
    _String strdata = {0};

    while (atoms < last) {
        Atom* current_atom = atoms++;
        Token new_token = current_atom->token;
        switch (new_token) {
            case CUSTOM: 
                if (expect == NONE) {
                    expect = FUNCTION;
                    strdata = current_atom->extra_str;
                    break;
                }
                printf("Parsing error on line %d: Unexpected token: ", current_atom->line_number); print_atom(*current_atom, 0); exit(1);
                break;
            case OPEN_PAREN:
                if (expect == FUNCTION) {
                    Function* return_func = function_block(ast_arena, &strdata, &atoms);
                    if (func_ls->list) func_ls->list = return_func ; else {apush(dynamic_list_arena, return_func); func_ls->length++;}
                    
                    expect = NONE;
                }
                break;
            default: break;
        }
    }
    afree(dynamic_list_arena);
}