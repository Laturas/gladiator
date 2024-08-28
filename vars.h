#ifndef INCLUDES
#include "ALL_INCLUDES"
#endif

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

void print_var_type (VarType type) {
    switch (type) {
        case TYPE_VOID: printf("Void\n"); break;
        case TYPE_I8: printf("i8\n"); break;
        case TYPE_I16: printf("i16\n"); break;
        case TYPE_I32: printf("i32\n"); break;
        case TYPE_I64: printf("i64\n"); break;
        case TYPE_U64: printf("u64\n"); break;
        case TYPE_U32: printf("u32\n"); break;
        case TYPE_U16: printf("u16\n"); break;
        case TYPE_U8: printf("u8\n"); break;
        default: printf("Wtf\n"); break;
    }
    fflush(stdout);
}

typedef struct Var {
    VarType var_type;
    string var_identifier;
} Var;