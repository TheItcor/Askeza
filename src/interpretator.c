/* ==== Virtual Stack Machine Interpretator (Askeza) ====
 *
 * Version: 1.0.0 "Socrates"
 * Started: 16.12.2025
 * Github (documentation and etc): https://github.com/TheItcor/Askeza
 *
 *
 * I would be glad to receive your suggestions.
 * p.s. In the future, it is planned to reprogram the interpreter into a compiler.
 * */

/* To Do
 * [x] reading file line by line, counter the number of line.
 * [x] split file by tokens (tk).
 * [x] simple interpretation.
 * [x] stack
 * [x] registers
 * [x] errors message.
 * 
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//#include "throwErrors.h"?
//#include "stackRegisters.h"?

int STACK_SIZE = 1024;
int REGISTER_NUMBER = 4;

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


void push() {}

void pop() {}

void swap() {}

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
        fprintf(stderr, "[Fatal Error]: This file does not exist. Wrong path?.\n");
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
        fprintf(stderr, "[Fatal Error]: This file does have right extension .ask!\n");
        exit(1);
    }
}

// Funcs for parse

void strip_comment(char *line) {
    char *comment = strstr(line, "//");
    if (comment) *comment = '\0';
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
        fprintf(stderr, "[Fatal error]: Arguments needed!\n");
        exit(1);
    }

    check_file_path(argv[argc-1]);      // is real file or no?
    check_file_extension(argv[argc-1]); // is .ask file or no?
    
    // Initialization of Stack & Registers
    
    int stack_pointer = -1;             // if -1, then stack is empty 
    Element stack[STACK_SIZE];
    Element registers[REGISTER_NUMBER]; // [0] = r0, [1] = r1, ...
    Element return_register;            // rr
    
    init(stack, STACK_SIZE);
    init(registers, REGISTER_NUMBER);

    printf("[Debugger]: Stack size: %d\n", STACK_SIZE);
    printf("[Debugger]: Amount of registers: %d\n", REGISTER_NUMBER);
    
    
    // Reading .ask file
    FILE *file = fopen(argv[argc-1], "r");

    char line[256];      // one line of file.ask
    char *tk;            // token
    int line_number = 0; // line numbers for error throws
    short flag_main = 0; // is "main" started?

    while (fgets(line, sizeof(line), file) != NULL)
    {
        line_number++;
        
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
        
        OP_CODE operation = get_op_code(&tk[0]);
        
        switch (operation)
        {
        case OP_PUSH:
            printf("[Debugger]: %d. PUSH\n", line_number);
            break;
            
        case OP_POP:
            printf("[Debugger]: %d. POP\n", line_number);
            break;
            
        case OP_SWAP:
            printf("[Debugger]: %d. SWAP\n", line_number);
            break;
            
        case OP_COPY:
            printf("[Debugger]: %d. COPY\n", line_number);
            break;
            
        case OP_PRINT:
            printf("[Debugger]: %d. PRINT\n", line_number);
            break;
            
        case OP_INPUT:
            printf("[Debugger]: %d. INPUT\n", line_number);
            break;
            
        case OP_GETLINES:
            printf("[Debugger]: %d. GETLINES\n", line_number);
            break;
            
        case OP_ADD:
            printf("[Debugger]: %d. ADD\n", line_number);
            break;
            
        case OP_MUL:
            printf("[Debugger]: %d. MUL\n", line_number);
            break;
            
        case OP_SUB:
            printf("[Debugger]: %d. SUB\n", line_number);
            break;
            
        case OP_DIV:
            printf("[Debugger]: %d. DIV\n", line_number);
            break;
            
        case OP_IDIV:
            printf("[Debugger]: %d. IDIV\n", line_number);
            break;
            
        case OP_JMP:
            printf("[Debugger]: %d. JMP\n", line_number);
            break;
            
        case OP_IF:
            printf("[Debugger]: %d. IF\n", line_number);
            break;
            
        case OP_RET:
            printf("[Debugger]: %d. RET\n", line_number);
            break;
            
        case OP_END:
            printf("[Debugger]: %d. END\n", line_number);
            return 0;
            
        case OP_LABEL:
            printf("[Debugger]: %d. MAIN!\n", line_number);
            break;
        
        
        default:
            fprintf(stderr, "[Syntax error]: unknown instruction at line %d\n", line_number);
            break;
        }
        
        
    }

    if (flag_main == 0) fprintf(stderr, "[Error]: label main not found.");

    return 0;
}
