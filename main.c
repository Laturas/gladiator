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

typedef enum VarType {
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
    TYPE_STRUCT,
    TYPE_RAWPTR,
    TYPE_REF,
    TYPE_STRING,
} VarType;

typedef struct Var {
    VarType var_type;
    string var_identifier;
} Var;

// FUNCTION NODE SYNTAX
// [type] -> [Params] -> [Body]
#include "blocks.h"

struct FunctionList {
    u64 length;
    Function* list;
};

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
        
    }
    afree(dynamic_list_arena);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {printf("Usage: ./<exe> <source file>\n"); return 1;}
    u64 gigabyte = 1073741824 / 4;
    arena rawfile_arena = aalloc(gigabyte);
    arena token_arena = aalloc(gigabyte);
    arena ast_arena = aalloc(gigabyte);

    string str = map_file_to_memory(rawfile_arena, argv[1]);

    if (str == NULL) return 1; else printf("File read in successfully\n");

    AtomList* atoms = atomize(token_arena, str);
    print_atom_list(atoms, LINES);
    //build_tree(ast_arena, atoms);
    function_block(ast_arena, str, &(atoms->list));


    afree(rawfile_arena);
    afree(token_arena);
}