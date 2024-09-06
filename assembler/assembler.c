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

//void write_to_asm(arena asm_arena, _String to_write) {
//    char* pushed = apush_size(asm_arena, to_write.raw_string, to_write.length);
//    for (int i = 0; i < to_write.length; i++) {
//        pushed[i] = to_write.raw_string[i];
//    }
//}
//void write_token_to_asm(arena asm_arena, u32 start, const string const file_str) {
//    int i = start;
//    while (match_1(file_str->raw_string[i]) != CUSTOM) {
//        i++;
//    }
//    char* element = file_str->raw_string[start];
//    char* pushed = apush_size(asm_arena, element, i - start);
//    for (int j = start; j < i + start; j++) {
//        pushed[j] = file_str->raw_string[j];
//    }
//}
//
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
//
//void generate_assembly(struct AbstractSyntaxStream ass, const string const file_str) {
//    arena asm_arena = aalloc(4194304);
//    string asm_string = newstr(asm_arena);
//
//    enum Register current = NO_REGISTER;
//    PolNode* first_cp = ass.first;
//    
//    while (first_cp <= ass.last) {
//        switch (first_cp->type) {
//            case POL_FUNC_START:
//                printf("	.globl	__"); print_to_next_token(first_cp->start,file_str); printf("\n__");
//                print_to_next_token(first_cp->start,file_str); printf(":\n");
//            break;
//            case POL_LITERAL:
//                current++;
//                printf("	movl   $"); printf("0x%x, ",get_int_literal(first_cp->start, file_str)); print_register(current, true);
//            break;
//            case POL_NEGATE:
//                printf(" 	neg    "); print_register(current, true);
//            break;
//            case POL_COMPLEMENT:
//                printf(" 	not    "); print_register(current, true);
//            break;
//            case POL_NOT:
//                printf(" 	cmpl   $0, "); print_register(current, true);
//                printf(" 	movl   $0, "); print_register(current, true); // Using xor clears the flags and thus can't be used to zero eax :c
//                printf(" 	sete   %%al\n");
//            break;
//            case POL_ADD:
//                printf(" 	addl   "); print_register(current, false); printf(", "); print_register(--current,true);
//            break;
//            case POL_RETURN:
//                printf(" 	ret\n");
//            break;
//        }
//        first_cp++;
//    }
//}

#define ADVANCE_DATA while (first_data->type != POL_LITERAL && first_data < ass.last) {first_data++;}
#define MOVE_IF_NOT(register) if (current == register) {ADVANCE_DATA printf("	movl   $"); printf("0x%x, ",get_int_literal(first_data->start, file_str)); print_register(register + 1, true); current = register + 1; first_data++;}

//#define clear_unaries(item_index)

bool item_is_in_register(enum Register current_in_use_register, int item_index, enum Register* held_registers, enum Register *out_register) {
    for (int i = EAX; i <= current_in_use_register; i++) {
        if (held_registers[i] == item_index) {*out_register = i; return true;}
    }
    *out_register = NO_REGISTER;
    return false;
}

void apply_unaries(FILE* output, enum Register reg, PolType* unary_operator_stack, int next_unop_index, int amount_to_apply) {
    for (int i = amount_to_apply; i > 0; i--) {
        switch (unary_operator_stack[next_unop_index - i]) {
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

void fprint_comp_stacks(FILE* output, struct AsmItem* compiler_stack, PolType* unary_operator_stack, int next_item_index, int next_unop_index, const string const file_str) {
    fprintf(output, "Unary operators: [");
    if (next_unop_index) {
        PolNode wrapper = {unary_operator_stack[0], 0};
        fprint_node(output, wrapper, file_str);
    }
    for (int i = 1; i < next_unop_index; i++) {
        PolNode wrapper = {unary_operator_stack[i], 0};
        fprintf(output, ", ");
        fprint_node(output, wrapper, file_str);
    }
    fprintf(output, "]\n");
    fprintf(output, "Unary operator counts: [");
    if (next_item_index) {
        fprintf(output, "%d", compiler_stack[0].unaries);
    }
    for (int i = 1; i < next_item_index; i++) {
        fprintf(output, ", ");
        fprintf(output, "%d", compiler_stack[i].unaries);
    }
    fprintf(output, "]\n");
}

void generate_asm(struct AbstractSyntaxStream ass, const string const file_str) {
    enum Register held_registers[5] = {0}; // Holds indexes into compiler stack, knows when to reuse a register.
    enum Register current_in_use_register = NO_REGISTER;
    struct AsmItem compiler_stack[255] = {0};
    PolType unary_operator_stack[255] = {0};
    int next_item_index = 0;
    int next_unop_index = 0;

    #define DATA_STACK_TOP compiler_stack[next_item_index - 1]
    #define OP_STACK_TOP unary_operator_stack[next_unop_index - 1]

    freopen("output.s", "w", stdout);
    FILE* console = fopen("console.txt", "w");
    PolNode* current_item = ass.first;

    while (current_item <= ass.last) {
        fprint_comp_stacks(console, &(*compiler_stack), &(*unary_operator_stack), next_item_index, next_unop_index, file_str);
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
                if (OP_STACK_TOP == current_item->type && DATA_STACK_TOP.unaries > 0) {
                    fprintf(console, "Removing from stack :(\n");
                    next_unop_index--;
                    DATA_STACK_TOP.unaries -= 1;
                    break;
                }
                fprintf(console, "Adding to stack :D\n");
                unary_operator_stack[next_unop_index++] = current_item->type;
                DATA_STACK_TOP.unaries += 1;
            break;
            case POL_ADD: {
                //fprint_comp_stacks(console, &(*compiler_stack), &(*unary_operator_stack), next_item_index, next_unop_index, file_str);
                enum Register out_register = 0;
                for (int offset = 1; offset <= 2; offset++) {
                    if (compiler_stack[next_item_index - offset].unaries > 0) {
                        if (!item_is_in_register(current_in_use_register, next_item_index - offset, held_registers, &out_register)) {
                            held_registers[++current_in_use_register] = next_item_index - offset;
                            printf("	movl   $0x%x, ", compiler_stack[next_item_index - offset].value);
                            print_register(current_in_use_register, true);
                            out_register = current_in_use_register;
                            //fprintf(console, "Moved item to register %d\n", current_in_use_register);
                        }
                        apply_unaries(console, out_register, unary_operator_stack, next_unop_index, compiler_stack[next_item_index - offset].unaries);
                        next_unop_index -= compiler_stack[next_item_index - offset].unaries;
                        compiler_stack[next_item_index - offset].unaries = 0;
                    }
                }
                fprintf(console, "Case %d\n\n", item_is_in_register(current_in_use_register, next_item_index - 1, held_registers, &out_register) + item_is_in_register(current_in_use_register, next_item_index - 2, held_registers, &out_register));
                switch (item_is_in_register(current_in_use_register, next_item_index - 1, held_registers, &out_register) 
                       + item_is_in_register(current_in_use_register, next_item_index - 2, held_registers, &out_register)
                    ) {
                    case 0: // Both int literals
                        held_registers[++current_in_use_register] = next_item_index - 2;
                        printf("	movl   $0x%x, ", compiler_stack[next_item_index - 2].value);
                        print_register(current_in_use_register, true);
                        out_register = current_in_use_register;

                        printf(" 	addl   "); printf("$0x%x, ", DATA_STACK_TOP.value); print_register(current_in_use_register, true);
                    break;
                    case 1: // One is a literal and one is a register
                        printf(" 	addl   ");
                        if (out_register) {
                            printf("$0x%x, ", compiler_stack[next_item_index - 1].value);
                            print_register(out_register, true);
                            break;
                        }
                        item_is_in_register(current_in_use_register, next_item_index - 1, held_registers, &out_register);
                        printf("$0x%x, ", compiler_stack[next_item_index - 2].value);
                        print_register(out_register, true);
                    break;
                    case 2: // Both are registers
                        printf(" 	addl   ");
                        item_is_in_register(current_in_use_register, next_item_index - 2, held_registers, &out_register);
                        print_register(out_register, false);
                        printf(", ");
                        item_is_in_register(current_in_use_register, next_item_index - 1, held_registers, &out_register);
                        print_register(out_register, true);
                        current_in_use_register--;
                    break;
                }
                held_registers[out_register] = next_item_index - 2;
                next_item_index--;
            }
            break;
            case POL_RETURN: {
                enum Register out_register = 0;
                if (!item_is_in_register(current_in_use_register, next_item_index - 1, held_registers, &out_register)) {
                    held_registers[++current_in_use_register] = next_item_index - 1;
                    printf("	movl   $0x%x, ", DATA_STACK_TOP.value);
                    print_register(current_in_use_register, true);
                    out_register = current_in_use_register;
                    //fprintf(console, "Moved item to register %d\n", current_in_use_register);
                }
                apply_unaries(console, out_register, unary_operator_stack, next_unop_index, DATA_STACK_TOP.unaries);
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

void generate_assembly(struct AbstractSyntaxStream ass, const string const file_str) {
    enum Register current = NO_REGISTER;
    freopen("output.s", "w", stdout);
    FILE* console = fopen("console.txt", "w");
    PolNode* first_data = ass.first;
    PolNode* first_operation = ass.first;
    while (first_operation <= ass.last) {
        //fprint_node(console,*first_operation,file_str); fprintf(console, " | "); fprint_node(console,*first_data,file_str); fprintf(console, "\n");
        //fprintf(console, "\nfirst_operation: %d\nfirst_data: %d\n", first_operation - ass.first, first_data - ass.first);
        switch (first_operation->type) {
            case POL_FUNC_START:
                printf("	.globl	__"); print_to_next_token(first_operation->start,file_str); printf("\n__");
                print_to_next_token(first_operation->start,file_str); printf(":\n");
            break;
            case POL_LITERAL: break;
            case POL_NEGATE:
                MOVE_IF_NOT(NO_REGISTER)
                printf(" 	neg    "); print_register(current, true);
                current++;
            break;
            case POL_COMPLEMENT:
                MOVE_IF_NOT(NO_REGISTER)
                printf(" 	not    "); print_register(current, true);
                current++;
            break;
            case POL_NOT:
                MOVE_IF_NOT(NO_REGISTER)
                printf(" 	cmpl   $0, "); print_register(EAX, true);
                printf(" 	movl   $0, "); print_register(EAX, true); // Using xor clears the flags and thus can't be used to zero eax :c
                printf(" 	sete   %%al\n");
            break;
            case POL_ADD:
                MOVE_IF_NOT(NO_REGISTER)
                if (current == EAX) {
                    ADVANCE_DATA

                    printf("	movl   $"); printf("0x%x, ",get_int_literal(first_data->start, file_str)); print_register(EBX, true);
                    current = EBX;
                    first_data++;
                }
                printf(" 	addl   "); print_register(current - 1, false); printf(", "); print_register(current, true);
                current--;
            break;
            case POL_RETURN:
                printf(" 	ret\n");
            break;
            case POL_ENDSTATEMENT:
                current = NO_REGISTER;
            break;
            default: break;
        }
        first_operation++;
    }
    //while (first_data <= ass.last) {
    //    switch (first_data->type) {
    //        case POL_FUNC_START:
    //            printf("	.globl	__"); print_to_next_token(first_data->start,file_str); printf("\n__");
    //            print_to_next_token(first_data->start,file_str); printf(":\n");
    //        break;
    //        case POL_LITERAL:
    //            first_operation = first_data;
    //            while (first_operation->type != POL_LITERAL) {first_operation++;}
    //            printf("	movl   $"); printf("0x%x, ",get_int_literal(first_data->start, file_str)); print_register(current, true);
    //        break;
    //        case POL_NEGATE:
    //            printf(" 	neg    "); print_register(current, true);
    //        break;
    //        case POL_COMPLEMENT:
    //            printf(" 	not    "); print_register(current, true);
    //        break;
    //        case POL_NOT:
    //            printf(" 	cmpl   $0, "); print_register(current, true);
    //            printf(" 	movl   $0, "); print_register(current, true); // Using xor clears the flags and thus can't be used to zero eax :c
    //            printf(" 	sete   %%al\n");
    //        break;
    //        case POL_ADD:
    //            printf(" 	addl   "); print_register(current, false); printf(", "); print_register(--current,true);
    //        break;
    //        case POL_RETURN:
    //            printf(" 	ret\n");
    //        break;
    //    }
    //    first_data++;
    //}
}