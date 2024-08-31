#ifndef INCLUDES
#include "ALL_INCLUDES"
#endif

struct SyntaxStream {
    PolNode* stream_start;
};

void parse_function_header(arena ass_arena, Atom** atoms, Atom* bound) {
    enum HeaderExpectations {
        EXPECT_DEFAULT, // Expect end of params, or start of next param 
        EXPECT_VARNAME, // Expect name of variable (only comes after a comma is seen)
        EXPECT_COLON,   // Expect colon separator
        EXPECT_VARTYPE, // Expect variable type
        EXPECT_END,     // Expect the end of params, or a comma
    };
    enum HeaderExpectations current_stage = EXPECT_DEFAULT;

    PolNode to_push = {0, 0};

    Atom* start = *atoms;
    while (1) {
        Atom* next = (*atoms)++;
        if (next >= bound) {printf("Parsing error: file terminates early at function definition. next = %p, bound = %p\n", next, bound); exit(1);}
        switch (next->token) {
            case CUSTOM:
                switch (current_stage) {
                    case EXPECT_VARNAME:
                    case EXPECT_DEFAULT:
                        to_push.start = next->start;
                        to_push.type = POL_INIT_FN_PARAM;
                        current_stage = COLON;
                    break;
                    default: printf("Parsing error: custom types are currently not supported\n"); exit(1);
                }
            break;
            case COLON:
                switch (current_stage) {
                    case COLON: current_stage = EXPECT_VARTYPE; break;
                    default: printf("Parsing error: Expected colon separator but found something else\n"); exit(1);
                }
            break;
            case I32:
                switch (current_stage) {
                    case EXPECT_VARTYPE: current_stage = EXPECT_END; break;
                    default: printf("Parsing error: Found something other than a type when a type was expected\n"); exit(1);
                }
            break;
            case CLOSE_PAREN:
                switch (current_stage) {
                    case EXPECT_DEFAULT:
                    case EXPECT_END: 
                        if (to_push.type == POL_INIT_FN_PARAM) {
                            PolNode* pushed = apush(ass_arena, to_push);
                            pushed->type = POL_INIT_LOCAL_VAR;
                            pushed->start = to_push.start;
                            pushed = apush(ass_arena, to_push);
                            pushed->type = POL_INIT_FN_PARAM;
                            return;
                        }
                    break;
                    default: printf("Parsing error: Unexpected termination\n"); exit(1);
                }
            break;
            case COMMA: switch (current_stage) {
                    case EXPECT_DEFAULT:
                    case EXPECT_END: 
                        if (to_push.type == POL_INIT_FN_PARAM) {
                            PolNode* pushed = apush(ass_arena, to_push);
                            pushed->type = POL_INIT_FN_PARAM;
                            pushed->start = to_push.start;
                            current_stage = EXPECT_VARNAME;
                            break;
                        } else {
                            printf("Parsing error: Variable unexpectedly ended!\n"); exit(1);
                        }
                    break;
                    default: printf("Parsing error: Unexpected termination\n"); exit(1);
                }
            break;
            default: 
            printf("parsing error: Unexpected token: "); 
            print_atom(*next, 0, NULL); 
            printf("\nToken value: %d\n", next->token);
            exit(1);
        }
    }
}

#define BOUNDS_CHECK if (next >= bound) {printf("Parsing error: file terminates early at function definition. next = %p, bound = %p\n", next, bound); exit(1);}

void parse_function_returns(arena ass_arena, Atom** atoms, Atom* bound) {
    while (1) {
        Atom* next = (*atoms)++;
        BOUNDS_CHECK
        switch (next->token) {
            case I32:
                PolNode el = {0};
                apush(ass_arena,el);
                break;
            default: printf("Illegal function return type: "); print_atom(*next,0,NULL); exit(1);
        }
        next = (*atoms)++;
        BOUNDS_CHECK
        switch (next->token) {
            case COMMA:
                break;
            case OPEN_BRACE:
                return;
            default: printf("Illegal item in function return type: "); print_atom(*next,0,NULL); exit(1);
        }
    }

}

typedef enum bool {
    false,
    true,
} bool;

bool is_number(u32 start, const string const file_text) {
    u32 len = token_length(start,file_text);
    for (int i = 0; i < len; i++) {
        if (file_text->raw_string[i] >= '9' || file_text->raw_string[i] <= '1') {
            return false;
        }
    }
    return true;
}

PolNode* reverse_polish_push(arena output, arena stack_arena, PolType type, Atom atom, const string const file_text) {
    
}

void parse_tokens_into_nodes(arena ass_arena, Atom** atoms, Atom* bound, const string const file_text) {
    arena operator_stack = aalloc(4096);


    Atom* next = (*atoms);
    PolNode empty = {0};
    PolNode* last = NULL;
    BOUNDS_CHECK
    while ((next = (*atoms)++)->token != CLOSE_BRACE) {
        BOUNDS_CHECK
        switch (next->token)
        {
            case CUSTOM:
                if (is_number(next->start, file_text)) {
                    last = reverse_polish_push(ass_arena, operator_stack, POL_LITERAL, *next, file_text);
                }
                else {
                    last = reverse_polish_push(ass_arena, operator_stack, POL_VAR, *next, file_text);
                }
            break;
            case RETURN:
                last = reverse_polish_push(ass_arena, operator_stack, POL_RETURN, *next, file_text);
            break;
            default:
                break;
        }
    }

    afree(operator_stack);
}
#undef BOUNDS_CHECK

void parse_function(arena ass_arena, Atom** atoms, Atom* bound, const string const file_text) {
    parse_function_header(ass_arena, atoms, bound);
    Atom* next = (*atoms)++;
    if (next >= bound) {printf("Parsing error: file terminates early at function definition. next = %p, bound = %p\n", next, bound); exit(1);}
    if (next->token == COLON) {
        parse_function_returns(ass_arena, atoms, bound);
    }
    else if (next->token != OPEN_BRACE) {printf("Parsing error: Illegal start of function"); exit(1);}
    parse_tokens_into_nodes(ass_arena, atoms, bound, file_text);
}

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

                    parse_function(ass_arena,&atoms,last,file_text);
                    PolNode* start = ass_start;
                    while (start < (*ass_arena.size) + ass_arena.buffer) {
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