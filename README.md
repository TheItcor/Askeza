**Askeza** is a programming language centered around a virtual register and stack data structure system, interpreted via the VSMI (Virtual Stack Machine Interpreter).

Currently, the language is interpreted; a potential future transition to compilation is planned.

Versioning logic: number.number.number "name" Example: 1.0.0 "Socrates"  
├─ X: MAJOR (major changes) - may break compatibility  
├─ Y: MINOR (new features)  
└─ Z: PATCH (bug fixes)  


**Features:**
- Stack with 1024 elements (default) 
- 4 virtual registers (default)
- Interpreted
- Concatenative 
- Supports three data types: char, integer, float


Language Principles:
1. Stack operations are performed exclusively on the top elements of the stack. 
2. The return value will always be placed into a return register. 
3. Code must not be redundant.
4. Code must remain readable.
5. One instruction - one action.  

---

# Interpreter

The interpreter, compiled from a .c file, operates according to the following scheme:  
Read the source file line by line -> tokenization -> processing of instructions and arguments -> execution

run file:
```
ask main.ask
```

run with debugger:
```
ask -d main.ask
```

Change the stack size in elements (default: 1024 elements):
```
ask s2048 main.ask
```

Change the number of registers (default: 4 registers):
```
ask r8 main.ask
```

All:
```
ask (debugger on) (stack size) (number of registers) (file.ask)
```

## Debugger 

Displays information about the stack and registers after each operation involving them.
Example:
```
main:
    push s 10
    push s 30
    add s
    push r0 s
    print r0
    end 0
```

`ask -d main.ask`

```
===== DEBUGGER ON! =====
Stack size: 1024
Amount of registers: 4
========================
[1]: LABEL
[2]: PUSH s <- 10  |  [SP: 0 -> 1]
[3]: PUSH s <- 30  |  [SP: 1 -> 2]
[4]: ADD
[5]: PUSH r0 <- s  |  [SP: 0 -> 1]
[6]: PRINT: 30
[7]: END
========================

Stack:
[0][40, TYPE_INT]

Registers:
[r0][30, TYPE_INT]
[r1][0, TYPE_VOID]
[r2][0, TYPE_VOID]
[r3][0, TYPE_VOID]

[Stack Pointer -> 0]
```

## interpreter's errors

During the interpreter's operation, an error may be detected:

* [Fatal Error] - A critical error during code interpretation.
* [Syntax Error] - A syntax error.
* [Read Error] - An error while reading the file.ask.   

| Error                                                              | Meaning                                       |
| -------------------------------------------------------------------| ----------------------------------------------|
| `[Fatal Error][line]: Stack overflow`                                    | Stack overflow. The stack pointer has flown beyond its upper limits. |
| `[Syntax Error][line]: Unknown instruction /command/.`                   | Syntax error in command.                          |
| `[Read Error]: This file does not exist. Wrong path?.`             | The file for interpretation does not exist.   |
| `[Read Error]: This file does have right extension .ask!`          | File has the wrong extension.                 |
| `[Read Error]: Arguments needed!`                                  | No arguments were provided.                   |
| `[Fatal Error][line]: Label main not found.`                             | Program entry point (label 'main') not found. |
| `[Fatal Error][line]: Attempt to add char!`                              | Trying to sum characters.                     |
| `[Fatal Error][line]: Stack underflow`                                   | Stack underflow. The stack pointer fell below its lower limits. |
| `[Fatal Error][line]: Invalid types for add`                             | Error related to types when adding. |
| `[Syntax Error][line]: Non-existent register on line`                    | Attempt to use a non-existent register.  |
| `[Fatal Error][line]: Invalid types for sub`                             | Error related to types when subtraction. |
| `[Fatal Error][line]: Invalid types for mul`                             | Error related to types when multiplication. |
| `[Fatal Error][line]: Invalid types for div`                             | Error related to types when division. |
| `[Fatal Error][line]: Division by zero!`                                 | Attempt to divide by zero. |
| `[Syntax Error][line]: Unknow type on input`                       | Unknown type on input. |
| `[Fatal Error][line]: Failed to read integer`                      | A non-integer was entered when input. |
| `[Fatal Error][line]: Failed to read float`                        | A non-float was entered when input. |
| `[Fatal Error][line]: Failed to read char`                         | A non-char was entered when input. |


---

# Stack and Registers

The stack is the central data structure, the working memory for the virtual machine. All computations occur through operations on the top of this structure (LIFO). The default size is 1024 elements.

Askeza has a total of 4 (5) registers:
- r0, r1, r2, r3. Each of them can hold only a single value. These are not real CPU registers as in assembly, but rather their virtual implementation.
- rr (return register) - a designated register where the result of executing labels is intended to be placed.

A stack element or a register can hold the following data types:
- int (4 bytes)
- char (1 byte)
- float (4 bytes)


# Syntax

## Instructions / Commands

- r/s (Register/Stack) - indicates that the instruction argument should be a register or a stack location from which a value is taken.
- r/s/n (Register/Stack/Number) - indicates that the instruction argument can be a register, a stack location, or a literal number.
- r/s/v (Register/Stack/Number) - indicates that the instruction argument can be a register, a stack location, or a value (char/integer/float).
- & - an optional argument.

| Instruction/Command                    | Description                                                                                                                       | Example #1                           | Example #2                          |
| ------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------ | ----------------------------------- |
| push (r/s/v) (r/s/v)          | Places a value into a register / onto the top of the stack. `push dest src` - moves a value from the source (src) to the destination (dest). If the register already contains a value, it is overwritten with the new one. | push r0 25                           | push s 25                           |
| swap (r/s) (r/s)                      | Swap a value from one location to another.                                                                                       | swap r0 r1                           | swap s r0                           |
| copy (r/s) (r/s)                      | Copies a value from one location to another.                                                                                      | copy r0 r1                           | copy s r0                           |
| pop (s) (&last N elements)            | Removes a value from the top of the stack.                                                                                        | pop s                                | pop s 3                             |
| print (r/s) (&last N elements)        | Outputs to the console from a register / the top of the stack.                                                                    | print r0                             | print s 10                          |
| input (int/float/char) (r/s)                           | Inputs a value into a register / onto the top of the stack.                                                                       | input int r0                             | input float s                             |
| getline s                             | Inputs a string into the stack, split into characters.                                                                            |                                      | getline s                           |
| add (r/s/n) (r/s/n)                   | Addition.                                                                                                                          | add r0 r1                            | add s s                              |
| sub (r/s/n) (&r/s/n)                  | Subtraction.                                                                                                                       | sub r0 r1                            | sub s s                              |
| mul (r/s/n) (&r/s/n)                  | Multiplication.                                                                                                                    | mul r0 r1                            | mul s s                               |
| div (r/s/n) (&r/s/n)                  | Division.                                                                                                                          | div r0 r1                            | div s s                               |
| idiv (r/s/n) (&r/s/n)                 | Modulo (remainder of division).                                                                                                    | idiv r0 2                            | idiv s r2                           |
| jmp (label)                           | Jump to a label.                                                                                                                  | jmp loop                             |                                     |
| if (condition); (then) ; (&else)      | If-else branching.                                                                                                                | if (r0 == 1); print 'y' ; print 'n'  | if (s == 1); print 'y' ; print 'n'  |
| ret (&value)                          | Returns a value to the rr (return register).                                                                                     | ret 0                                |                                     |
| end (value)                           | Terminates the program.                                                                                                           | end 0                                | end 1                               |
| //                                    | Comment.                                                                                                                           |                                      |                                     |


### Syntactic Sugar

#### push
You can push multiple elements or even entire "strings" onto the stack at once:

```
push s 10 20 30 40
```
In this case, 10 will be at the bottom of the stack, and 40 will be at the top.

```
push s 'cat'
```
The characters 't', 'a', 'c' will be pushed onto the stack, with 'c' at the top. In other words, the word is written into the stack in reverse order.

#### print
In addition to outputting a single element:
```
push r0 'a'
push s 'b'

print r0 // --> a
print s  // --> b
```

you can output chunks from the stack:
```
push s 20 10 40

print s 3
```
We push 3 numbers onto the stack and then print the last 3 elements from the stack. The output will start from the topmost number, "40".

Outputting a "string":
```
push s 'Hello'

print s 5
```
We push 5 characters onto the stack and then print the last 5 elements from the stack. The stack will contain: 'o','l','l','e','H', and the output will be: 'H','e','l','l','o'.




## Labels

There is no conventional concept of a function as found in high-level languages. Instead, the language uses labels, returns, and jumps.
To declare a label, write `label_name:` before `main`. Example:

```
print_hello:
    // using stack       // the label will work only with the stack
    push s 'hello world' // will be pushed onto the stack as 'h', 'e', 'l'...
    print s 11           // print the last 11 elements of stack to the console
    ret                  // return


main:
    jmp print_hello
```

```
print_two:
  // using r0, rr   //  uses r0, rr in the label
	add r0 2
	push rr, r0 // move the value from r0 to rr
	ret


main:
    jmp print_two
```

## Loops
You can create loops using labels and conditional jumps:

Implementation of *for* loop:
```
main:
    push r0 0
    
for:

    // loop body
    add r0 1
    if (r0 != 10); jmp for; jmp end_for

end_for:
    // continuation
```

Implementation of a *while* loop:
```
main:
    push r0 0

loop:

    // loop body
    if (r0 = 1); jmp loop; jmp end_loop

end_loop:
    
    // continuation
```

# Importing Other Files
In Askeza, you can import code from other files. When importing, only labels (except for `main`) are imported from the file.

File lib.ask
```
square:
    // use r0, rr
    mult r0 r0
    ret rr

main:
	push r0 2
	jmp square
	print r0
	end 0
```

File main.ask
```
import "lib.ask"

main:
    push r0 10
    call square
    print rr
    end 0
```
Output:
`100`

Be aware that during the execution of functions, your registers/stack may change values, especially the rr (return register).


# Code Style Guidelines

## Function Formatting

- Function names should use snake_case.
- If a label returns a value, it must return it to the special register `rr` (return register).

## Indentation

- It is recommended to use 4 spaces after a label, including `main`:
```
main:
    push s 'hello, world!'
    print s 13

```

* There should be one blank line between labels, including `main`:
```
main:
    if (r0 != 0); jmp then;

then:
    jmp print_cool
```
