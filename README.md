# My-personal-CPU

## Purpose: 
This is a binary-to-assembly translator/decompiler project. The main program reads a binary file and passes it to a translate function that converts machine code into x86-64 assembly language. The program specifically handles different types of instructions (0-operand, 1-operand, 2-operand, and 3-operand instructions) and translates them into their corresponding assembly representations.
## Implementation Details:
  * The project consists of three files: main.c (handles file I/O), xtra.h (header file), and xtra.c (contains the main translation logic)
  * The translate function uses register mappings for both 64-bit registers (like %rax, %rbx) and 8-bit registers
  * It processes instructions based on their significant bits (using 2-bit prefixes to determine instruction type)
  * The program handles various x86 instructions including arithmetic operations (ADD, SUB, MUL), logical operations (AND, OR, XOR), control flow (JMP, CALL), and data movement (MOV, LOAD, STORE)
