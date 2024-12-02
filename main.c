//
// Created by Simran Arora on 2024-03-12.
//
// This file will have all the switch statements
#include "xis.h"
#include <stdio.h>
#include <stdbool.h>
void translate(FILE *file) {
    // array of size 16 to store registers
    char *register_mapping[16] =
            {
                    "%rax", "%rbx", "%rcx", "%rdx", "%rsi",
                    "%rdi", "%r8","%r9", "%r10","%r11",
                    "%r12", "%r13", "%r14", "%r15", "%rbp", "%rsp"
            };
    //  array to store 1-byte sized registers
    char *one_byte_regs[16] =
            {
                    "%al", "%bl", "%cl", "%dl", "%sil",
                    "%dil", "%r8b","%r9b", "%r10b","%r11b",
                    "%r12b", "%r13b", "%r14b", "%r15b", "%bpl", "%spl"
            };

    unsigned char buffer[500]; // Assuming each byte is read
    unsigned char  one_operand = 0, sigbits = 0,reg1 = 0,reg2 = 0;
    unsigned short immediate = 0;//stores the different immediate values across different cases
    unsigned short label = 0;//this is our program counter
    bool flag = false;//tells us if std is on or not

    // Step 2: Read the binary data
    size_t bytesRead = fread(buffer, sizeof(unsigned char), 500, file);
    if (bytesRead == 0) {
        printf("Error reading file");
        fclose(file);
    }
    //printing out the prolouge
    printf(".globl test\n"
           "test:\n"
           "    push %%rbp\n"
           "    mov %%rsp, %%rbp\n");
    for (int i = 0; i < bytesRead; i++) {
        // Check for the condition of two consecutive 0 bytes
        if (buffer[i] == 0 && i + 1 < bytesRead && buffer[i + 1] == 0)
        {
               //printing out the label
                printf(".L00%02x:\n",label);
                //printing call debug only if std has already been called
                if(flag == true) {
                    printf("    call debug\n");
                }
                //printing out the epilogue once we encounter 2 consecutive 0 bits
                printf("    pop %%rbp\n"
                       "    ret");
                printf("\n");
                break;

        }
        else
        {
            sigbits = buffer[i] >> 6;//stores the first 2 significant bits of the byte being read
            switch (sigbits)
            {
                case 0://0 operand instructions
                    printf(".L00%02x:\n",label);
                    label += 2;
                    //incrementing the label by 2 everytime since 0 operand instructions take in 2 bytes
                    if(flag == true)
                    {
                        printf("    call debug\n");
                    }
                    switch (buffer[i])
                    {
                        case I_RET:
                            printf("    ret\n");
                            i = i + 1;
                            break;
                        case I_CLD:
                            flag = false;//change flag variable to false everytime we encounter cld
                            i = i + 1;
                            break;
                        case I_STD:
                            flag = true;//change the flag variable to true everytime we encounter std
                            i = i + 1;
                            break;
                        default:
                            printf("0 operand case is not working");
                            break;
                    }
                    break;
                case 1://1 operand instruction
                    printf(".L00%02x:\n",label);
                    label += 2;
                    //incrementing the label by 2 everytime since 1 operand instructions take in 2 bytes
                    one_operand = ((buffer[i] >> 5) & 1);
                    //one_operand stores the third most significant bit of the byte
                    reg1 = XIS_REG1(buffer[i+1]);//stores register 1,i.e. the first 4 bits of second byte
                    reg2 = XIS_REG2(buffer[i+1]);//stores register 2,i.e. the next 4 bits of second byte
                    if(flag == true)
                    {
                        printf("    call debug\n");
                    }
                    switch (one_operand)
                    {
                        //since the third most sig bit could either be 1 or 0, therefore we have 2 different cases
                        //inside this switch statement case 0 and case 1
                        case 0:
                            switch (buffer[i])
                            {
                                case I_NEG:
                                    printf("    neg %s\n", register_mapping[reg1]);
                                    i = i + 1;
                                    break;
                                case I_NOT:
                                    printf("    not %s\n",register_mapping[reg1]);
                                    i = i + 1;
                                    break;
                                case I_INC:
                                    printf("    inc %s\n",register_mapping[reg1]);
                                    i = i + 1;
                                    break;
                                case I_DEC:
                                    printf("    dec %s\n",register_mapping[reg1]);
                                    i = i + 1;
                                    break;
                                case I_PUSH:
                                    printf("    push %s\n",register_mapping[reg1]);
                                    i = i + 1;
                                    break;
                                case I_POP:
                                    printf("    pop %s\n",register_mapping[reg1]);
                                    i = i + 1;
                                    break;
                                case I_OUT:
                                    printf("    mov %s, %%rdi\n",register_mapping[reg1]);
                                    printf("    call outchar\n");
                                    i = i + 1;
                                    break;
                            }
                            break;
                        case 1:
                            switch (buffer[i])
                            {
                                case I_BR:
                                    //since we need to jump relatively depending upon whether the bit is set or not
                                    immediate = buffer[i+1];
                                    printf("    test %s, %s\n","$1",register_mapping[13]);
                                    printf("    jnz .L00%02x\n",(immediate + label - 2));
                                    i = i + 1;
                                    break;
                                case I_JR:
                                    immediate = buffer[i+1];
                                    printf("    jmp .L00%02x\n",(immediate + label - 2));
                                    i = i + 1;
                                    break;
                            }
                            break;
                        default:
                            printf("1 operand instructions are not working");
                            break;
                    }
                    break;
                case 2: //2 operand instructions
                    printf(".L00%02x:\n",label);
                    label += 2;
                    if(flag == true)
                    {
                        printf("    call debug\n");
                    }
                    reg1 = XIS_REG1(buffer[i+1]);
                    reg2 = XIS_REG2(buffer[i+1]);
                    switch (buffer[i])
                    {
                        case I_ADD:
                            printf("    add %s, %s\n",register_mapping[reg1],register_mapping[reg2]);
                            i = i + 1;
                            break;
                        case I_SUB:
                            printf("    sub %s, %s\n",register_mapping[reg1],register_mapping[reg2]);
                            i = i + 1;
                            break;
                        case I_MUL:
                            printf("    imul %s, %s\n",register_mapping[reg1],register_mapping[reg2]);
                            i = i + 1;
                            break;
                        case I_AND:
                            printf("    and %s, %s\n",register_mapping[reg1],register_mapping[reg2]);
                            i = i + 1;
                            break;
                        case I_OR:
                            printf("    or %s, %s\n",register_mapping[reg1],register_mapping[reg2]);
                            i = i + 1;
                            break;
                        case I_XOR:
                            printf("    xor %s, %s\n",register_mapping[reg1],register_mapping[reg2]);
                            i = i + 1;
                            break;
                        case I_TEST:
                            printf("    test %s , %s\n",register_mapping[reg1],register_mapping[reg2]);
                            printf("    setnz %sb\n",register_mapping[13]);
                            i = i + 1;
                            break;
                        case I_CMP:
                            printf("    cmp %s, %s\n",register_mapping[reg1],register_mapping[reg2]);
                            printf("    setg %sb\n",register_mapping[13]);
                            i = i + 1;
                            break;
                        case I_EQU:
                            printf("    cmp %s, %s\n",register_mapping[reg1],register_mapping[reg2]);
                            printf("    setz %sb\n",register_mapping[13]);
                            i = i + 1;
                            break;
                        case I_MOV:
                            printf("    mov %s, %s\n",register_mapping[reg1],register_mapping[reg2]);
                            i = i + 1;
                            break;
                        case I_LOAD:
                            printf("    mov (%s), %s\n",register_mapping[reg1],register_mapping[reg2]);
                            i = i + 1;
                            break;
                        case I_STOR:
                            printf("    mov %s, (%s)\n",register_mapping[reg1],register_mapping[reg2]);
                            i = i + 1;
                            break;
                        case I_LOADB:
                            printf("    mov (%s), %s\n",register_mapping[reg1],one_byte_regs[reg2]);
                            i = i + 1;
                            break;
                        case I_STORB:
                            printf("    mov %s, (%s)\n",one_byte_regs[reg1],register_mapping[reg2]);
                            i = i + 1;
                            break;
                        default:
                            printf("2 operand instructions are not working");
                            break;
                    }
                    break;
                case 3:
                    printf(".L00%02x:\n",label);
                    label += 4;
                    if(flag == true)
                    {
                        printf("    call debug\n");
                    }
                    //we need to check if the register operand and extract the third bit
                    one_operand = ((buffer[i] >> 5) & 1);
                    switch(one_operand)
                    {
                        //extended encoding
                        case 0:
                            switch (buffer[i]) {
                                case I_JMP:
                                    //assuming that buffer[i+1] is always 00000000
                                    immediate = buffer[i+2];
                                    immediate = (immediate << 8) | buffer[i+3];
                                    printf("    jmp .L00%02x\n",immediate);
                                    i = i + 3;
                                    break;
                                case I_CALL:
                                    immediate = buffer[i+2];
                                    immediate = (immediate << 8) | buffer[i+3];
                                    printf("    call .L00%02x\n",immediate);
                                    i = i + 3;
                                    break;
                            }
                            break;
                        case 1:
                            switch (buffer[i]) {
                                case I_LOADI:
                                    reg1 = XIS_REG1(buffer[i+1]);
                                    immediate = buffer[i+2];
                                    immediate = (immediate << 8) | buffer[i+3];
                                    //joining 2 bytes together for making immediate a 16 bit value
                                    printf("    mov $%u, %s\n",immediate,register_mapping[reg1]);
                                    i = i + 3;
                                    break;
                            }
                            break;
                        default:
                            printf("3 operand instructions are not working");
                            break;
                    }
                    break;
                default:
                    printf("Its in default why is it not working :D ");
                    break;
            }
        }
    }
}







