#ifndef INCLUDES
#include "ALL_INCLUDES"
#endif

typedef enum PolTypes {
	POL_EMPTY,
	POL_FUNC_START,
	POL_INIT_FN_PARAM,
	POL_INIT_LOCAL_VAR,
	POL_RETURN,
	POL_LITERAL,
	POL_VAR,
	POL_APPEND_RETURN_TYPE, // Appends a return type to the current function
	
	POL_LPAREN, // (
	POL_RPAREN, // )

	// Unary operators
	POL_NEGATE, // -
	POL_COMPLEMENT, // ~
	POL_NOT, // !

	// Binary operators
	POL_ADD, // +
	POL_MINUS, // -
	POL_MUL, // *

	POL_ASSIGN, // =
	POL_TYPEASSIGN, // :

	// Other
	POL_ENDSTATEMENT, // ;
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
		case POL_VAR:
			printf("Var identifier: "); print_to_next_token(input.start, file_buffer); break;
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
		case POL_NEGATE:
			printf("Negate"); break;
		case POL_COMPLEMENT:
			printf("Complement"); break;
		case POL_NOT:
			printf("Not"); break;
		case POL_ADD:
			printf("Add"); break;
		case POL_MUL:
			printf("Multiply"); break;
		case POL_TYPEASSIGN:
			printf("Assign type"); break;
		case POL_MINUS:
			printf("Minus"); break;
		case POL_ENDSTATEMENT:
			printf("Statement end");
	}
}

void fprint_node(FILE* out, PolNode input, const string const file_buffer) {
	switch (input.type) {
		case POL_EMPTY:
			fprintf(out, "Do nothing"); break;
		case POL_VAR:
			fprintf(out, "Var identifier: "); fprint_to_next_token(out, input.start, file_buffer); break;
		case POL_FUNC_START:
			fprintf(out, "Declare fn: "); fprint_to_next_token(out, input.start, file_buffer); break;
		case POL_INIT_FN_PARAM:
			fprintf(out, "Load as fn param"); break;
		case POL_INIT_LOCAL_VAR:
			fprintf(out, "Declare var: "); fprint_to_next_token(out, input.start, file_buffer); break;
		case POL_ASSIGN:
			fprintf(out, "Assign"); break;
		case POL_RETURN:
			fprintf(out, "Return"); break;
		case POL_LITERAL:
			fprint_to_next_token(out, input.start, file_buffer); break;
		case POL_APPEND_RETURN_TYPE:
			fprintf(out, "Append return type to current function: "); fprint_to_next_token(out, input.start, file_buffer); break;
		case POL_NEGATE:
			fprintf(out, "Negate"); break;
		case POL_COMPLEMENT:
			fprintf(out, "Complement"); break;
		case POL_NOT:
			fprintf(out, "Not"); break;
		case POL_ADD:
			fprintf(out, "Add"); break;
		case POL_MUL:
			fprintf(out, "Multiply"); break;
		case POL_TYPEASSIGN:
			fprintf(out, "Assign type"); break;
	}
}

static void test() {sizeof(PolNode);}