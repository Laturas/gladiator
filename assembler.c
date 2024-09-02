#ifndef INCLUDES
#include "ALL_INCLUDES"
#endif

int get_int_literal(u32 start, const string const file_str) {
    char buffer[50] = {0};
    int i = 0;
    while (start < file_str->length && match_1(file_str->raw_string[start]) == CUSTOM) {
        buffer[i++] = file_str->raw_string[start++];
    }
    return atoi(buffer);
}

void generate_assembly(struct AbstractSyntaxStream ass, const string const file_str) {
    freopen("output.s", "w", stdout); 
    PolNode* first_cp = ass.first;
    while (first_cp <= ass.last) {
        switch (first_cp->type) {
            case POL_FUNC_START:
                printf("	.globl	__"); print_to_next_token(first_cp->start,file_str); printf("\n__");
                print_to_next_token(first_cp->start,file_str); printf(":\n");
            break;
            case POL_LITERAL:
                printf("	movl    $"); printf("%d",get_int_literal(first_cp->start, file_str)); printf(", %%eax\n");
            break;
            case POL_RETURN:
                printf(" 	ret\n");
            break;
        }
        first_cp++;
    }
}