#ifndef ATOMIZER
#define ATOMIZER
//#ifndef u64
//#define u64 unsigned long long
//#endif

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

    // Unary operators
    NEGATE, // -
    NOT, // !
    COMPLEMENT, // ~

    // Binary operators
    PLUS, // +
    MULTIPLY, // *

    EQUALS, // =
    RETURN,
    INT_LITERAL,

    /** Non-appending tokens are tokens that won't make it to the final stream. This includes whitespace, newlines, and comment markers. */
    NO_APPEND,
    WHITESPACE,
    NEWLINE,
    ONELINE_COMMENT,
} typedef Token;

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
    EXCLUDE_OTHER_TAG = 4,
};

void print_atom_list(AtomList* list, enum AtomPrintParams params, const string const file_ref);
AtomList* atomize(arena token_storage_arena, string str);

#include "atomizer.c"
#endif