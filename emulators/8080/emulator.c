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


// TODO at some point pull arithmetic common functionalitys into functions
// that take state and answer as params 
// For example, all add instructions have some common add, then you build 
// on that for the variants with flags, this is true of other things like CALL, RET, etc 

void executeInstruction(State8080* State8080){
    unsigned char *opcode = &State8080->memory[State8080->pc];
    switch(*opcode) {
        case 0x00: break; // NOP 
        case 0x01: // LXI B,word
            State8080->b = opcode[2];
            State8080->c = opcode[1];
            State8080->pc += 2;
            break;
        // TODO 0x02 
        case 0x03: // INX B
            {
                uint16_t bc = (uint16_t) State8080->b << 8 | (uint16_t) State8080->c;
                bc++;
                State8080->b = (bc & 0xff00) >> 8;
                State8080->c = (bc & 0xff);
            }
            break;
        case 0x04: // INR B
            {
                uint16_t answer = (uint16_t) State8080->b + (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.p = parity(answer&0xff);
                State8080->b = answer & 0xff;
            }
            break;
        case 0x05: // DCR B
            {
                uint16_t answer = (uint16_t) State8080->b - (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.p = parity(answer&0xff);
                State8080->b = answer & 0xff;
            }
            break;
        // TODO 0x06 - 08 
        case 0x09: // DAD B
            {
                uint32_t hl = (uint32_t) State8080->h << 8 | (uint32_t) State8080->l;
                uint32_t bc = (uint32_t) State8080->b << 8 | (uint32_t) State8080->c;
                uint32_t answer = hl + bc;
                State8080->h = (answer & 0xff00) >> 8;
                State8080->l = (answer & 0xff);
                State8080->cc.cy = (answer > 0xffff);
            }
            break;
        case 0x0a: // LDAX B
            {
                uint16_t offset = (uint16_t) (State8080->b << 8) | (uint16_t) (State8080->c);
                State8080->a = State8080->memory[offset];
            }
            break;
        case 0x0b: // DCX B
            {
                uint16_t bc = (uint16_t) State8080->b << 8 | (uint16_t) State8080->c;
                bc--;
                State8080->b = (bc & 0xff00) >> 8;
                State8080->c = (bc & 0xff);
            }
            break;
        case 0x0c: // INR C
            {
                uint16_t answer = (uint16_t) State8080->c + (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.p = parity(answer&0xff);
                State8080->c = answer & 0xff;
            }
            break;
        case 0x0d: // DCR C
            {
                uint16_t answer = (uint16_t) State8080->c - (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.p = parity(answer&0xff);
                State8080->c = answer & 0xff;
            }
            break;
        // TODO 0x0e - 0x12
        case 0x13: // INX D
            {
                uint16_t de = (uint16_t) State8080->d << 8 | (uint16_t) State8080->e;
                de++;
                State8080->d = (de & 0xff00) >> 8;
                State8080->e = (de & 0xff);
            }
            break;
        case 0x14: // INR D
            {
                uint16_t answer = (uint16_t) State8080->d + (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.p = parity(answer&0xff);
                State8080->d = answer & 0xff;
            }
            break;
        case 0x15: // DCR D
            {
                uint16_t answer = (uint16_t) State8080->d - (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.p = parity(answer&0xff);
                State8080->d = answer & 0xff;
            }
            break;
        // TODO 0x16 - 0x19
        case 0x1a: // LDAX D
            {
                uint16_t offset = (uint16_t) (State8080->d << 8) | (uint16_t) (State8080->e);
                State8080->a = State8080->memory[offset];
            }
            break;
        case 0x1b: // DCX D
            {
                uint16_t de = (uint16_t) State8080->d << 8 | (uint16_t) State8080->e;
                de--;
                State8080->d = (de & 0xff00) >> 8;
                State8080->e = (de & 0xff);
            }
            break;
        case 0x1c: // INR E
            {
                uint16_t answer = (uint16_t) State8080->e + (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.p = parity(answer&0xff);
                State8080->e = answer & 0xff;
            }
            break;
        case 0x1d: // DCR E
            {
                uint16_t answer = (uint16_t) State8080->e - (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.p = parity(answer&0xff);
                State8080->e = answer & 0xff;
            }
            break;
        // TODO 0x1e - 0x22
        case 0x23: // INX H
            {
                uint16_t hl = (uint16_t) State8080->h << 8 | (uint16_t) State8080->l;
                hl++;
                State8080->h = (hl & 0xff00) >> 8;
                State8080->l = (hl & 0xff);
            }
            break;
        case 0x24: // INR H
            {
                uint16_t answer = (uint16_t) State8080->h + (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.p = parity(answer&0xff);
                State8080->h = answer & 0xff;
            }
            break;
        case 0x25: // DCR H
            {
                uint16_t answer = (uint16_t) State8080->h - (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.p = parity(answer&0xff);
                State8080->h = answer & 0xff;
            }
            break;
        // TODO 0x26 - 0x28
        case 0x29: // DAD H
            {
                uint32_t hl = (uint32_t) State8080->h << 8 | (uint32_t) State8080->l;
                uint32_t answer = hl + hl;
                State8080->h = (answer & 0xff00) >> 8;
                State8080->l = (answer & 0xff);
                State8080->cc.cy = (answer > 0xffff);
            }
            break;
        // TODO 0x2a
        case 0x2b: // DCX H
            {
                uint16_t hl = (uint16_t) State8080->h << 8 | (uint16_t) State8080->l;
                hl--;
                State8080->h = (hl & 0xff00) >> 8;
                State8080->l = (hl & 0xff);
            }
            break;
        case 0x2c: // INR L
            {
                uint16_t answer = (uint16_t) State8080->l + (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.p = parity(answer&0xff);
                State8080->l = answer & 0xff;
            }
            break;
        case 0x2d: // DCR L
            {
                uint16_t answer = (uint16_t) State8080->l - (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.p = parity(answer&0xff);
                State8080->l = answer & 0xff;
            }
            break;
        // TODO 0x2e - 0x32
        case 0x33: // INX SP
            {
                State8080->sp++;
            }
            break;
        case 0x34: // INR M
            {
                uint16_t offset = (uint16_t) (State8080->h << 8) | (uint16_t) (State8080->l);
                uint16_t answer = (uint16_t) State8080->memory[offset] + (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.p = parity(answer&0xff);
                State8080->memory[offset] = answer & 0xff;
            }
            break;
        case 0x35: // DCR M
            {
                uint16_t offset = (uint16_t) (State8080->h << 8) | (uint16_t) (State8080->l);
                uint16_t answer = (uint16_t) State8080->memory[offset] - (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.p = parity(answer&0xff);
                State8080->memory[offset] = answer & 0xff;
            }
            break;
        // TODO 0x36 - 0x38
        case 0x39: // DAD SP
            {
                uint32_t hl = (uint32_t) State8080->h << 8 | (uint32_t) State8080->l;
                uint32_t answer = hl + State8080->sp;
                State8080->h = (answer & 0xff00) >> 8;
                State8080->l = (answer & 0xff);
                State8080->cc.cy = (answer > 0xffff);
            }
            break;
        
        // TODO 0x3a
        case 0x3b: // DCX SP
            {
                State8080->sp--;
            }
            break;
        case 0x3c: // INR A
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x3d: // DCR A
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        // TODO 0x3e - 0x3f
        case 0x41:  // MOV B,C
            State8080->b = State8080->c;
            break;
        case 0x42: // MOV B,D
            State8080->b = State8080->d;
            break;
        case 0x43: // MOV B,E
            State8080->b = State8080->e;
            break;
        case 0x44: // MOV B,H
            State8080->b = State8080->h;
            break;
        case 0x45: // MOV B,L
            State8080->b = State8080->l;
            break;
        case 0x46: // MOV B,M
            {
                uint16_t offset = (uint16_t)(State8080->h << 8) | (uint16_t)(State8080->l);
                State8080->b = State8080->memory[offset];
            }
            break;
        case 0x47: // MOV B,A
            State8080->b = State8080->a;
            break;
        case 0x48: // MOV C,B
            State8080->c = State8080->b;
            break;
        case 0x49: // MOV C,C
            // No operation
            break;
        case 0x4a: // MOV C,D
            State8080->c = State8080->d;
            break;
        case 0x4b: // MOV C,E
            State8080->c = State8080->e;
            break;
        case 0x4c: // MOV C,H
            State8080->c = State8080->h;
            break;
        case 0x4d: // MOV C,L
            State8080->c = State8080->l;
            break;
        case 0x4e: // MOV C,M
            {
                uint16_t offset = (uint16_t)(State8080->h << 8) | (uint16_t)(State8080->l);
                State8080->c = State8080->memory[offset];
            }
            break;
        case 0x4f: // MOV C,A
            State8080->c = State8080->a;
            break;
        case 0x50: // MOV D,B
            State8080->d = State8080->b;
            break;
        case 0x51: // MOV D,C
            State8080->d = State8080->c;
            break;
        case 0x52: // MOV D,D
            // No operation
            break;
        case 0x53: // MOV D,E
            State8080->d = State8080->e;
            break;
        case 0x54: // MOV D,H
            State8080->d = State8080->h;
            break;
        case 0x55: // MOV D,L
            State8080->d = State8080->l;
            break;
        case 0x56: // MOV D,M
            {
                uint16_t offset = (uint16_t)(State8080->h << 8) | (uint16_t)(State8080->l);
                State8080->d = State8080->memory[offset];
            }
            break;
        case 0x57: // MOV D,A
            State8080->d = State8080->a;
            break;
        case 0x58: // MOV E,B
            State8080->e = State8080->b;
            break;
        case 0x59: // MOV E,C
            State8080->e = State8080->c;
            break;
        case 0x5a: // MOV E,D
            State8080->e = State8080->d;
            break;
        case 0x5b: // MOV E,E
            // No operation
            break;
        case 0x5c: // MOV E,H
            State8080->e = State8080->h;
            break;
        case 0x5d: // MOV E,L
            State8080->e = State8080->l;
            break;
        case 0x5e: // MOV E,M
            {
                uint16_t offset = (uint16_t)(State8080->h << 8) | (uint16_t)(State8080->l);
                State8080->e = State8080->memory[offset];
            }
            break;
        case 0x5f: // MOV E,A
            State8080->e = State8080->a;
            break;
        case 0x60: // MOV H,B
            State8080->h = State8080->b;
            break;
        case 0x61: // MOV H,C
            State8080->h = State8080->c;
            break;
        case 0x62: // MOV H,D
            State8080->h = State8080->d;
            break;
        case 0x63: // MOV H,E
            State8080->h = State8080->e;
            break;
        case 0x64: // MOV H,H
            // No operation
            break;
        case 0x65: // MOV H,L
            State8080->h = State8080->l;
            break;
        case 0x66: // MOV H,M
            {
                uint16_t offset = (uint16_t)(State8080->h << 8) | (uint16_t)(State8080->l);
                State8080->h = State8080->memory[offset];
            }
            break;
        case 0x67: // MOV H,A
            State8080->h = State8080->a;
            break;
        case 0x68: // MOV L,B
            State8080->l = State8080->b;
            break;
        case 0x69: // MOV L,C
            State8080->l = State8080->c;
            break;
        case 0x6a: // MOV L,D
            State8080->l = State8080->d;
            break;
        case 0x6b: // MOV L,E
            State8080->l = State8080->e;
            break;
        case 0x6c: // MOV L,H
            State8080->l = State8080->h;
            break;
        case 0x6d: // MOV L,L
            // No operation
            break;
        case 0x6e: // MOV L,M
            {
                uint16_t offset = (uint16_t)(State8080->h << 8) | (uint16_t)(State8080->l);
                State8080->l = State8080->memory[offset];
            }
            break;
        case 0x6f: // MOV L,A
            State8080->l = State8080->a;
            break;
        case 0x70: // MOV M,B
            {
                uint16_t offset = (uint16_t)(State8080->h << 8) | (uint16_t)(State8080->l);
                State8080->memory[offset] = State8080->b;
            }
            break;
        case 0x71: // MOV M,C
            {
                uint16_t offset = (uint16_t)(State8080->h << 8) | (uint16_t)(State8080->l);
                State8080->memory[offset] = State8080->c;
            }
            break;
        case 0x72: // MOV M,D
            {
                uint16_t offset = (uint16_t)(State8080->h << 8) | (uint16_t)(State8080->l);
                State8080->memory[offset] = State8080->d;
            }
            break; 
        case 0x73: // MOV M,E
            {
                uint16_t offset = (uint16_t)(State8080->h << 8) | (uint16_t)(State8080->l);
                State8080->memory[offset] = State8080->e;
            }
            break; 
        case 0x74: // MOV M,H
            {
                uint16_t offset = (uint16_t)(State8080->h << 8) | (uint16_t)(State8080->l);
                State8080->memory[offset] = State8080->h;
            }
            break;
        case 0x75: // MOV M,L
            {
                uint16_t offset = (uint16_t)(State8080->h << 8) | (uint16_t)(State8080->l);
                State8080->memory[offset] = State8080->l;
            }
            break;
        // TODO case 0x76 special case HLT
        case 0x77: // MOV M,A
            {
                uint16_t offset = (uint16_t)(State8080->h << 8) | (uint16_t)(State8080->l);
                State8080->memory[offset] = State8080->a;
            }
            break;
        case 0x78: // MOV A,B
            State8080->a = State8080->b;
            break;
        case 0x79: // MOV A,C
            State8080->a = State8080->c;
            break;
        case 0x7a: // MOV A,D
            State8080->a = State8080->d;
            break;
        case 0x7b: // MOV A,E
            State8080->a = State8080->e;
            break;
        case 0x7c: // MOV A,H
            State8080->a = State8080->h;
            break;
        case 0x7d: // MOV A,L
            State8080->a = State8080->l;
            break;
        case 0x7e: // MOV A,M
            {
                uint16_t offset = (uint16_t)(State8080->h << 8) | (uint16_t)(State8080->l);
                State8080->a = State8080->memory[offset];
            }
            break;
        case 0x7f: // MOV A,A
            // No operation
            break;
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
            break;
        case 0x81: // ADD C
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->c;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x82: // ADD D
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->d;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x83: // ADD E
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->e;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x84: // ADD H
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->h;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x85: // ADD L
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->l;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
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
            break;
        case 0x87: // ADD A
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->a;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
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
            break;
        case 0x89: // ADC C
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->c + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x8a: // ADC D
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->d + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x8b: // ADC E
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->e + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x8c: // ADC H
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->h + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x8d: // ADC L
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->l + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
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
            break;
        case 0x8f: // ADC A
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->a + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
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
            break;
        case 0x91: // SUB C
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->c;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x92: // SUB D
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->d;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x93: // SUB E
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->e;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x94: // SUB H
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->h;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x95: // SUB L
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->l;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
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
            break;
        case 0x97: // SUB A
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->a;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
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
            break;
        case 0x99: // SBB C
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->c - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x9a: // SBB D
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->d - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x9b: // SBB E
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->e - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x9c: // SBB H
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->h - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x9d: // SBB L
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->l - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
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
            break;
        case 0x9f: // SBB A
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->a - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer&0xff);
                State8080->a = answer & 0xff;
            }
            break;
        /* TODO 0xa1 through 0xbf */
        case 0xc0: // RNZ
            {
                if(State8080->cc.z == 0){
                    State8080->pc = State8080->memory[State8080->sp] | (State8080->memory[State8080->sp+1] << 8);
                    State8080->sp += 2;
                }else{
                    State8080->pc++;
                }
            }
            break;
        // TODO 0xc1 
        case 0xc2: // JNZ address 
            {
                if(State8080->cc.z == 0){
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2; 
                }
            }
            break;
        case 0xc3: // JMP address
            State8080->pc = (opcode[2] << 8 ) | opcode[1];
            break; 
        case 0xc4: // CNZ address
            {
                if(State8080->cc.z == 0){
                    uint16_t ret = State8080->pc + 2;
                    State8080->memory[State8080->sp-1] = (ret >> 8) & 0xff;
                    State8080->memory[State8080->sp-2] = (ret & 0xff);
                    State8080->sp = State8080->sp - 2;
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        // TODO 0xc5
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
            break;
        case 0xc7: // RST 0 
            {
                uint16_t ret = State8080->pc;
                State8080->memory[State8080->sp-1] = (ret >> 8) & 0xff;
                State8080->memory[State8080->sp-2] = (ret & 0xff);
                State8080->sp = State8080->sp - 2;
                State8080->pc = 0;
            }
            break;
        case 0xc8: // RZ 
            {
                if(State8080->cc.z == 1){
                    State8080->pc = State8080->memory[State8080->sp] | (State8080->memory[State8080->sp+1] << 8);
                    State8080->sp += 2;
                }else{
                    State8080->pc++;
                }
            }
            break;
        case 0xc9: // RET 
                State8080->pc = State8080->memory[State8080->sp] | (State8080->memory[State8080->sp+1] << 8);
                State8080->sp += 2;
                break; 
        case 0xca: // JZ adr 
            {
                if(State8080->cc.z == 1){
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        // TODO 0xcb-0xcb
        case 0xcc: // CZ adr 
            {
                if(State8080->cc.z == 1){
                    uint16_t ret = State8080->pc + 2;
                    State8080->memory[State8080->sp-1] = (ret >> 8) & 0xff;
                    State8080->memory[State8080->sp-2] = (ret & 0xff);
                    State8080->sp = State8080->sp - 2;
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        case 0xcd: //CALL adr 
            {
                uint16_t ret = State8080->pc + 2;
                State8080->memory[State8080->sp-1] = (ret >> 8) & 0xff;
                State8080->memory[State8080->sp-2] = (ret & 0xff);
                State8080->sp = State8080->sp - 2;
                State8080->pc = (opcode[2] << 8) | opcode[1];
            }
            break;
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
            break;
        case 0xcf: // RST 1 
            {
                uint16_t ret = State8080->pc;
                State8080->memory[State8080->sp-1] = (ret >> 8) & 0xff;
                State8080->memory[State8080->sp-2] = (ret & 0xff);
                State8080->sp = State8080->sp - 2;
                State8080->pc = 0x8;
            }
            break;
        case 0xd0: // RNC 
            {
                if(State8080->cc.cy == 0){
                    State8080->pc = State8080->memory[State8080->sp] | (State8080->memory[State8080->sp+1] << 8);
                    State8080->sp += 2;
                }else{
                    State8080->pc++;
                }
            }
            break;
        // TODO 0xd1
        case 0xd2: // JNC adr 
            {
                if (State8080->cc.cy == 0){
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        /* TODO 0xd0-d3*/
        case 0xd4: // CNC adr 
            {
                if(State8080->cc.cy == 0){
                    uint16_t ret = State8080->pc + 2;
                    State8080->memory[State8080->sp-1] = (ret >> 8) & 0xff;
                    State8080->memory[State8080->sp-2] = (ret & 0xff);
                    State8080->sp = State8080->sp - 2;
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        // TODO 0xd5
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
            break;
        case 0xd7: // RST 2 
            {
                uint16_t ret = State8080->pc;
                State8080->memory[State8080->sp-1] = (ret >> 8) & 0xff;
                State8080->memory[State8080->sp-2] = (ret & 0xff);
                State8080->sp = State8080->sp - 2;
                State8080->pc = 0x10;
            }
            break;
        case 0xd8: // RC 
            {
                if(State8080->cc.cy == 1){
                    State8080->pc = State8080->memory[State8080->sp] | (State8080->memory[State8080->sp+1] << 8);
                    State8080->sp += 2;
                }else{
                    State8080->pc++;
                }
            }
            break;
        // TODO 0xd9
        case 0xda: // JC ADR
            {
                if(State8080->cc.cy == 1){
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        /* TODO 0xdb*/
        case 0xdc: // CC ADR
            {
                if(State8080->cc.cy == 1){
                    uint16_t ret = State8080->pc + 2;
                    State8080->memory[State8080->sp-1] = (ret >> 8) & 0xff;
                    State8080->memory[State8080->sp-2] = (ret & 0xff);
                    State8080->sp = State8080->sp - 2;
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        // TODO 0xdd 
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
            break;
        case 0xdf: // RST 3 
            {
                uint16_t ret = State8080->pc;
                State8080->memory[State8080->sp-1] = (ret >> 8) & 0xff;
                State8080->memory[State8080->sp-2] = (ret & 0xff);
                State8080->sp = State8080->sp - 2;
                State8080->pc = 0x18;
            }
            break;
        case 0xe0: // RPO 
            {
                if(State8080->cc.p == 0){
                    State8080->pc = State8080->memory[State8080->sp] | (State8080->memory[State8080->sp+1] << 8);
                    State8080->sp += 2;
                }else{
                    State8080->pc++;
                }
            }
            break;
        // TODO 0xe1
        case 0xe2: // JPO ADR 
            {
                if(State8080->cc.p == 0){
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        /* TODO 0xe3 */
        case 0xe4: // CPO ADR 
            {
                if(State8080->cc.p == 0){
                    uint16_t ret = State8080->pc + 2;
                    State8080->memory[State8080->sp-1] = (ret >> 8) & 0xff;
                    State8080->memory[State8080->sp-2] = (ret & 0xff);
                    State8080->sp = State8080->sp - 2;
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        // TODO 0xe5-0xe6
        case 0xe7: // RST 4 
            {
                uint16_t ret = State8080->pc;
                State8080->memory[State8080->sp-1] = (ret >> 8) & 0xff;
                State8080->memory[State8080->sp-2] = (ret & 0xff);
                State8080->sp = State8080->sp - 2;
                State8080->pc = 0x20;
            }
            break;
        case 0xe8: // RPE 
            {
                if(State8080->cc.p == 1){
                    State8080->pc = State8080->memory[State8080->sp] | (State8080->memory[State8080->sp+1] << 8);
                    State8080->sp += 2;
                }else{
                    State8080->pc++;
                }
            }
            break;
        case 0xe9: // PCHL 
            {
                State8080->pc = (State8080->h << 8) | State8080->l;
            }
            break;
        case 0xea: // JPE ADR 
            {
                if(State8080->cc.p == 1){
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        /* TODO 0xeb*/
        case 0xec: // CPE ADR 
            {
                if(State8080->cc.p == 1){
                    uint16_t ret = State8080->pc + 2;
                    State8080->memory[State8080->sp-1] = (ret >> 8) & 0xff;
                    State8080->memory[State8080->sp-2] = (ret & 0xff);
                    State8080->sp = State8080->sp - 2;
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        // TODO 0xed - 0xee
        case 0xef: // RST 5 
            {
                uint16_t ret = State8080->pc;
                State8080->memory[State8080->sp-1] = (ret >> 8) & 0xff;
                State8080->memory[State8080->sp-2] = (ret & 0xff);
                State8080->sp = State8080->sp - 2;
                State8080->pc = 0x28;
            }
            break;
        case 0xf0: // RP 
            {
                if(State8080->cc.s == 0){
                    State8080->pc = State8080->memory[State8080->sp] | (State8080->memory[State8080->sp+1] << 8);
                    State8080->sp += 2;
                }else{
                    State8080->pc++;
                }
            }
            break;
        // TODO 0xf1 
        case 0xf2: // JP ADR 
            {
                if(State8080->cc.s == 0){
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        /* TODO 0xf3 */
        case 0xf4: // CP ADR 
            {
                if(State8080->cc.s == 0){
                    uint16_t ret = State8080->pc + 2;
                    State8080->memory[State8080->sp-1] = (ret >> 8) & 0xff;
                    State8080->memory[State8080->sp-2] = (ret & 0xff);
                    State8080->sp = State8080->sp - 2;
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        // TODO 0xf5-0xf6
        case 0xf7: // RST 6
            {
                uint16_t ret = State8080->pc;
                State8080->memory[State8080->sp-1] = (ret >> 8) & 0xff;
                State8080->memory[State8080->sp-2] = (ret & 0xff);
                State8080->sp = State8080->sp - 2;
                State8080->pc = 0x30;
            }
            break;
        case 0xf8: // RM 
            {
                if(State8080->cc.s == 1){
                    State8080->pc = State8080->memory[State8080->sp] | (State8080->memory[State8080->sp+1] << 8);
                    State8080->sp += 2;
                }else{
                    State8080->pc++;
                }
            }
            break;
        // TODO 0xf9
        case 0xfa: // JM ADR 
            {
                if(State8080->cc.s == 1){
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        /* TODO 0xfb*/
        case 0xfc: // CM ADR 
            {
                if(State8080->cc.s == 1){
                    uint16_t ret = State8080->pc + 2;
                    State8080->memory[State8080->sp-1] = (ret >> 8) & 0xff;
                    State8080->memory[State8080->sp-2] = (ret & 0xff);
                    State8080->sp = State8080->sp - 2;
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        // TODO 0xfd- 0xfe
        case 0xff: // RST 7 
            {
                uint16_t ret = State8080->pc;
                State8080->memory[State8080->sp-1] = (ret >> 8) & 0xff;
                State8080->memory[State8080->sp-2] = (ret & 0xff);
                State8080->sp = State8080->sp - 2;
                State8080->pc = 0x38;
            }
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