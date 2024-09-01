#ifndef INCLUDES
#include "ALL_INCLUDES"
#endif

#include "parse_function.c"

PolNode* generate_ass(arena ass_arena, AtomList* atom_list, string file_text) {
    enum GlobalTypes {
        NONE,
        FUNCTION,
        STRUCT,
        OTHER,
    };
    enum GlobalTypes expect = NONE;

    arena dynamic_list_arena = aalloc(8388608);

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
                    PolNode* start = ass_start;
                    while (start <= last_node) {
                        print_node((*start), file_text); printf("\n");
                        start++;
                    }
                    
                    expect = NONE;
                }
                break;
            default: break;
        }
    }
    afree(dynamic_list_arena);
}