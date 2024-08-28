#ifndef INCLUDES
#include "ALL_INCLUDES"
#endif


Variable* parse_params(arena block_arena, Atom** atom_list) {
    enum Stage { // This is what the parser expects next in the sequence
        VarNameOptional, // In case of no params
        VarName,
        VarColon,
        VarType,
        VarEnd,
    };

    enum Stage current_stage = VarName;
    Atom current_atom;
    Variable current_var = EMPTY_VARIABLE;
    Variable* vars = NULL;
    
    while (1) {
        current_atom = *((*atom_list)++);
        switch (current_atom.token) {
            case CUSTOM: {
                switch (current_stage) {
                    case VarColon: printf("Parsing error on line %d: Found second identifier when : separator was expected\n", current_atom.line_number); exit(1);
                    case VarType: printf("Parsing error on line %d: Found second identifier when type was expected\n", current_atom.line_number); exit(1);
                    case VarEnd: printf("Parsing error on line %d: Current variable was not ended\n", current_atom.line_number); exit(1);
                    default: break;
                }
                current_var.name = current_atom.extra_str;
                current_stage = VarColon;
                break;
            }
            case COLON: {
                switch (current_stage) {
                    case VarType: printf("Parsing error on line %d: Found : separator when variable type was expected\n", current_atom.line_number); exit(1);
                    case VarEnd: printf("Parsing error on line %d: Found : separator when variable end was expected (did you miss a ',' or ')'?)\n", current_atom.line_number); exit(1);
                    default:
                    case VarName: printf("Parsing error on line %d: Found : separator when variable name was expected\n", current_atom.line_number); exit(1);
                    case VarColon: break;
                }
                current_stage = VarType;
                break;
            }
            case I32: {
                switch (current_stage) {
                    case VarEnd: printf("Parsing error on line %d: Found variable type when variable end was expected\n", current_atom.line_number); exit(1);
                    default:
                    case VarName: printf("Parsing error on line %d: Found variable name when type was expected\n", current_atom.line_number); exit(1);
                    case VarColon: printf("Parsing error on line %d: Found variable type when : separator was expected\n", current_atom.line_number); exit(1);
                    case VarType: break;
                }
                current_var.variable_type = TYPE_I32;
                current_stage = VarEnd;
                break;
            }
            case COMMA: {
                switch (current_stage) {
                    default:
                    case VarName:
                    case VarColon:
                    case VarType: printf("Parsing error on line %d: Variable unexpectedly ended!\n", current_atom.line_number); exit(1);
                    case VarEnd: break;
                }
                current_stage = VarName;
                Variable* return_block = apush(block_arena, current_var);
                *return_block = (Variable){current_var.type, current_var.name, current_var.variable_type};
                vars = (!vars) ? return_block : vars;
                break;
            }
            case CLOSE_PAREN: {
                switch (current_stage) {
                    case VarColon:
                    case VarType: printf("Parsing error on line %d: Variable unexpectedly ended!\n", current_atom.line_number); exit(1);
                    default: break;
                }
                Variable* return_block = apush(block_arena, current_var);
                *return_block = (Variable){current_var.type, current_var.name, current_var.variable_type};
                
                vars = (!vars) ? return_block : vars;
                return vars;
            }
            default: {
                printf("Parsing error on line %d: Unexpected token: ", current_atom.line_number); print_atom(current_atom, 0); fflush(stdout);
                exit(1);
            }
        }
    }
}

#define ptrless_print(a) print(&a)

Function* function_block(arena block_arena, string name, Atom** atom_list) {
    Function* new_func = NULL;
    {
        Function _new_func = {FUNCTION, name, 0, 0, 0};
        new_func = apush(block_arena, _new_func);
    }
    Atom** ls_cop = atom_list;
    new_func->params = parse_params(block_arena, ls_cop); // Hardcoded
    //print_var_type(new_func->params->variable_type);
    //ptrless_print(((Variable*)new_func->params)->name);
    //new_func->returns = parse_returns();
}