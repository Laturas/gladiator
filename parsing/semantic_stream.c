#ifndef INCLUDES
#include "ALL_INCLUDES"
#endif

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
                }
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

                    PolNode* last_node = parse_function(ass_arena, &atoms, last, file_text);
                    struct AbstractSyntaxStream new = {ass_start, last_node};
                    return new;
                    
                    expect = NONE;
                }
                break;
            default: break;
        }
    }
}