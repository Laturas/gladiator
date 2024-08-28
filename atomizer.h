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

typedef struct Atom {
    Token token;
    _String extra_str;
    unsigned short line_number;
} Atom;

typedef struct AtomList {
    u64 listlen;
    Atom* list;
} AtomList;

enum AtomPrintParams {
    NONE = 0,
    LINES = 1,
};

void print_atom_list(AtomList* list, enum AtomPrintParams params);
AtomList* atomize(arena token_storage_arena, string str);

#include "atomizer.c"
#endif