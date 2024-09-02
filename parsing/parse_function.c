#ifndef INCLUDES
#include "ALL_INCLUDES"
#endif

#define BOUNDS_CHECK if (next >= bound) { error_on(next->start, file_text, PARSING_ERROR); printf("Parsing error: file terminates early at function definition. next = %p, bound = %p\n", next, bound); exit(1); }

void parse_function_header(arena ass_arena, Atom** atoms, Atom* bound, const string const file_text) {
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
        BOUNDS_CHECK
        switch (next->token) {
            case CUSTOM:
                switch (current_stage) {
                    case EXPECT_VARNAME:
                    case EXPECT_DEFAULT:
                        to_push.start = next->start;
                        to_push.type = POL_INIT_FN_PARAM;
                        current_stage = COLON;
                    break;
                    case EXPECT_VARTYPE:
                        error_on(next->start, file_text, PARSING_ERROR); printf("Custom types are currently not supported\n"); exit(1);
                    case EXPECT_COLON: 
                        error_on(next->start, file_text, PARSING_ERROR); printf("Custom types are currently not supported - but you also forgot the : separator\n"); exit(1);
                    default: 
                }
            break;
            case COLON:
                switch (current_stage) {
                    case COLON: current_stage = EXPECT_VARTYPE; break;
                    default: error_on(next->start, file_text, PARSING_ERROR); printf("Expected colon separator but found something else\n"); exit(1);
                }
            break;
            case I32:
                switch (current_stage) {
                    case EXPECT_VARTYPE: current_stage = EXPECT_END; break;
                    default: error_on(next->start, file_text, PARSING_ERROR); printf("Found something other than a type when a type was expected\n"); exit(1);
                }
            break;
            case CLOSE_PAREN:
                switch (current_stage) {
                    case EXPECT_DEFAULT:
                    case EXPECT_END: 
                        if (to_push.type == POL_INIT_FN_PARAM) {
                            PolNode* pushed = apush(ass_arena, to_push);
                            pushed->type = POL_INIT_LOCAL_VAR;
                            pushed = apush(ass_arena, to_push);
                            pushed->type = POL_INIT_FN_PARAM;
                            pushed->start = to_push.start;
                        }
                        return;
                    break;
                    case EXPECT_VARNAME: 
                        error_on(next->start, file_text, PARSING_ERROR); printf("Expected variable identifier, but found end of function parameters\n"); 
                        exit(1);
                    case EXPECT_COLON: 
                        error_on(next->start, file_text, PARSING_ERROR); printf("Expected : separator and variable type, but found end of function parameters\n"); 
                        exit(1);
                    case EXPECT_VARTYPE: 
                        error_on(next->start, file_text, PARSING_ERROR); printf("Expected variable type but found end of function parameters\n"); 
                        exit(1);
                    default: error_on(next->start, file_text, PARSING_ERROR); printf("Unexpected termination\n"); exit(1);
                }
            break;
            case COMMA: switch (current_stage) {
                    case EXPECT_DEFAULT:
                    case EXPECT_END: 
                        if (to_push.type == POL_INIT_FN_PARAM) {
                            PolNode* pushed = apush(ass_arena, to_push);
                            pushed->type = POL_INIT_LOCAL_VAR;
                            pushed = apush(ass_arena, to_push);
                            pushed->type = POL_INIT_FN_PARAM;

                            pushed->start = to_push.start;
                            current_stage = EXPECT_VARNAME;
                            break;
                        } else {
                            error_on(next->start, file_text, PARSING_ERROR); printf("Variable unexpectedly ended!\n"); exit(1);
                        }
                    break;
                    default: error_on(next->start, file_text, PARSING_ERROR); printf("Unexpected termination\n"); exit(1);
                }
            break;
            default: 
            error_on(next->start, file_text, PARSING_ERROR); printf("Unexpected token: "); 
            print_atom(*next, 0, NULL);
            exit(1);
        }
    }
}

void parse_function_returns(arena ass_arena, Atom** atoms, Atom* bound, const string const file_text) {
    while (1) {
        Atom* next = (*atoms)++;
        BOUNDS_CHECK
        switch (next->token) {
            case I32:
                PolNode el = {0};
                PolNode* el_star = apush(ass_arena,el);
                el_star->type = POL_APPEND_RETURN_TYPE;
                el_star->start = next->start;
                break;
            default: error_on(next->start, file_text, PARSING_ERROR); printf("Illegal function return type: "); print_atom(*next,0,NULL); exit(1);
        }
        next = (*atoms)++;
        BOUNDS_CHECK
        switch (next->token) {
            case COMMA:
                break;
            case OPEN_BRACE:
                return;
            default: error_on(next->start, file_text, PARSING_ERROR); printf("Illegal item in function return type: "); print_atom(*next,0,NULL); exit(1);
        }
    }

}

typedef enum bool {
    false,
    true,
} bool;

bool is_number(u32 start, const string const file_text) {
    u32 len = token_length(start,file_text);
    for (int i = start; i < start + len; i++) {
        if (file_text->raw_string[i] > '9' || file_text->raw_string[i] < '1') {
            return false;
        }
    }
    return true;
}

int get_operator_priority(PolType type) {
    switch (type) {
        case RETURN: return 1;
        default: return 0;
    }
}

PolNode* parse_tokens_into_nodes(arena output_arena, Atom** atoms, Atom* bound, const string const file_text) {
    arena operator_stack = aalloc(4096);


    Atom* next = (*atoms);
    PolNode empty = {0};
    PolNode* last = NULL;
    PolNode* operators = apush(operator_stack, empty);
    u32 count = 0;
    BOUNDS_CHECK
    while ((next = (*atoms)++)->token != CLOSE_BRACE) {
        BOUNDS_CHECK
        switch (next->token)
        {
            case CUSTOM:
                if (is_number(next->start, file_text)) {
                    PolNode* pushed = apush(output_arena, empty);
                    pushed->type = POL_LITERAL;
                    pushed->start = next->start;
                    last = pushed;
                }
                else {
                    PolNode* pushed = apush(output_arena, empty);
                    pushed->type = POL_VAR;
                    pushed->start = next->start;
                    last = pushed;
                }
            break;
            case RETURN:
                if (count == 0) {
                    PolNode* pushed = apush(operator_stack, empty);
                    pushed->type = POL_RETURN;
                    count++;
                }
            break;
            case SEMICOLON:
                while (count > 0) {
                    PolNode* pushed = apush(output_arena, empty);
                    pushed->type = operators[count].type;
                    pushed->start = operators[count].start;
                    count--;
                    pop(operator_stack,sizeof(PolNode));
                    last = pushed;
                }
            break;
            default:
                break;
        }
    }

    afree(operator_stack);
    return last;
}

PolNode* parse_function(arena ass_arena, Atom** atoms, Atom* bound, const string const file_text) {
    parse_function_header(ass_arena, atoms, bound, file_text);
    Atom* next = (*atoms)++;
    BOUNDS_CHECK
    if (next->token == COLON) {
        parse_function_returns(ass_arena, atoms, bound, file_text);
    }
    else if (next->token != OPEN_BRACE) {printf("Parsing error: Illegal start of function"); exit(1);}
    return parse_tokens_into_nodes(ass_arena, atoms, bound, file_text);
}
#undef BOUNDS_CHECK