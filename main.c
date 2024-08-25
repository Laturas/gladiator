#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "./libraries/debug.c"
#include "./libraries/safe_strings.c"
#include "./libraries/arenas.h"

#include "atomizer.h"

/**
* Dumps the contents of a file into a char*
*/
string map_file_to_memory(arena aren, char* file) {
    FILE* src_file = fopen(file, "r");
    if (!src_file) {debug_print("PARSER ERROR: Source file not found: "); printf(file); return NULL;}

    // Gets the length of the source file
    fseek(src_file,0,SEEK_END);
    int length = ftell(src_file);
    fseek(src_file,0,SEEK_SET);
    
    string filestr = newstr(aren);
    char* buffer = apush_buf(aren, length);

    fread(buffer,1,length,src_file);
    fclose(src_file);

    filestr->raw_string = buffer;
    filestr->length = length - 1;
    return filestr;
}

#define rawptr void*
typedef struct AST {
    rawptr start;
    rawptr end;
} AST;

enum NodeType {
    FUNCTION,
    VAR,
    LITERAL,
};

enum VarType {
    TYPE_VOID,
    TYPE_I8,
    TYPE_I16,
    TYPE_I32,
    TYPE_I64,
    TYPE_U8,
    TYPE_U16,
    TYPE_U32,
    TYPE_U64,
    TYPE_CHAR,
    TYPE_GLYPH,
    TYPE_STRUCT,
    TYPE_RAWPTR,
    TYPE_REF,
    TYPE_STRING,
};

typedef struct Var {
    enum VarType var_type;
    string var_identifier;
} Var;

// FUNCTION NODE SYNTAX
// [type] -> [Params] -> [Body]
typedef struct Function {
    string name;
    rawptr params;
    rawptr type;
    rawptr body;
} Function;

struct FunctionList {
    u64 length;
    Function* list;
};

struct FunctionConstructorReturn {
    Function* returned_func;
    u64 end_token_index;
};
#define FIND_NON_NEWLINE while (atoms->list[++current_token_index].token != NEWLINE)
struct FunctionConstructorReturn construct_function(arena ast_arena, AtomList* atoms, u64 current_token_index) {
    // Constructs the params (UNIMPLEMENTED)
    while (atoms->list[current_token_index].token != CLOSE_PAREN) current_token_index++;

    // Constructs the return type
    FIND_NON_NEWLINE;

    if (atoms->list[current_token_index].token != COLON) {printf("Unexpected token: Function names need to be followed by a : before the return type(s)");}
    FIND_NON_NEWLINE;

    if (atoms->list[current_token_index].token != OPEN_BRACE) {printf("Unexpected token: Function bodies must be preceeded with a {");}

    while (atoms->list[current_token_index].token != CLOSE_BRACE) {
        
    }
    // Constructs the function body
    //Token current_token;
    //while ((current_token = atoms->list[current_token_index++].token) != OPEN_BRACE) {
    //    if (current_token == SEMICOLON) {return {NULL,0};}
    //    Var new_var = {0};
    //    switch (atoms->list[current_token_index].token) {
    //        case CUSTOM: new_var.var_identifier = atoms->list[current_token_index++].extra_data; break;
    //        case NEWLINE: current_token_index++; break;
    //        default: printf("Invalid function return type(s)"); exit(1);
    //    }
    //    
    //}
}

AST* build_tree(arena ast_arena, AtomList* atoms) {
    arena dynamic_list_arena = aalloc(8388608);

    AST* syntree;
    struct FunctionList* func_ls;
    {
        struct FunctionList _fls = {0};
        AST _syntree = {0};
        syntree = apush(ast_arena, _syntree);
        func_ls = apush(dynamic_list_arena, _fls); func_ls->list = func_ls + sizeof(struct FunctionList);
    }
    u64 current_token_index = 0;
    u64 line_number = 1;

    while (current_token_index < atoms->listlen) {
        Atom current_atom = atoms->list[current_token_index++];
        HARDBREAK:
        switch (current_atom.token) {
            case NEWLINE: line_number++; continue;
            case CUSTOM: {
                LOOP:
                switch(atoms->list[current_token_index++].token) {
                    case NEWLINE: goto LOOP;
                    case OPEN_PAREN: {
                        struct FunctionConstructorReturn result = construct_function(ast_arena, atoms, current_token_index);
                        current_token_index = result.end_token_index;
                        apush(dynamic_list_arena, result.returned_func);
                        func_ls->length++;
                        goto HARDBREAK;
                    }
                    default: debug_print("UNRECOGNIZED TOKEN ERROR: Token not valid in top scope"); exit(1);
                }
            }
            default: continue;
        }
    }
    afree(dynamic_list_arena);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {printf("Usage: ./<exe> <source file>\n"); return 1;}
    u64 gigabyte = 1073741824 / 3;
    arena rawfile_arena = aalloc(gigabyte);
    arena token_arena = aalloc(gigabyte);
    arena ast_arena = aalloc(gigabyte);

    string str = map_file_to_memory(rawfile_arena, argv[1]);

    if (str == NULL) return 1; else printf("File read in successfully\n");

    AtomList* atoms = atomize(token_arena, str);
    //print_atom_list(atoms);
    build_tree(ast_arena, atoms);


    afree(rawfile_arena);
    afree(token_arena);
}