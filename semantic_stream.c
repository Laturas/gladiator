#ifndef INCLUDES
#include "ALL_INCLUDES"
#endif

typedef enum ERROR_TYPE {
    DEFAULT_ERROR,
    PARSING_ERROR,
    ASSEMBLING_ERROR,
} ERROR_TYPE;

void print_line_column(u32 start, const string const file_buff) {
    u32 line = 1; u32 column = 1;
    for (u32 i = 0; i < start; i++) {
        if (file_buff->raw_string[i] == '\n') {column = 1; line++;}
        else {column++;}
    }
    printf("line %d column %d", line, column);
}

void error_on(u32 start, const string const file_text, ERROR_TYPE error) {
    switch (error) {
        case DEFAULT_ERROR: printf("Error on: "); break;
        case PARSING_ERROR: printf("Parsing error on: "); break;
        case ASSEMBLING_ERROR: printf("Assembling error on: "); break;
    }
    print_line_column(start,file_text); printf("\n");
}

#include "parse_function.c"

struct AbstractSyntaxStream {
    PolNode* first;
    PolNode* last;
};

struct AbstractSyntaxStream generate_ass(arena ass_arena, AtomList* atom_list, string file_text) {
    enum GlobalTypes {
        NONE,
        FUNCTION,
        STRUCT,
        OTHER,
    };
    enum GlobalTypes expect = NONE;

    Atom* atoms = atom_list->list;
    Atom* last = atom_list->listlen + atom_list->list;
    PolNode* ass_start = NULL;
    u32 saved_start = 0;

    while (atoms < last) {
        Atom* current_atom = atoms++;
        Token new_token = current_atom->token;
        switch (new_token) {
            case CUSTOM: 
                if (expect == NONE) {
                    saved_start = current_atom->start;
                    expect = FUNCTION;
                    break;
                }
                //printf("Parsing error on line %d: Unexpected token: ", current_atom->line_number); print_atom(*current_atom, 0); exit(1);
                break;
            case OPEN_PAREN:
                if (expect == FUNCTION) {
                    PolNode pusher = {POL_FUNC_START, saved_start};
                    if (!ass_start) {
                        ass_start = apush(ass_arena, pusher);
                        ass_start->start = saved_start;
                        ass_start->type = POL_FUNC_START;
                    } else {
                        PolNode* tmpptr = apush(ass_arena, pusher);
                        tmpptr->start = saved_start;
                        tmpptr->type = POL_FUNC_START;
                    }

                    PolNode* last_node = parse_function(ass_arena,&atoms,last,file_text);
                    struct AbstractSyntaxStream new = {ass_start, last_node};
                    return new;
                    
                    expect = NONE;
                }
                break;
            default: break;
        }
    }
}