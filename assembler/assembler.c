#ifndef INCLUDES
#include "ALL_INCLUDES"
#endif

static enum Register {
    NO_REGISTER,
    EAX,
    EBX,
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
        case EBX:
            printf("%%ebx");
        break;
        case ECX:
            printf("%%ecx");
        break;
        case EDX:
            printf("%%edx");
        break;
    }
    if (newline) {printf("\n");}
}

bool item_is_in_register(enum Register current_in_use_register, int item_index, enum Register* held_registers, enum Register *out_register) {
    for (int i = EAX; i <= current_in_use_register; i++) {
        if (held_registers[i] == item_index) {*out_register = i; return true;}
    }
    *out_register = NO_REGISTER;
    return false;
}

void apply_unaries(FILE* output, enum Register reg, PolType* unary_operator_stack, int next_unop_index, int amount_to_apply) {
    //fprintf(output, "Item has to have %d unaries applied\n", amount_to_apply);
    for (int i = 0; i < amount_to_apply; i++) {

        //fprintf(output, "Applying unary: "); fprint_node(output, tmp, NULL); fprintf(output, "\n");
        switch (unary_operator_stack[next_unop_index - 1 - i]) {
            case POL_NEGATE:
                printf(" 	neg    "); print_register(reg, true);
            break;
            case POL_COMPLEMENT:
                printf(" 	not    "); print_register(reg, true);
            break;
            case POL_NOT:
                printf(" 	cmpl   $0, "); print_register(reg, true);
                printf(" 	movl   $0, "); print_register(reg, true); // Using xor clears the flags and thus can't be used to zero eax :c
                printf(" 	sete   %%al\n");
            break;
        }
    }
}

struct AsmItem {
    int unaries;
    int value;
};

void fprint_comp_stacks(FILE* output, struct AsmItem* compiler_stack, PolType* unary_operator_stack, int first_item_index, int next_item_index, int next_unop_index, const string const file_str) {
    fprintf(output, "Unary operators: [");
    if (next_unop_index) {
        PolNode wrapper = {unary_operator_stack[0], 0};
        fprint_node(output, wrapper, file_str);
    }
    for (int i = first_item_index; i < next_unop_index; i++) {
        PolNode wrapper = {unary_operator_stack[i], 0};
        fprintf(output, ", ");
        fprint_node(output, wrapper, file_str);
    }
    fprintf(output, "]\n");
    fprintf(output, "Unary operator counts: [");
    if (next_item_index) {
        fprintf(output, "%d", compiler_stack[0].unaries);
    }
    for (int i = first_item_index; i < next_item_index; i++) {
        fprintf(output, ", ");
        fprintf(output, "%d", compiler_stack[i].unaries);
    }
    fprintf(output, "]\n");
}

bool prepare_for_binop(struct AsmItem* item_ref, int item_index, enum Register* register_reference, enum Register* registers) {
    enum Register _;
    struct AsmItem item = *item_ref;
    if (item.unaries > 0) {
        if (!item_is_in_register(*register_reference, item_index, registers, &_)) {
            registers[++(*register_reference)] = item_index;
            printf("	movl   $0x%x, ", item.value);
            print_register((*register_reference), true);
        }
        return true;
    }
    return false;
}

void generate_asm(struct AbstractSyntaxStream ass, const string const file_str) {
    enum Register held_registers[5] = {0}; // Holds indexes into compiler stack, knows when to reuse a register.
    enum Register current_in_use_register = NO_REGISTER;
    struct AsmItem compiler_stack[255] = {0};
    PolType unary_operator_stack[255] = {0};
    int next_item_index = 0;
    int next_unop_index = 0;

    int first_item_index = 0; // TODO: Prevent overflow
    int first_uop_index = 0;


    freopen("output.s", "w", stdout);
    FILE* console = fopen("console.txt", "w");
    PolNode* current_item = ass.first;

    while (current_item <= ass.last) {
        //fprint_comp_stacks(console, &(*compiler_stack), &(*unary_operator_stack), first_item_index, next_item_index, next_unop_index, file_str);
        switch (current_item->type) {
            case POL_FUNC_START:
                printf("	.globl	__"); print_to_next_token(current_item->start, file_str); printf("\n__");
                print_to_next_token(current_item->start, file_str); printf(":\n");
            break;
            case POL_LITERAL: {
                struct AsmItem item = {0, get_int_literal(current_item->start, file_str)};
                compiler_stack[next_item_index++] = item;
            }
            break;
            case POL_NEGATE:
            case POL_COMPLEMENT:
            case POL_NOT:
                if (unary_operator_stack[next_unop_index - 1] == current_item->type && compiler_stack[next_item_index - 1].unaries > 0) {
                    next_unop_index--;
                    compiler_stack[next_item_index - 1].unaries -= 1;
                    break;
                }
                unary_operator_stack[next_unop_index++] = current_item->type;
                compiler_stack[next_item_index - 1].unaries += 1;
            break;
            case POL_MINUS:  {
                enum Register out_register = 0;

                int item_1_index = next_item_index - 1;
                int item_2_index = next_item_index - 2;

                struct AsmItem* item_1 = &compiler_stack[item_1_index];
                struct AsmItem* item_2 = &compiler_stack[item_2_index];

                // 3 - 1
                // item_1 - item_2 -> item_2
                // subl %ebx, %eax

                // Apply unary operations to the items.
                if (prepare_for_binop(item_1, item_1_index, &current_in_use_register, held_registers)) {
                    apply_unaries(console, current_in_use_register, unary_operator_stack, next_unop_index, item_1->unaries);
                    next_unop_index -= item_1->unaries;
                    item_1->unaries = 0;
                }
                if (prepare_for_binop(item_2, item_2_index, &current_in_use_register, held_registers)) {
                    apply_unaries(console, current_in_use_register, unary_operator_stack, next_unop_index, item_2->unaries);
                    next_unop_index -= item_2->unaries;
                    item_2->unaries = 0;
                }
                if (!item_is_in_register(current_in_use_register, item_2_index, held_registers, &out_register)) {
                    held_registers[++current_in_use_register] = item_2_index;
                    printf("	movl   $0x%x, ", item_2->value);
                    print_register(current_in_use_register, true);
                    out_register = current_in_use_register;
                }
                if (!item_is_in_register(current_in_use_register, item_1_index, held_registers, &out_register)) {
                    held_registers[++current_in_use_register] = item_1_index;
                    printf("	movl   $0x%x, ", item_1->value);
                    print_register(current_in_use_register, true);
                    out_register = current_in_use_register;
                }
                #define IS_IN_REGISTER(item) item_is_in_register(current_in_use_register, item, held_registers, &out_register)
                
                IS_IN_REGISTER(item_1_index);
                enum Register i1_register = out_register;
                printf(" 	subl   ");
                // Both in registers
                print_register(out_register, false);
                printf(", ");
                IS_IN_REGISTER(item_2_index);
                enum Register i2_register = out_register;
                current_in_use_register--;

                #undef IS_IN_REGISTER
                print_register(out_register, true);
                held_registers[out_register] = item_2_index;
                next_item_index--;
                if (i2_register > i1_register) {
                    printf("	movl   "); print_register(i2_register, false); printf(", "); print_register(i1_register, true);
                    held_registers[i1_register] = item_2_index;
                }
            }
            break;
            case POL_ADD: {
                enum Register out_register = 0;

                int item_1_index = next_item_index - 1;
                int item_2_index = next_item_index - 2;

                struct AsmItem* item_1 = &compiler_stack[item_1_index];
                struct AsmItem* item_2 = &compiler_stack[item_2_index];
                // Apply unary operations to the items.
                if (prepare_for_binop(item_1, item_1_index, &current_in_use_register, held_registers)) {
                    apply_unaries(console, current_in_use_register, unary_operator_stack, next_unop_index, item_1->unaries);
                    next_unop_index -= item_1->unaries;
                    item_1->unaries = 0;
                }
                if (prepare_for_binop(item_2, item_2_index, &current_in_use_register, held_registers)) {
                    apply_unaries(console, current_in_use_register, unary_operator_stack, next_unop_index, item_2->unaries);
                    next_unop_index -= item_2->unaries;
                    item_2->unaries = 0;
                }

                #define IS_IN_REGISTER(item) item_is_in_register(current_in_use_register, item, held_registers, &out_register)

                if (IS_IN_REGISTER(item_1_index)) {
                    printf(" 	addl   ");
                    // Both in registers
                    if (IS_IN_REGISTER(item_2_index)) {
                        print_register(out_register, false);
                        printf(", ");
                        IS_IN_REGISTER(item_1_index);
                        current_in_use_register--;
                    }
                    // Item 1 in register, item 2 not.
                    else {
                        IS_IN_REGISTER(item_1_index);
                        printf("$0x%x, ", item_2->value);
                    }
                } 
                else {
                    // Item 2 in register, item 1 not
                    if (IS_IN_REGISTER(item_2_index)) {
                        printf(" 	addl   ");
                        printf("$0x%x, ", item_1->value);
                    } 
                    // Neither item in register
                    else {
                        held_registers[++current_in_use_register] = item_2_index;
                        printf("	movl   $0x%x, ", item_2->value);
                        print_register(current_in_use_register, true);
                        out_register = current_in_use_register;

                        printf(" 	addl   "); 
                        printf("$0x%x, ", item_1->value);
                    }
                }
                #undef IS_IN_REGISTER
                print_register(out_register, true);
                held_registers[out_register] = item_2_index;
                next_item_index--;
            }
            break;
            case POL_RETURN: {
                enum Register out_register = 0;
                if (!item_is_in_register(current_in_use_register, next_item_index - 1, held_registers, &out_register)) {
                    held_registers[++current_in_use_register] = next_item_index - 1;
                    printf("	movl   $0x%x, ", compiler_stack[next_item_index - 1].value);
                    print_register(current_in_use_register, true);
                    out_register = current_in_use_register;
                    //fprintf(console, "sorry my bad\n");
                }
                if (prepare_for_binop(&compiler_stack[next_item_index - 1], next_item_index - 1, &current_in_use_register, held_registers)) {
                    apply_unaries(console, current_in_use_register, unary_operator_stack, next_unop_index, compiler_stack[next_item_index - 1].unaries);
                    next_unop_index -= compiler_stack[next_item_index - 1].unaries;
                    compiler_stack[next_item_index - 1].unaries = 0;
                }
                printf(" 	ret\n");
            }
            break;
            case POL_ENDSTATEMENT:
                next_item_index = NO_REGISTER;
            break;
            default: break;
        }
        current_item++;
    }
}