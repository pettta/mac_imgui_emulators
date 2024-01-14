#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disassembler.h"

void UnimplementedInstruction(State8080* state) {
    // pc will have advanced one, so undo that
    printf ("Error: Unimplemented instruction\n");
    exit(1);
}

u_int8_t parity(int x) {
    int parity = 0;
    while (x) {
        parity = !parity;
        x = x & (x-1);
    }
    return parity;
}

void executeInstruction(State8080* State8080){
    unsigned char *opcode = &State8080->memory[State8080->pc];
    switch(*opcode) {
        case 0x00: break; // NOP 
        case 0x01: // LXI B,word
            State8080->b = opcode[2];
            State8080->c = opcode[1];
            State8080->pc += 2;
            break;
        /*..TODO all the hard ones ..*/
        case 0x41:  // MOV B,C
            State8080->b = State8080->c;
            break;
        case 0x42: // MOV B,D
            State8080->b = State8080->d;
            break;
        case 0x43: // MOV B,E
            State8080->b = State8080->e;
            break;
        /*..TODO all the hard ones ..*/
        // Begin Arithmetic Instructions
        // Addition 
        case 0x80: // ADD B
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->b;
                // zero flag: set flag to zero else clear it 
                State8080->cc.z = ((answer & 0xff) == 0);
                // sign flag: set if bit 7 is set
                State8080->cc.s = ((answer & 0x80) != 0);
                // Carry flag : set if answer is greater than 8 bits
                State8080->cc.cy = (answer > 0xff);
                // parity flag: set if parity is even via subroutine 
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x81: // ADD C
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->c;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x82: // ADD D
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->d;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x83: // ADD E
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->e;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x84: // ADD H
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->h;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x85: // ADD L
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->l;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x86: // ADD M
            {
                uint16_t offset = (uint16_t) (State8080->h << 8) | (uint16_t) (State8080->l);
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->memory[offset];
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x87: // ADD A
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->a;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        // Carry variants 
        case 0x88: // ADC B
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->b + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x89: // ADC C
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->c + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x8a: // ADC D
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->d + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x8b: // ADC E
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->e + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x8c: // ADC H
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->h + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x8d: // ADC L
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->l + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x8e: // ADC M
            {
                uint16_t offset = (uint16_t) (State8080->h << 8) | (uint16_t) (State8080->l);
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->memory[offset] + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x8f: // ADC A
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->a + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        // Subtraction
        case 0x90: // SUB B
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->b;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x91: // SUB C
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->c;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x92: // SUB D
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->d;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x93: // SUB E
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->e;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x94: // SUB H
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->h;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x95: // SUB L
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->l;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x96: // SUB M
            {
                uint16_t offset = (uint16_t) (State8080->h << 8) | (uint16_t) (State8080->l);
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->memory[offset];
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x97: // SUB A
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->a;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        // SBB
        case 0x98: // SBB B
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->b - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x99: // SBB C
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->c - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x9a: // SBB D
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->d - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x9b: // SBB E
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->e - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x9c: // SBB H
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->h - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x9d: // SBB L
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->l - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x9e: // SBB M
            {
                uint16_t offset = (uint16_t) (State8080->h << 8) | (uint16_t) (State8080->l);
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->memory[offset] - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        case 0x9f: // SBB A
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->a - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
        /* TODO a bunch of logical operations */
        case 0xc6: // ADI byte 
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) opcode[1];
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
                State8080->pc++;
            }
        /* TODO more */
        case 0xce: // ACI byte 
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) opcode[1] + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
                State8080->pc++;
            }
        /* TODO more*/
        case 0xd6: // SUI byte
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) opcode[1];
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
                State8080->pc++;
            }
        /* TODO more*/
        case 0xde: // SBI byte
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) opcode[1] - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
                State8080->pc++;
            }
        /* TODO more*/
            break;
    }
}
    

void Emulate8080Op(State8080* state) {
    // execute instruction 
    executeInstruction(state);
    // disassemble the next instruction 
    state->pc += Disassemble8080Op(state->memory, state->pc);
}


int main (int argc, char** argv) {
    FILE *f = fopen(argv[1], "rb");
    if (f==NULL) {
        printf("error: Couldn't open %s\n", argv[1]);
        exit(1);
    }
    // Get the file size and read it into a memory buffer
    fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    unsigned char *buffer = malloc(fsize);
    fread(buffer, fsize, 1, f);
    fclose(f);

    State8080* newState = (State8080*)malloc(sizeof(State8080));
    newState->memory = malloc(0x10000);  //16K
    memcpy(newState->memory, buffer, fsize);
    newState->pc = 0;
    

    while (newState->pc < fsize) {
        Emulate8080Op(newState);
    }

    return 0;
}