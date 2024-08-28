#ifndef INCLUDES
#include "ALL_INCLUDES"
#endif

void print_atom(Atom input_atom, enum AtomPrintParams params) {
    if (params & LINES) {printf("Line %d: ", input_atom.line_number);}

    switch (input_atom.token) {
        case CUSTOM:
            printf("OTHER("); print(&input_atom.extra_str); printf(")"); break;
        case OPEN_BRACE:
            printf("{"); break;
        case CLOSE_BRACE:
            printf("}"); break;
        case OPEN_PAREN:
            printf("("); break;
        case CLOSE_PAREN:
            printf(")"); break;
        case OPEN_SQBRACKET:
            printf("["); break;
        case CLOSE_SQBRACKET:
            printf("]"); break;
        case SEMICOLON:
            printf(";"); break;
        case COLON:
            printf(":"); break;
        case DOT:
            printf("."); break;
        case COMMA:
            printf(","); break;
        case I32:
            printf("i32"); break;
        case U8:
            printf("u8"); break;
        case RETURN:
            printf("return"); break;
        case INT_LITERAL:
            printf("[value]"); break;
        case WHITESPACE:
            printf("WHITESPACE"); break;
        case NEWLINE:
            printf("NEWLINE"); break;
        case ONELINE_COMMENT:
            printf("//"); break;
    }
}

void print_atom_list(AtomList* list, enum AtomPrintParams params) {
    for (int i = 0; i < list->listlen; i++) {
        print_atom(list->list[i], params); printf("\n");
    }
}

Token match_6(string str) {
    if (str->length != 6) return CUSTOM;
    if (str_chrstr_equal(str,"return")) {
        return RETURN;
    }
    else {
        return CUSTOM;
    }
}

Token match_5(string str) {
    return CUSTOM;
}

Token match_4(string str) {
    return CUSTOM;
}

Token match_3(string str) {

}

Token match_1(char match_char) {
    switch (match_char) {
        case ';': return SEMICOLON;
        case ':': return COLON;
        case '{': return OPEN_BRACE;
        case '}': return CLOSE_BRACE;
        case '(': return OPEN_PAREN;
        case ')': return CLOSE_PAREN;
        case '[': return OPEN_SQBRACKET;
        case ']': return CLOSE_SQBRACKET;
        case '.': return DOT;
        case ',': return COMMA;
        case '\n': return NEWLINE;
        case ' ' : return WHITESPACE;

        default: return CUSTOM;
    }
}

Token match(string str) {
    switch (str->length) {
        case 6:
            return match_6(str);
        case 5:
            return match_5(str);
        case 4:
            return match_4(str);
        case 3:
            if (str->raw_string[0] == 'i' && str->raw_string[1] == '3' && str->raw_string[2] == '2') {return I32;}
            return CUSTOM;
            //return match_3(str);
        case 2:
            if (str->raw_string[0] == 'u' && str->raw_string[1] == '8') {return U8;}
            if (str->raw_string[0] == '/' && str->raw_string[1] == '/') {return ONELINE_COMMENT;}
            return CUSTOM;
        case 1:
            return match_1(str->raw_string[0]);
        default:
            return CUSTOM;
    }
}

Atom token_list_append(arena token_arena, AtomList* atomlist, Atom atom) {
    if (atom.token == WHITESPACE || atom.token == ONELINE_COMMENT || atom.token == NEWLINE) {return atom;} // Whitespace is not appended
    Atom* a = apush(token_arena, atom);
    if (!(atomlist->listlen++)) {atomlist->list = a;}
    return atom;
}

Atom construct_atom(Token token, _String file_text, u64 line) {
    _String tmp = {file_text.length, file_text.raw_string};
    Atom return_atom = {token,tmp,line};
    switch (token) {
        case CUSTOM:
        case INT_LITERAL:
            return_atom.extra_str = file_text;
        default:
            return return_atom;
    }
}

#define IN_BOUNDS (max_bound > newstr.raw_string + newstr.length)

/**
 * Turns the raw string into a list of atoms
 */
AtomList* atomize(arena token_storage_arena, string str) {
    _String newstr = {1, str->raw_string};
    AtomList* atoms;
    u64 line_num = 1;

    // I don't want to accidentally modify the stack-allocated struct
    {
        AtomList _token_list = {0,NULL};
        atoms = apush(token_storage_arena, _token_list);
    }
    
    char* max_bound = str->raw_string + str->length;
    char symbol; Atom prevAtom;
    while (max_bound > newstr.raw_string + newstr.length) {
        symbol = CUSTOM;
        while (prevAtom.token == ONELINE_COMMENT && (symbol = match_1(newstr.raw_string[0])) != NEWLINE && IN_BOUNDS) {newstr.raw_string++;}
        line_num += (symbol == NEWLINE);
        if (!IN_BOUNDS) {break;}

        // Moves to the first non-whitespace character
        while ((symbol = match_1(newstr.raw_string[0])) == WHITESPACE) {newstr.raw_string++;}

        if (symbol == CUSTOM) { // Extends the length of the current token until it hits a tokenizing character
            while (!(symbol = match_1(newstr.raw_string[newstr.length])) && IN_BOUNDS) {newstr.length++;}
        }
        line_num += (symbol == NEWLINE);

        prevAtom = token_list_append(token_storage_arena, atoms, construct_atom(match(&newstr), newstr, line_num));
        newstr.raw_string += newstr.length;
        newstr.length = 1;
    }
    return atoms;
}
#undef IN_BOUNDS