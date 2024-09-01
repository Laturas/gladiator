#ifndef INCLUDES
#include "ALL_INCLUDES"
#endif

typedef enum PolTypes {
    POL_EMPTY,
    POL_FUNC_START,
    POL_INIT_FN_PARAM,
    POL_INIT_LOCAL_VAR,
    POL_ASSIGN,
    POL_RETURN,
    POL_LITERAL,
    POL_VAR,
    POL_APPEND_RETURN_TYPE, // Appends a return type to the current function
    POL_LPAREN, // (
    POL_RPAREN, // )
} PolType;

#ifndef u32
#define u32 unsigned int
#endif
typedef struct PolNode {
    PolType type;
    u32 start;
} PolNode;

void print_node(PolNode input, const string const file_buffer) {
    switch (input.type) {
        case POL_EMPTY:
            printf("Do nothing"); break;
        case POL_FUNC_START:
            printf("Declare fn: "); print_to_next_token(input.start, file_buffer); break;
        case POL_INIT_FN_PARAM:
            printf("Load as fn param"); break;
        case POL_INIT_LOCAL_VAR:
            printf("Declare var: "); print_to_next_token(input.start, file_buffer); break;
        case POL_ASSIGN:
            printf("Assign"); break;
        case POL_RETURN:
            printf("Return"); break;
        case POL_LITERAL:
            print_to_next_token(input.start, file_buffer); break;
        case POL_APPEND_RETURN_TYPE:
            printf("Append return type to current function: "); print_to_next_token(input.start, file_buffer); break;
    }
}

static void test() {sizeof(PolNode);}

#include "semantic_stream.c"