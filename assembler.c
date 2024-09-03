#ifndef INCLUDES
#include "ALL_INCLUDES"
#endif

static enum Register {
    NO_REGISTER,
    EAX,
    ECX, 
    EDX,
} Register;

int get_int_literal(u32 start, const string const file_str) {
    char buffer[50] = {0};
    int i = 0;
    while (start < file_str->length && match_1(file_str->raw_string[start]) == CUSTOM) {
        if (i >= 10) {error_on(start, file_str, ASSEMBLING_ERROR); exit(1);}
        buffer[i++] = file_str->raw_string[start++];
    }
    return atoi(buffer);
}

static void print_register(enum Register reg_to_print, bool newline) {
    switch (reg_to_print) {
        case NO_REGISTER:
            printf("error! Null register value attempting to be written");
        break;
        case EAX:
            printf("%%eax");
        break;
        //case EBX:
        //    printf("%%ebx");
        //break;
        case ECX:
            printf("%%ecx");
        break;
        case EDX:
            printf("%%edx");
        break;
    }
    if (newline) {printf("\n");}
}

void generate_assembly(struct AbstractSyntaxStream ass, const string const file_str) {
    enum Register current = NO_REGISTER;
    freopen("output.s", "w", stdout); 
    PolNode* first_cp = ass.first;
    while (first_cp <= ass.last) {
        switch (first_cp->type) {
            case POL_FUNC_START:
                printf("	.globl	__"); print_to_next_token(first_cp->start,file_str); printf("\n__");
                print_to_next_token(first_cp->start,file_str); printf(":\n");
            break;
            case POL_LITERAL:
                current++;
                printf("	movl   $"); printf("0x%x, ",get_int_literal(first_cp->start, file_str)); print_register(current, true);
            break;
            case POL_NEGATE:
                printf(" 	neg    "); print_register(current, true);
            break;
            case POL_COMPLEMENT:
                printf(" 	not    "); print_register(current, true);
            break;
            case POL_NOT:
                printf(" 	cmpl   $0, "); print_register(current, true);
                printf(" 	movl   $0, "); print_register(current, true); // Using xor clears the flags and thus can't be used to zero eax :c
                printf(" 	sete   %%al\n");
            break;
            case POL_ADD:
                printf(" 	addl   "); print_register(current, false); printf(", "); print_register(--current,true);
            break;
            case POL_RETURN:
                printf(" 	ret\n");
            break;
        }
        first_cp++;
    }
}