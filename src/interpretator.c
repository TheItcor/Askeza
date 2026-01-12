/* ==== Virtual Stack Machine Interpretator (Askeza) ====
 *
 * Version: 1.2.0 "Socrates"
 * Started: 16.12.2025
 * Github (documentation and etc): https://github.com/TheItcor/Askeza
 *
 *
 * I would be glad to receive your suggestions.
 * p.s. In the future, it is planned to reprogram the interpreter into a compiler.
 * */

/* To Do
 * [ ] True interpretation of main instructions:
 *     push, pop, swap, copy, print, input(?)
 * [ ] Interpretation of math instructions:
 *     add, mul, sub, div, idiv
 * [ ] Split the error header.
 * [ ] Add optional debugger
 * */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#include "throwErrors.h"?
//#include "stackRegisters.h"?

int STACK_SIZE = 1024;
int REGISTER_NUMBER = 4;
int stack_pointer = -1;             // if -1, then stack is empty
int line_number = 0;                // for throwing errors

// Data type
typedef enum {
    TYPE_CHAR,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_VOID   // = int 0
} DataType;

// Value of Stack/Register
typedef union {
    char c;
    int i;
    float f;
} Value;

// One element of Stack/Register
typedef struct {
    DataType type;
    Value value;
} Element;


// Stack/Register initilization

void init(Element *regstack, int size) {
    for (int i = 0; i < size; i++)
    {
        regstack[i].type = TYPE_VOID;
        regstack[i].value.i = 0;
    }
}


// Instuctions

typedef enum {
    OP_PUSH,
    OP_POP,
    OP_SWAP,
    OP_COPY,
    OP_PRINT,
    OP_INPUT,
    OP_GETLINES,
    OP_ADD,
    OP_MUL,
    OP_SUB,
    OP_DIV,
    OP_IDIV,
    OP_JMP,
    OP_IF,
    OP_RET,
    OP_END,
    OP_LABEL,
    OP_UNKNOW
} OP_CODE;

OP_CODE get_op_code(char *operation){
    if (strcmp(operation, "push") == 0) return OP_PUSH;
    if (strcmp(operation, "pop") == 0) return OP_POP;
    if (strcmp(operation, "swap") == 0) return OP_SWAP;
    if (strcmp(operation, "copy") == 0) return OP_COPY;
    if (strcmp(operation, "print") == 0) return OP_PRINT;
    if (strcmp(operation, "input") == 0) return OP_INPUT;
    if (strcmp(operation, "getline") == 0) return OP_GETLINES;
    if (strcmp(operation, "add") == 0) return OP_ADD;
    if (strcmp(operation, "mul") == 0) return OP_MUL;
    if (strcmp(operation, "sub") == 0) return OP_SUB;
    if (strcmp(operation, "div") == 0) return OP_DIV;
    if (strcmp(operation, "idiv") == 0) return OP_IDIV;
    if (strcmp(operation, "jmp") == 0) return OP_JMP;
    if (strcmp(operation, "ret") == 0) return OP_RET;
    if (strcmp(operation, "end") == 0) return OP_END;
    if (strcmp(operation, "if") == 0) return OP_IF;
    if (strchr(operation, ':') != NULL) return OP_LABEL;

    return OP_UNKNOW;
}


void push(Element *to, Element *from) {
    /* Pushing Element into Stack/Register
    * example:
    * push <dest> <src>
    *
    * <dest> can be register/stack
    * <src> can ve register/stack/char/int/float
    */
    to->value = from->value;
    to->type = from->type;

    from->type = TYPE_VOID;
    from->value.i = 0;
}


void pop(Element *stack) {
    /* Delete element from Stack */
    stack->type = TYPE_VOID;
    stack->value.i = 0;
    stack_pointer--;
}

void swap(Element *first, Element *second) {
    Element temp;
    temp.type = first->type;
    temp.value = first->value;

    first->type = second->type;
    first->value = second->value;

    second->type = temp.type;
    second->value = temp.value;
}

void prints() {}

void inputs() {}

void getline_() {}

void add() {}

void sub() {}

void div_() {}

void idiv() {}

void jmp() {}

void if_() {}

void ret() {}

void end() {}



// Funcs for errors (throwErrors.c in future):

void check_file_path(char *file_path)
{
    FILE *file = fopen(file_path, "r");
    if (file != NULL)
    {
        printf("[Debugger]: check_file_path - ok.\n");
        fclose(file);
    }
    else
    {
        fprintf(stderr, "[Read Error]: This file does not exist. Wrong path?.\n");
        exit(1);
    }
}

void check_file_extension(char *file_path)
{
    if (strrchr(file_path, '.') != NULL && strcmp(strrchr(file_path, '.'), ".ask") == 0)
    {
        printf("[Debugger]: check_file_extension - ok.\n");
    }
    else
    {
        fprintf(stderr, "[Read Error]: This file does have right extension .ask!\n");
        exit(1);
    }
}

void is_overflow() {
    if (stack_pointer+1 >= STACK_SIZE) {
        fprintf(stderr, "[Fatal Error]: stack overflow\n");
        exit(1);
    }
}

// Funcs for parse

void strip_comment(char *line) {
    char *comment = strstr(line, "//");
    if (comment) *comment = '\0';
}

Element* process_token(char* tk, Element* stack, Element* r_register, Element* registers, Element* temp_value) {
    /* Parses the string (argument) and then returns the address:
    * "s"   -> stack address
    * "rr"  -> return_register addres
    * "r0"  -> registers[0] adress
    * ""a"" -> temp_value (a)(char) address
    * "1"   -> temp_value (1)(int) address
    * "1.0" -> temp_value (1.0)(float) address
    */

    // Check for (stack)
    if (strcmp(tk, "s") == 0) {
        //printf("[process_token]: arg = stack\n");
        return stack;
    }
    // Check for "rr" (return register)
    else if (strcmp(tk, "rr") == 0) {
        //printf("[process_token]: arg = rr\n");
        return r_register;
    }
    // Check for registers (r0, r1, r2, ...)
    else if (tk[0] == 'r' && strlen(tk) > 1) {
        // Extract and validate register number
        char *num_part = tk + 1;
        while (*num_part) {
            if (!isdigit(*num_part)) break;
            num_part++;
        }

        if (*num_part == '\0') {
            int reg_index = atoi(tk + 1);
            //printf("[process_token]: arg = r%d\n", reg_index);
            return &registers[reg_index];
        } else {
            fprintf(stderr, "[Syntax Error]: Non-existent register on line %d.\n", line_number);
            exit(1);
        }
    }
    // Check if token has double quotes (char literal)
    else if (tk[0] == '"' && tk[strlen(tk)-1] == '"') {
        // Remove the quotes
        char content[strlen(tk)-1];
        strncpy(content, tk + 1, strlen(tk) - 2);
        content[strlen(tk)-2] = '\0';

        // Check if it's a single character
        if (strlen(content) == 1) {
            //printf("[process_token]: arg = char '%c'\n", content[0]);
            temp_value->type = TYPE_CHAR;
            temp_value->value.c = content[0];
            return temp_value;
        } else {
            //printf("[process_token]: arg = string (not a single char)\n");
            return NULL;
        }
    }
    // Token doesn't have double quotes = check if it's a number
    else {
        char *endptr;
        double d = strtod(tk, &endptr);

        if (*endptr == '\0') {
            // Valid number detected
            if (d == (int)d) {
                // Int
                //printf("[process_token]: arg = int %d\n", (int)d);
                temp_value->type = TYPE_INT;
                temp_value->value.i = (int)d;
            } else {
                // Float
                //printf("[process_token]: arg = float %f\n", d);
                temp_value->type = TYPE_FLOAT;
                temp_value->value.f = (float)d;
            }
            return temp_value;
        }
        else {
            // Not a number and not in quotes - check if it's a single character
            if (strlen(tk) == 1) {
                //printf("[process_token]: arg = single char '%c'\n", tk[0]);
                temp_value->type = TYPE_CHAR;
                temp_value->value.c = tk[0];
                return temp_value;
            } else {
                //fprintf(stderr, "[Fatal Error]: arg = unknown format '%s'\n", tk);
                exit(1);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    /* Arguments:
     * argv[0] - ask
     * argv[?] - debugger on
     * argv[?] - stack size
     * argv[?] - amount of registers
     * argv[argc-1] (last) - file name
     *
     * example:
     * ask dbg s512 r10 file.ask
     */


    // Analise all arguments. Throw errors.
    if (argc < 2)
    {
        fprintf(stderr, "[Read error]: Arguments needed!\n");
        exit(1);
    }

    check_file_path(argv[argc-1]);      // is real file or no?
    check_file_extension(argv[argc-1]); // is .ask file or no?

    // Initialization of Stack & Registers

    Element stack[STACK_SIZE];
    Element registers[REGISTER_NUMBER]; // [0] = r0, [1] = r1, ...
    Element return_register;            // rr
    Element temp_value;                 // for pushing separate char/int/float to stack/register: push s 5

    init(stack, STACK_SIZE);
    init(registers, REGISTER_NUMBER);

    printf("[Debugger]: Stack size: %d\n", STACK_SIZE);
    printf("[Debugger]: Amount of registers: %d\n", REGISTER_NUMBER);


    // Reading .ask file
    FILE *file = fopen(argv[argc-1], "r");

    char line[256];      // one line of file.ask
    char *tokens[8];     // array for tokens
    char *tk;            // token
    int tk_count;        // tokens count
    line_number = 0;     // line numbers for error throws
    short flag_main = 0; // is "main" started?

    while (fgets(line, sizeof(line), file) != NULL)
    {
        line_number++;
        tk_count = 0;

        // Delete comments
        strip_comment(line);

        // Finds the "main" label, if it's not there, then just skips everything else.
        if (!flag_main && !strstr(line, "main:")) continue;
        if (!flag_main) flag_main = 1;

        line[strcspn(line, "\n")] = '\0'; // no "\n" in my lines!

        // Tokenization:
        tk = strtok(line, " ");
        // If line is empty just skip it. Maybe TODO: skip empty (even with spaces) line before tokenization
        if (tk == NULL) continue;

        tokens[tk_count++] = tk;  // first token

        while ((tk = strtok(NULL, " ")) != NULL && tk_count < 8) {
            tokens[tk_count++] = tk;
        }

        // Tokens print test
        //for (int i = 0; i < tk_count; i++) {
        //    printf("token %d: %s\n", i, tokens[i]);
        //}

        OP_CODE operation = get_op_code(tokens[0]);

        switch (operation)
        {
        case OP_PUSH: {
            //printf("[Debugger]: %d. PUSH\n", line_number);
            // Reading arguments & get address of them
            Element *first_arg = process_token(tokens[1], stack, &return_register, registers, &temp_value);
            Element *second_arg = process_token(tokens[2], stack, &return_register, registers, &temp_value);

            if (first_arg == stack) {
                is_overflow();
                stack_pointer++;
                first_arg = &stack[stack_pointer];
            }
            if (second_arg == stack) {
                second_arg = &stack[stack_pointer];
                stack_pointer--;
            }

            push(first_arg, second_arg);

            break;
        }

        case OP_POP: {
            //printf("[Debugger]: %d. POP\n", line_number);
            pop(&stack[stack_pointer]);
            break;
        }

        case OP_SWAP: {
            printf("[Debugger]: %d. SWAP\n", line_number);

            Element *first_arg = process_token(tokens[1], stack, &return_register, registers, &temp_value);
            Element *second_arg = process_token(tokens[2], stack, &return_register, registers, &temp_value);

            swap(first_arg, second_arg);

            break;
        }

        case OP_COPY: {
            printf("[Debugger]: %d. COPY\n", line_number);
            break;
        }

        case OP_PRINT: {
            printf("[Debugger]: %d. PRINT\n", line_number);
            break;
        }

        case OP_INPUT: {
            printf("[Debugger]: %d. INPUT\n", line_number);
            break;
        }

        case OP_GETLINES: {
            printf("[Debugger]: %d. GETLINES\n", line_number);
            break;
        }

        case OP_ADD: {
            printf("[Debugger]: %d. ADD\n", line_number);
            break;
        }

        case OP_MUL: {
            printf("[Debugger]: %d. MUL\n", line_number);
            break;
        }

        case OP_SUB: {
            printf("[Debugger]: %d. SUB\n", line_number);
            break;
        }

        case OP_DIV: {
            printf("[Debugger]: %d. DIV\n", line_number);
            break;
        }

        case OP_IDIV: {
            printf("[Debugger]: %d. IDIV\n", line_number);
            break;
        }

        case OP_JMP: {
            printf("[Debugger]: %d. JMP\n", line_number);
            break;
        }

        case OP_IF: {
            printf("[Debugger]: %d. IF\n", line_number);
            break;
        }

        case OP_RET: {
            printf("[Debugger]: %d. RET\n", line_number);
            break;
        }

        case OP_END: {
            printf("[Debugger]: %d. END\n", line_number);
            goto end;
        }

        case OP_LABEL: {
            printf("[Debugger]: %d. MAIN!\n", line_number);
            break;
        }

        default:
            fprintf(stderr, "[Syntax error]: unknown instruction: \n%s \n   ...at the line %d.\n", line, line_number);
            break;
        }
    }

    end:

    if (flag_main == 0) fprintf(stderr, "[Fatal Error]: label main not found.");

    // Primitive debug
    // for (int i = 0; i < (int)(STACK_SIZE/16); i++) {
    //     printf("%d ", stack[i].value.i);
    // }
    //
    // printf("\n\n\n");
    // for (int i = 0; i < REGISTER_NUMBER; i++) {
    //     printf("%d ", registers[i].value.i);
    // }
    // printf("\n\n\n");
    // printf("[Stack pointer]: %d\n", stack_pointer);

    return 0;
}
