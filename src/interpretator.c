/* ==== Virtual Stack Machine Interpretator (Askeza) ====
 *
 * Version: 2.0.0 "Socrates"
 * Started: 16.12.2025
 * Github (documentation and etc): https://github.com/TheItcor/Askeza
 *
 *
 * I would be glad to receive your suggestions.
 * p.s. In the future, it is planned to reprogram the interpreter into a compiler.
 * */

/* To Do
 * [x] True interpretation of main instructions:
 *     push, pop, swap, copy, print, input(?)
 * [x] Interpretation of math instructions:
 *     add, mul, sub, div, mod
 * [ ] Split the error header.
 * [x] Add optional debugger
 * */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <sys/types.h>

//#include "throwErrors.h"?
//#include "stackRegisters.h"?

// Optional debugger
#define DPRINT(...) (debug && printf(__VA_ARGS__))
int debug = 0;

// Global Virtual Machine state
#define STACK_SIZE 1024             // this is a temporary solution. In the future, the value can be changed before program starts
#define REGISTER_NUMBER 4
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


// for debugging
const char* type_name(DataType t) {
    switch (t) {
        case TYPE_CHAR:  return "TYPE_CHAR";
        case TYPE_INT:   return "TYPE_INT";
        case TYPE_FLOAT: return "TYPE_FLOAT";
        case TYPE_VOID:  return "TYPE_VOID";
        default:         return "UNKNOWN";
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
    OP_MOD,
    OP_JMP,
    OP_CALL,
    OP_IF,
    OP_RET,
    OP_END,
    OP_LABEL,
    OP_UNKNOW,
    OP_IMPORT,
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
    if (strcmp(operation, "mod") == 0) return OP_MOD;
    if (strcmp(operation, "jmp") == 0) return OP_JMP;
    if (strcmp(operation, "call") == 0) return OP_CALL;
    if (strcmp(operation, "ret") == 0) return OP_RET;
    if (strcmp(operation, "end") == 0) return OP_END;
    if (strcmp(operation, "if") == 0) return OP_IF;
    if (strcmp(operation, "import") == 0) return OP_IMPORT;
    if (strchr(operation, ':') != NULL) return OP_LABEL;

    return OP_UNKNOW;
}


void push(Element *to, Element *from) {
    /* Pushing Element into Stack/Register
    * example:
    * push <dest> <src>
    *
    * <dest> can be register/stack
    * <src> can be register/stack/char/int/float
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
    /* Swaps to elements */
    Element temp;
    temp.type = first->type;
    temp.value = first->value;

    first->type = second->type;
    first->value = second->value;

    second->type = temp.type;
    second->value = temp.value;
}


void copy(Element *paste, Element *copy) {
    /* Copy element */
    paste->type = copy->type;
    paste->value = copy->value;
}


void prints(Element *element) {
    /* Output element
     * Only one element each command
     */

    if (element->type == TYPE_CHAR) {
        printf("%c\n", element->value.c);
    } else if (element->type == TYPE_FLOAT) {
        printf("%f\n", element->value.f);
    } else if (element->type == TYPE_INT) {
        printf("%d\n", element->value.i);
    } else {
        return;
    }
}


void inputs(char type, Element *element) {
    /* Input element
     * Safe value in element from output
     * example:
     * input int r0
     * input float r0
     * input char r0
     */

    switch (type) {

        // input int -> element
        case 'i':
            element->type = TYPE_INT;
            if (scanf("%d", &element->value.i) != 1) {
                fprintf(stderr, "[Fatal Error][%d]: Failed to read integer\n", line_number);
                exit(1);
            }

            break;

        // input float -> element
        case 'f':
            element->type = TYPE_FLOAT;
            if (scanf("%f", &element->value.f) != 1) {
                fprintf(stderr, "[Fatal Error][%d]: Failed to read float\n", line_number);
                exit(1);
            }

            break;

        // input char -> element
        case 'c':
            element->type = TYPE_CHAR;
            if (scanf(" %c", &element->value.c) != 1) {
                fprintf(stderr, "[Fatal Error][%d]: Failed to read char\n", line_number);
                exit(1);
            }

            break;

        default:
            fprintf(stderr, "[Syntax Error][%d]: Unknow type on input", line_number);
            exit(1);
    }
}


void getline_() {}


void add(Element *first, Element *second) {
    /* Addition operation
     * first = first + second
     */

    if (first->type == TYPE_CHAR || second->type == TYPE_CHAR) {
        fprintf(stderr, "[Fatal Error][%d]: Attempt to add char!\n", line_number);
        exit(1);
    }

    // this parts looks specific...
    // both int
    if (first->type == TYPE_INT && second->type == TYPE_INT) {
        first->value.i += second->value.i;
    }
    // both float
    else if (first->type == TYPE_FLOAT && second->type == TYPE_FLOAT) {
        first->value.f += second->value.f;
    }
    // int + float -> float
    else if (first->type == TYPE_INT && second->type == TYPE_FLOAT) {
        float a = (float)first->value.i;
        first->value.f = a + second->value.f;
        first->type = TYPE_FLOAT;
    }
    // float + int -> float
    else if (first->type == TYPE_FLOAT && second->type == TYPE_INT) {
        first->value.f += (float)second->value.i;
    }
    else {
        fprintf(stderr, "[Fatal Error[%d]: Invalid types for add\n", line_number);
        exit(1);
    }
}


void sub(Element *first, Element *second) {
    /* Subtraction operation
     * first = first - second
     */

    if (first->type == TYPE_CHAR || second->type == TYPE_CHAR) {
        fprintf(stderr, "[Fatal Error][%d]: Attempt to sub char!\n", line_number);
        exit(1);
    }

    // both int
    if (first->type == TYPE_INT && second->type == TYPE_INT) {
        first->value.i -= second->value.i;
    }
    // both float
    else if (first->type == TYPE_FLOAT && second->type == TYPE_FLOAT) {
        first->value.f -= second->value.f;
    }
    // int - float -> float
    else if (first->type == TYPE_INT && second->type == TYPE_FLOAT) {
        float a = (float)first->value.i;
        first->value.f = a - second->value.f;
        first->type = TYPE_FLOAT;
    }
    // float - int -> float
    else if (first->type == TYPE_FLOAT && second->type == TYPE_INT) {
        first->value.f -= (float)second->value.i;
    }
    else {
        fprintf(stderr, "[Fatal Error][%d]: Invalid types for sub\n", line_number);
        exit(1);
    }
}


void mul(Element *first, Element *second) {
    /* Multiplication operation
     * first = first * second
     */

    if (first->type == TYPE_CHAR || second->type == TYPE_CHAR) {
        fprintf(stderr, "[Fatal Error][%d]: Attempt to mul char!\n", line_number);
        exit(1);
    }

    // both int
    if (first->type == TYPE_INT && second->type == TYPE_INT) {
        first->value.i *= second->value.i;
    }
    // both float
    else if (first->type == TYPE_FLOAT && second->type == TYPE_FLOAT) {
        first->value.f *= second->value.f;
    }
    // int * float -> float
    else if (first->type == TYPE_INT && second->type == TYPE_FLOAT) {
        float a = (float)first->value.i;
        first->value.f = a * second->value.f;
        first->type = TYPE_FLOAT;
    }
    // float * int -> float
    else if (first->type == TYPE_FLOAT && second->type == TYPE_INT) {
        first->value.f *= (float)second->value.i;
    }
    else {
        fprintf(stderr, "[Fatal Error][%d]: Invalid types for mul\n", line_number);
        exit(1);
    }
}


void div_(Element *first, Element *second) {
    /* Division operation
     * first = first / second
     */

    if (first->type == TYPE_CHAR || second->type == TYPE_CHAR) {
        fprintf(stderr, "[Fatal Error][%d]: Attempt to div char!\n", line_number);
        exit(1);
    }

    // Checking for division by zero for int and float (0.0f)
    if ((second->type == TYPE_INT   && second->value.i == 0) ||
        (second->type == TYPE_FLOAT && second->value.f == 0.0f)) {
        fprintf(stderr, "[Fatal Error][%d]: Division by zero!\n", line_number);
        exit(1);
    }

    // both int
    if (first->type == TYPE_INT && second->type == TYPE_INT) {
        first->value.i /= second->value.i;
    }
    // both float
    else if (first->type == TYPE_FLOAT && second->type == TYPE_FLOAT) {
        first->value.f /= second->value.f;
    }
    // int / float -> float
    else if (first->type == TYPE_INT && second->type == TYPE_FLOAT) {
        float a = (float)first->value.i;
        first->value.f = a / second->value.f;
        first->type = TYPE_FLOAT;
    }
    // float / int -> float
    else if (first->type == TYPE_FLOAT && second->type == TYPE_INT) {
        first->value.f /= (float)second->value.i;
    }
    else {
        fprintf(stderr, "[Fatal Error][%d]: Invalid types for div\n", line_number);
        exit(1);
    }
}


void mod(Element *first, Element *second) {
    /* modulo operation
     * first = first % second
     */

    if (first->type == TYPE_CHAR || second->type == TYPE_CHAR) {
        fprintf(stderr, "[Fatal Error][%d]: Attempt to modulo char!\n", line_number);
        exit(1);
    }

    // Reject char types
    if (first->type == TYPE_CHAR || second->type == TYPE_CHAR) {
        fprintf(stderr, "[Fatal Error][%d]: Cannot apply modulo to char\n", line_number);
        exit(1);
    }

    // Reject void types
    if (first->type == TYPE_VOID || second->type == TYPE_VOID) {
        fprintf(stderr, "[Fatal Error][%d]: Cannot apply modulo to void\n", line_number);
        exit(1);
    }

    // Check for division by zero (int zero or float near zero)
    if ((second->type == TYPE_INT && second->value.i == 0) ||
        (second->type == TYPE_FLOAT && fabsf(second->value.f) < 1e-6f)) {
        fprintf(stderr, "[Fatal Error][%d]: Modulo by zero\n", line_number);
        exit(1);
    }

    // Both int
    if (first->type == TYPE_INT && second->type == TYPE_INT) {
        first->value.i %= second->value.i;
    }
    // Both float
    else if (first->type == TYPE_FLOAT && second->type == TYPE_FLOAT) {
        first->value.f = fmodf(first->value.f, second->value.f);
    }
    // int % float -> float
    else if (first->type == TYPE_INT && second->type == TYPE_FLOAT) {
        float a = (float)first->value.i;
        first->value.f = fmodf(a, second->value.f);
        first->type = TYPE_FLOAT;
    }
    // float % int -> float
    else if (first->type == TYPE_FLOAT && second->type == TYPE_INT) {
        first->value.f = fmodf(first->value.f, (float)second->value.i);
    }
    else {
        fprintf(stderr, "[Fatal Error][%d]: Invalid types for modulo\n", line_number);
        exit(1);
    }
}


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
        //printf("[Debugger]: check_file_path - ok.\n");
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
        //printf("[Debugger]: check_file_extension - ok.\n");
    }
    else
    {
        fprintf(stderr, "[Read Error]: This file does have right extension .ask!\n");
        exit(1);
    }
}

void is_overflow() {
    if (stack_pointer+1 >= STACK_SIZE) {
        fprintf(stderr, "[Fatal Error][%d]: Stack overflow.\n", line_number);
        exit(1);
    }
}

void is_underflow() {
    if (stack_pointer-1 <= -2) {
        fprintf(stderr, "[Fatal Error][%d]: Stack underflow.\n", line_number);
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
            fprintf(stderr, "[Syntax Error][%d]: Non-existent register.\n", line_number);
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
                //fprintf(stderr, "[Fatal Error]: Arg = unknown format '%s'\n", tk);
                exit(1);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    /* Arguments:
     * argv[0] - ask
     * argv[1] - debugger on
     * argv[?] - stack size
     * argv[?] - amount of registers
     * argv[argc-1] (last) - file name
     *
     * example:
     * ask -d s512 r10 file.ask
     */

    // Analise all arguments. Throw errors.
    debug |= !strcmp(argv[1], "-d");
    DPRINT("===== DEBUGGER ON! =====\n");

/* ----------------------------------------------------------- */
// Global elements
Element stack[STACK_SIZE];
Element registers[REGISTER_NUMBER]; // [0] = r0, [1] = r1, ...
Element return_register;            // rr
Element temp_value;                 // for pushing separate char/int/float to stack/register: push s 5
/* ----------------------------------------------------------- */

        line[strcspn(line, "\n")] = '\0'; // no "\n" in my lines!

        // Tokenization:
        tk = strtok(line, " ");
        // If line is empty just skip it. Maybe TODO: skip empty (even with spaces) line before tokenization
        if (tk == NULL) continue;

void execute_instruction(Instruction *instr) {
    int ln = instr->line_number;   // use stored line number for errors & debug
    line_number = ln;              // update global so error functions know the right line
    DPRINT("[%d]: ", ln);

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
            DPRINT("[%d]: PUSH %s <- %s  |  ", line_number, tokens[1], tokens[2]);
            // Reading arguments & get address of them
            Element *first_arg = process_token(tokens[1], stack, &return_register, registers, &temp_value);
            Element *second_arg = process_token(tokens[2], stack, &return_register, registers, &temp_value);

            if (first_arg == stack) {
                is_overflow();
                stack_pointer++;
                first_arg = &stack[stack_pointer];
            }
            if (second_arg == stack) {
                is_underflow();
                second_arg = &stack[stack_pointer];
                stack_pointer--;
            }

            push(first_arg, second_arg);

            DPRINT("[SP: %d -> %d]\n", stack_pointer, stack_pointer+1);
            break;
        }

        case OP_POP: {
            DPRINT("[%d]: POP  |  ", line_number);
            pop(&stack[stack_pointer]);

            DPRINT("[SP: %d -> %d]\n", stack_pointer, stack_pointer-1);
            break;
        }

        case OP_SWAP: {
            Element *first_arg = process_token(tokens[1], stack, &return_register, registers, &temp_value);
            Element *second_arg = process_token(tokens[2], stack, &return_register, registers, &temp_value);

            swap(first_arg, second_arg);

            break;
        }

        case OP_COPY: {
            DPRINT("[%d]: COPY\n", line_number);
            Element *first_arg = process_token(tokens[1], stack, &return_register, registers, &temp_value);
            Element *second_arg = process_token(tokens[2], stack, &return_register, registers, &temp_value);

            if (first_arg == stack) {
                is_overflow();
                stack_pointer++;
                first_arg = &stack[stack_pointer];
            }

            copy(first_arg, second_arg);

            break;
        }

        case OP_PRINT: {
            DPRINT("[%d]: PRINT: ", line_number);
            Element *first_arg = process_token(tokens[1], stack, &return_register, registers, &temp_value);

            if (first_arg == stack) {
                first_arg = &stack[stack_pointer];
            }

            prints(first_arg);
            break;
        }

        case OP_INPUT: {
            DPRINT("[%d]: INPUT %s <- ", line_number, tokens[2]);

            //Element *first_arg = process_token(tokens[1], stack, &return_register, registers, &temp_value);
            Element *second_arg = process_token(tokens[2], stack, &return_register, registers, &temp_value);

            if (second_arg == stack) {
                is_overflow();
                stack_pointer++;
                second_arg = &stack[stack_pointer];
            }

            //send only the first char of the type: float -> f, int -> i, char -> c
            inputs(tokens[1][0], second_arg);
            break;
        }

        case OP_GETLINES: {
            DPRINT("[%d]: GETLINES\n", line_number);
            break;
        }

        case OP_ADD: {
            DPRINT("[%d]: ADD\n", line_number);

            Element *first_arg = process_token(tokens[1], stack, &return_register, registers, &temp_value);
            Element *second_arg = process_token(tokens[2], stack, &return_register, registers, &temp_value);

            add(first_arg, second_arg);

            break;
        }

        case OP_MUL: {
            DPRINT("[%d]: MUL\n", line_number);

            Element *first_arg = process_token(tokens[1], stack, &return_register, registers, &temp_value);
            Element *second_arg = process_token(tokens[2], stack, &return_register, registers, &temp_value);

            mul(first_arg, second_arg);

            break;
        }

        case OP_SUB: {
            DPRINT("[%d]: SUB\n", line_number);

            Element *first_arg = process_token(tokens[1], stack, &return_register, registers, &temp_value);
            Element *second_arg = process_token(tokens[2], stack, &return_register, registers, &temp_value);

            sub(first_arg, second_arg);

            break;
        }

        case OP_DIV: {
            DPRINT("[%d]: DIV\n", line_number);

            Element *first_arg = process_token(tokens[1], stack, &return_register, registers, &temp_value);
            Element *second_arg = process_token(tokens[2], stack, &return_register, registers, &temp_value);

            div_(first_arg, second_arg);

            break;
        }

        case OP_MOD: {
            DPRINT("[%d]: MOD\n", line_number);

            Element *first_arg = process_token(tokens[1], stack, &return_register, registers, &temp_value);
            Element *second_arg = process_token(tokens[2], stack, &return_register, registers, &temp_value);

            mod(first_arg, second_arg);

            break;
        }

        case OP_JMP: {
            DPRINT("[%d]: JPM\n", line_number);
            break;
        }

        case OP_IF: {
            DPRINT("[%d]: IF\n", line_number);
            break;
        }

        case OP_RET: {
            DPRINT("[%d]: RET\n", line_number);
            break;
        }

        case OP_END: {
            DPRINT("[%d]: END\n", line_number);
            //printf("[Debugger]: %d. END\n", line_number);
            goto end;
        }

        case OP_LABEL: {
            DPRINT("[%d]: LABEL\n", line_number);
            //printf("[Debugger]: %d. LABEL!\n", line_number);
            break;
        }

        default:
            fprintf(stderr, "[Syntax error][%d]: unknown instruction: \n%s\n", line_number, line);
            break;
    }
}


int main(int argc, char *argv[]) {
    /* Arguments:
     * argv[0] - ask
     * argv[1] - debugger on
     * argv[?] - stack size
     * argv[?] - amount of registers
     * argv[argc-1] (last) - file name
     *
     * example:
     * ask -d s512 r10 file.ask
     */

    // Analise all arguments. Throw errors.
    debug |= !strcmp(argv[1], "-d");
    DPRINT("===== DEBUGGER ON! =====\n");

    if (argc < 2)
    {
        fprintf(stderr, "[Read error]: Arguments needed!\n");
        exit(1);
    }

    check_file_path(argv[argc-1]);      // is real file or no?
    check_file_extension(argv[argc-1]); // is .ask file or no?

    // Initialization of Stack & Registers
    init(stack, STACK_SIZE);
    init(registers, REGISTER_NUMBER);

    DPRINT("Stack size: %d\n", STACK_SIZE);
    DPRINT("Amount of registers: %d\n", REGISTER_NUMBER);
    DPRINT("========================\n");

    // Reading .ask file
    FILE *file = fopen(argv[argc-1], "r");

    char line[256];      // one line of file.ask
    // char *tokens[8];     // array for tokens
    // char *tk;            // token
    // int tk_count;        // tokens count
    line_number = 0;     // line numbers for error throws
    // short flag_main = 0; // is "main" started?


    // ====== First pass: collect instructions and labels ======
    rewind(file);
    program = NULL;
    program_size = 0;
    line_number = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
            line_number++;
            strip_comment(line);
            line[strcspn(line, "\n")] = '\0';   // no "\n" in my lines!
            if (strlen(line) == 0) continue;    // skip empty lines

            // Tokenization:
            char *tokens[8];
            int tk_count = 0;
            char *tk = strtok(line, " ");
            if (tk == NULL) continue;

            tokens[tk_count++] = tk;
            while ((tk = strtok(NULL, " ")) != NULL && tk_count < 8) {
                tokens[tk_count++] = tk;
            }

            // Tokens print test (you can keep it if you want)
            // for (int i = 0; i < tk_count; i++) {
            //     printf("token %d: %s\n", i, tokens[i]);
            // }

            OP_CODE operation = get_op_code(tokens[0]);
            if (operation == OP_UNKNOW) {
                fprintf(stderr, "[Syntax Error][%d]: Unknown instruction '%s'.\n", line_number, tokens[0]);
                fclose(file);
                exit(1);
            }

            // Store instruction in program array
            program = realloc(program, (program_size + 1) * sizeof(Instruction));
            Instruction *instr = &program[program_size];
            instr->op = operation;
            instr->token_count = tk_count;
            instr->line_number = line_number;
            for (int i = 0; i < tk_count; i++) {
                instr->tokens[i] = strdup(tokens[i]);
            }

            // If it's a label, add it to the label table
            if (operation == OP_LABEL) {
                // Token looks like "label_name:" -> remove the ':'
                char *label_name = strdup(tokens[0]);
                label_name[strlen(label_name) - 1] = '\0';
                add_label(label_name, program_size);
                free(label_name);
            }

            program_size++;
        }
        fclose(file);

    // Check that main label exists
    int main_idx = find_label("main");
    if (main_idx == -1) {
        fprintf(stderr, "[Fatal Error]: Label main not found.\n");
        exit(1);
    }


    // ====== Second pass: execution ======
    current_instr = main_idx;
    while (current_instr < program_size) {
        Instruction *instr = &program[current_instr];
        if (instr->op != OP_LABEL) {         // labels do nothing at runtime
            execute_instruction(instr);
        }
        current_instr++;
    }


    // Free allocated memory
    for (int i = 0; i < program_size; i++) {
        for (int j = 0; j < program[i].token_count; j++) {
            free(program[i].tokens[j]);
        }
    }
    free(program);
    for (int i = 0; i < label_count; i++) {
        free(label_table[i].name);
    }
    free(label_table);


    // debug in end
    if (debug)
    {
        DPRINT("========================\n");
        DPRINT("\nStack: \n");
        for (int i = 0; i <= stack_pointer; i++) {
            DPRINT("[%d] ", i);
            switch (stack[i].type) {
                case TYPE_INT:   DPRINT("%d (INT)\n", stack[i].value.i); break;
                case TYPE_FLOAT: DPRINT("%f (FLOAT)\n", stack[i].value.f); break;
                case TYPE_CHAR:  DPRINT("'%c' (CHAR)\n", stack[i].value.c); break;
                default:         DPRINT("void\n");
            }
        }
        printf("\nRegisters:\n");
        for (int i = 0; i < REGISTER_NUMBER; i++) {
            printf("[r%d] ", i);
            switch (registers[i].type) {
                case TYPE_INT:   printf("%d (INT)\n", registers[i].value.i); break;
                case TYPE_FLOAT: printf("%f (FLOAT)\n", registers[i].value.f); break;
                case TYPE_CHAR:  printf("'%c' (CHAR)\n", registers[i].value.c); break;
                default:         printf("void\n");
            }
        }
        printf("[Stack Pointer -> %d]\n", stack_pointer);
    }
    return 0;
}
