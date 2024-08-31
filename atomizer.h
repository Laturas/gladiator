#ifndef ATOMIZER
#define ATOMIZER
#ifndef u64
#define u64 unsigned long long
#endif

enum Token {
    // A token not in the default list. Func/var names and literals primarily.
    CUSTOM,

    // Scope handlers
    OPEN_BRACE,
    CLOSE_BRACE,
    SEMICOLON,

    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_SQBRACKET,
    CLOSE_SQBRACKET,

    COLON,
    COMMA,
    DOT,
    
    // Numerics
    I32,
    U8,

    RETURN,
    INT_LITERAL,
    WHITESPACE,
    NEWLINE,

    ONELINE_COMMENT,
} typedef Token;

#ifndef u32
#define u32 unsigned int
#endif

typedef struct Atom {
    Token token;
    u32 start;
} Atom;

void test_atom() {sizeof(Atom);}

typedef struct AtomList {
    u64 listlen;
    Atom* list;
} AtomList;

enum AtomPrintParams {
    NONE = 0,
    LINES = 1,
    FILE_PRESENT = 2,
};

void print_atom_list(AtomList* list, enum AtomPrintParams params, const string const file_ref);
AtomList* atomize(arena token_storage_arena, string str);

#include "atomizer.c"
#endif