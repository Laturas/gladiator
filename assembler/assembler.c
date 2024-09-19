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

FILE* asm_out = NULL;

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
			fprintf(asm_out, "%%eax");
		break;
		case EBX:
			fprintf(asm_out, "%%ebx");
		break;
		case ECX:
			fprintf(asm_out, "%%ecx");
		break;
		case EDX:
			fprintf(asm_out, "%%edx");
		break;
	}
	if (newline) {fprintf(asm_out, "\n");}
}

static bool item_is_in_register(enum Register current_in_use_register, int item_index, enum Register* held_registers, enum Register *out_register) {
	for (int i = EAX; i <= current_in_use_register; i++) {
		if (held_registers[i] == item_index) {*out_register = i; return true;}
	}
	*out_register = NO_REGISTER;
	return false;
}

/**
 * Applies unary operators in reverse order.
 * stack[top - 2] then stack[top - 1]
 */
void apply_unaries(enum Register reg, PolType* unary_operator_stack, int next_unop_index, int amount_to_apply) {
	//fprintf(output, "Item has to have %d unaries applied\n", amount_to_apply);
	for (int i = amount_to_apply; i > 0; i--) {

		//fprintf(output, "Applying unary: "); fprint_node(output, tmp, NULL); fprintf(output, "\n");
		switch (unary_operator_stack[next_unop_index - i]) {
			case POL_NEGATE:
				fprintf(asm_out, " 	neg    "); print_register(reg, true);
			break;
			case POL_COMPLEMENT:
				fprintf(asm_out, " 	not    "); print_register(reg, true);
			break;
			case POL_NOT:
				fprintf(asm_out, " 	cmpl   $0, "); print_register(reg, true);
				fprintf(asm_out, " 	movl   $0, "); print_register(reg, true); // Using xor clears the flags and thus can't be used to zero eax :c
				fprintf(asm_out, " 	sete   %%al\n");
			break;
		}
	}
}

struct AsmItem {
	int unaries;
	int value;
};

void print_comp_stacks(struct AsmItem* compiler_stack, PolType* unary_operator_stack, int first_item_index, int next_item_index, int next_unop_index, const string const file_str) {
	printf("Unary operators: [");
	if (next_unop_index) {
		PolNode wrapper = {unary_operator_stack[0], 0};
		print_node(wrapper, file_str);
	}
	for (int i = first_item_index; i < next_unop_index; i++) {
		PolNode wrapper = {unary_operator_stack[i], 0};
		printf(", ");
		print_node(wrapper, file_str);
	}
	printf("]\n");
	printf("Unary operator counts: [");
	if (next_item_index) {
		printf("%d", compiler_stack[0].unaries);
	}
	for (int i = first_item_index; i < next_item_index; i++) {
		printf(", ");
		printf("%d", compiler_stack[i].unaries);
	}
	printf("]\n");
}

bool prepare_for_binop(struct AsmItem* item_ref, int item_index, enum Register* register_reference, enum Register* registers) {
	enum Register _;
	struct AsmItem item = *item_ref;
	if (item.unaries > 0) {
		if (!item_is_in_register(*register_reference, item_index, registers, &_)) {
			registers[++(*register_reference)] = item_index;
			fprintf(asm_out, "	movl   $0x%x, ", item.value);
			print_register((*register_reference), true);
		}
		return true;
	}
	return false;
}
#define u16 unsigned short

typedef struct LVar {
	u16 byte_count;
	u16 type;
	u32 start;
} LVar;

bool idents_are_equal(u32 start_1, u32 start_2, const string const file_str) {
	int i = 0;
	while (1) {
		char chr = file_str->raw_string[start_1 + i];

		if (match_1(chr) != CUSTOM) {return true;}

		else if (chr != file_str->raw_string[start_2 + i]) {return false;}
		i++;
	}
	debug_print("Encountered infinite loop! Killing process"); exit(1);
}

static void asm_print_to_next_token(u32 start, const string const file_buffer) {
	while (start < file_buffer->length && match_1(file_buffer->raw_string[start]) == CUSTOM) {
		fprintf(asm_out, "%c", file_buffer->raw_string[start++]);
	}
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
	LVar vars[255] = {0};
	int vars_end = 0;

	asm_out = fopen("output.s", "w");
	PolNode* current_item = ass.first;

	while (current_item <= ass.last) {
		switch (current_item->type) {
			case POL_FUNC_START:
				fprintf(asm_out, "	.globl	__"); asm_print_to_next_token(current_item->start, file_str); fprintf(asm_out, "\n__");
				asm_print_to_next_token(current_item->start, file_str); fprintf(asm_out, ":\n");
			break;
			case POL_LITERAL: {
				struct AsmItem item = {0, get_int_literal(current_item->start, file_str)};
				compiler_stack[next_item_index++] = item;
			}
			break;
			case POL_INIT_LOCAL_VAR: {
			}
			break;
			case POL_VAR: {
				
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
					apply_unaries(current_in_use_register, unary_operator_stack, next_unop_index, item_1->unaries);
					next_unop_index -= item_1->unaries;
					item_1->unaries = 0;
				}
				if (prepare_for_binop(item_2, item_2_index, &current_in_use_register, held_registers)) {
					apply_unaries(current_in_use_register, unary_operator_stack, next_unop_index, item_2->unaries);
					next_unop_index -= item_2->unaries;
					item_2->unaries = 0;
				}
				if (!item_is_in_register(current_in_use_register, item_2_index, held_registers, &out_register)) {
					held_registers[++current_in_use_register] = item_2_index;
					fprintf(asm_out, "	movl   $0x%x, ", item_2->value);
					print_register(current_in_use_register, true);
					out_register = current_in_use_register;
				}
				#define IS_IN_REGISTER(item) item_is_in_register(current_in_use_register, item, held_registers, &out_register)
				
				IS_IN_REGISTER(item_1_index);
				enum Register i1_register = out_register;
				fprintf(asm_out, " 	subl   ");
				// Both in registers
				if (out_register) {
					print_register(out_register, false);
				}
				else {
					fprintf(asm_out, "$0x%x", item_1->value);
				}
				
				fprintf(asm_out, ", ");
				IS_IN_REGISTER(item_2_index);
				if (i1_register) {
					current_in_use_register--;
				}
				enum Register i2_register = out_register;

				#undef IS_IN_REGISTER
				print_register(out_register, true);
				held_registers[out_register] = item_2_index;
				next_item_index--;
				if ((i1_register) && i2_register > i1_register) {
					fprintf(asm_out, "	movl   "); print_register(i2_register, false); fprintf(asm_out, ", "); print_register(i1_register, true);
					held_registers[i1_register] = item_2_index;
				}
			}
			break;
			case POL_MUL: {
				enum Register out_register = 0;

				int item_1_index = next_item_index - 1;
				int item_2_index = next_item_index - 2;

				struct AsmItem* item_1 = &compiler_stack[item_1_index];
				struct AsmItem* item_2 = &compiler_stack[item_2_index];
				// Apply unary operations to the items.
				if (prepare_for_binop(item_1, item_1_index, &current_in_use_register, held_registers)) {
					apply_unaries(current_in_use_register, unary_operator_stack, next_unop_index, item_1->unaries);
					next_unop_index -= item_1->unaries;
					item_1->unaries = 0;
				}
				if (prepare_for_binop(item_2, item_2_index, &current_in_use_register, held_registers)) {
					apply_unaries(current_in_use_register, unary_operator_stack, next_unop_index, item_2->unaries);
					next_unop_index -= item_2->unaries;
					item_2->unaries = 0;
				}

				#define IS_IN_REGISTER(item) item_is_in_register(current_in_use_register, item, held_registers, &out_register)

				if (IS_IN_REGISTER(item_1_index)) {
					fprintf(asm_out, " 	imul   ");
					// Both in registers
					if (IS_IN_REGISTER(item_2_index)) {
						print_register(out_register, false);
						fprintf(asm_out, ", ");
						IS_IN_REGISTER(item_1_index);
						current_in_use_register--;
					}
					// Item 1 in register, item 2 not.
					else {
						IS_IN_REGISTER(item_1_index);
						fprintf(asm_out, "$0x%x, ", item_2->value);
					}
				} 
				else {
					// Item 2 in register, item 1 not
					if (IS_IN_REGISTER(item_2_index)) {
						fprintf(asm_out, " 	imul   ");
						fprintf(asm_out, "$0x%x, ", item_1->value);
					} 
					// Neither item in register
					else {
						held_registers[++current_in_use_register] = item_2_index;
						fprintf(asm_out, "	movl   $0x%x, ", item_2->value);
						print_register(current_in_use_register, true);
						out_register = current_in_use_register;

						fprintf(asm_out, " 	imul   "); 
						fprintf(asm_out, "$0x%x, ", item_1->value);
					}
				}
				#undef IS_IN_REGISTER
				print_register(out_register, true);
				held_registers[out_register] = item_2_index;
				next_item_index--;
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
					apply_unaries(current_in_use_register, unary_operator_stack, next_unop_index, item_1->unaries);
					next_unop_index -= item_1->unaries;
					item_1->unaries = 0;
				}
				if (prepare_for_binop(item_2, item_2_index, &current_in_use_register, held_registers)) {
					apply_unaries(current_in_use_register, unary_operator_stack, next_unop_index, item_2->unaries);
					next_unop_index -= item_2->unaries;
					item_2->unaries = 0;
				}

				#define IS_IN_REGISTER(item) item_is_in_register(current_in_use_register, item, held_registers, &out_register)
				enum Register i1_register = NO_REGISTER;
				enum Register i2_register = NO_REGISTER;

				if (IS_IN_REGISTER(item_1_index)) {
					i1_register = out_register;
					fprintf(asm_out, " 	addl   ");
					// Both in registers
					if (IS_IN_REGISTER(item_2_index)) {
						i2_register = out_register;
						if (i2_register < i1_register) {
							print_register(i1_register, false);
							fprintf(asm_out, ", ");
							out_register = i2_register;
							current_in_use_register--;
						}
						else {
							print_register(out_register, false);
							fprintf(asm_out, ", ");
							IS_IN_REGISTER(item_1_index);
							current_in_use_register--;
						}
					}
					// Item 1 in register, item 2 not.
					else {
						IS_IN_REGISTER(item_1_index);
						fprintf(asm_out, "$0x%x, ", item_2->value);
					}
				} 
				else {
					// Item 2 in register, item 1 not
					if (IS_IN_REGISTER(item_2_index)) {
						fprintf(asm_out, " 	addl   ");
						fprintf(asm_out, "$0x%x, ", item_1->value);
					} 
					// Neither item in register
					else {
						held_registers[++current_in_use_register] = item_2_index;
						fprintf(asm_out, "	movl   $0x%x, ", item_2->value);
						print_register(current_in_use_register, true);
						out_register = current_in_use_register;

						fprintf(asm_out, " 	addl   "); 
						fprintf(asm_out, "$0x%x, ", item_1->value);
					}
				}
				print_register(out_register, true);
				held_registers[out_register] = item_2_index;
				next_item_index--;
				#undef IS_IN_REGISTER
			}
			break;
			case POL_RETURN: {
				enum Register out_register = 0;
				if (!item_is_in_register(current_in_use_register, next_item_index - 1, held_registers, &out_register)) {
					held_registers[++current_in_use_register] = next_item_index - 1;
					fprintf(asm_out, "	movl   $0x%x, ", compiler_stack[next_item_index - 1].value);
					print_register(current_in_use_register, true);
					out_register = current_in_use_register;
				}
				if (prepare_for_binop(&compiler_stack[next_item_index - 1], next_item_index - 1, &current_in_use_register, held_registers)) {
					apply_unaries(current_in_use_register, unary_operator_stack, next_unop_index, compiler_stack[next_item_index - 1].unaries);
					next_unop_index -= compiler_stack[next_item_index - 1].unaries;
					compiler_stack[next_item_index - 1].unaries = 0;
				}
				fprintf(asm_out, " 	ret\n");
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