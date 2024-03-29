#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disassembler.h"
#include "emulator.h"

int parity(int x, int size)
{
	int i;
	int p = 0;
	x = (x & ((1<<size)-1));
	for (i=0; i<size; i++)
	{
		if (x & 0x1) p++;
		x = x >> 1;
	}
	return (0 == (p & 0x1));
}

void UnimplementedInstruction(State8080* state) {
    // pc will have advanced one, so undo that
    printf ("Error: Unimplemented instruction\n");
    state->pc--;
    Disassemble8080Op(state->memory, state->pc);
    printf("\n");
    exit(1);
}

// void generateInterrupt(State8080* state, int interrupt_num) {
//     // perform the stack push 
//     state->memory[state->sp - 1] = (state->pc >> 8) & 0xff;
//     state->memory[state->sp - 2] = (state->pc & 0xff);
//     state->sp = state->sp - 2;
//     state->pc = 8 * interrupt_num;
//     state->int_enable = 0;
// }

void executeInstruction(State8080* State8080, unsigned char* opcode, int CPU_TEST) {
    switch(*opcode) {
        case 0x00: break; // NOP 
        case 0x01: // LXI B,word
            State8080->b = opcode[2];
            State8080->c = opcode[1];
            State8080->pc += 2;
            break;
        case 0x02: UnimplementedInstruction(State8080); break;
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
                State8080->cc.p = parity(answer, 8);
                State8080->b = answer & 0xff;
            }
            break;
        case 0x05: // DCR B
            {
                uint16_t answer = (uint16_t) State8080->b - (uint16_t) 1;
                State8080->cc.z = (answer == 0);
                State8080->cc.s = ((answer & 0x80) == 0x80);
                State8080->cc.p = parity(answer, 8);
                State8080->b = answer;
            }
            break;
        case 0x06: // MVI B,byte
            State8080->b = opcode[1];
            State8080->pc++;
            break;
        case 0x07: UnimplementedInstruction(State8080); break;
        case 0x08: break; //NOP
        case 0x09: // DAD B
            {
                uint32_t hl = (uint32_t) State8080->h << 8 | State8080->l;
                uint32_t bc = (uint32_t) State8080->b << 8 | State8080->c;
                uint32_t answer = hl + bc;
                State8080->h = (answer & 0xff00) >> 8;
                State8080->l = (answer & 0xff);
                State8080->cc.cy = ((answer & 0xffff0000) > 0);
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
                State8080->cc.p = parity(answer, 8);
                State8080->c = answer & 0xff;
            }
            break;
        case 0x0d: // DCR C
            {
                uint16_t answer = (uint16_t) State8080->c - (uint16_t) 1;
                State8080->cc.z = (answer == 0);
                State8080->cc.s = ((answer & 0x80) == 0x80);
                State8080->cc.p = parity(answer, 8);
                State8080->c = answer;
            }
            break;
        case 0x0e: // MVI C,byte
            State8080->c = opcode[1];
            State8080->pc++;
            break;
        case 0x0f: // RRC
            {
                uint8_t x = State8080->a;
                State8080->a = ((x & 1) << 7) | (x >> 1);
                State8080->cc.cy = (1 == (x&1));
            }
            break;
        case 0x10: break; //NOP
        case 0x11: // LXI D,word
            State8080->d = opcode[2];
            State8080->e = opcode[1];
            State8080->pc += 2;
            break;
        case 0x12: UnimplementedInstruction(State8080); break;
        case 0x13: // INX D
            {
                State8080->e++;
                if (State8080->e == 0) {
                    State8080->d++;
                }
            }
            break;
        case 0x14: // INR D
            {
                uint16_t answer = (uint16_t) State8080->d + (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.p = parity(answer, 8);
                State8080->d = answer & 0xff;
            }
            break;
        case 0x15: // DCR D
            {
                uint16_t answer = (uint16_t) State8080->d - (uint16_t) 1;
                State8080->cc.z = (answer == 0);
                State8080->cc.s = ((answer & 0x80) == 0x80);
                State8080->cc.p = parity(answer, 8);
                State8080->d = answer;
            }
            break;
        case 0x16: // MVI D,byte
            State8080->d = opcode[1];
            State8080->pc++;
            break;
        case 0x17: // RAL
            {
                uint8_t x = State8080->a;
                State8080->a = (State8080->cc.cy << 7) | (x >> 1);
                State8080->cc.cy = (1 == (x&1));
            }
            break;
        case 0x18: break; //NOP
        case 0x19: // DAD D
            {
                uint32_t hl = (uint32_t) State8080->h << 8 | State8080->l;
                uint32_t de = (uint32_t) State8080->d << 8 | State8080->e;
                uint32_t answer = hl + de;
                State8080->h = (answer & 0xff00) >> 8;
                State8080->l = (answer & 0xff);
                State8080->cc.cy = ((answer & 0xffff0000) != 0);
            }
            break;
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
                State8080->cc.p = parity(answer, 8);
                State8080->e = answer & 0xff;
            }
            break;
        case 0x1d: // DCR E
            {
                uint16_t answer = (uint16_t) State8080->e - (uint16_t) 1;
                State8080->cc.z = (answer == 0);
                State8080->cc.s = ((answer & 0x80) == 0x80);
                State8080->cc.p = parity(answer, 8);
                State8080->e = answer;
            }
            break;
        case 0x1e: // MVI E,byte
            State8080->e = opcode[1];
            State8080->pc++;
            break;
        case 0x1f: // RAR
            {
                uint8_t x = State8080->a;
                State8080->a = (State8080->cc.cy << 7);
                State8080->cc.cy = (1 == (x&1));
            }
            break; 
        case 0x20: break; // NOP
        case 0x21: // LXI H,word
            State8080->h = opcode[2];
            State8080->l = opcode[1];
            State8080->pc += 2;
            break;
        case 0x22: UnimplementedInstruction(State8080); break;
        case 0x23: // INX H NOTE: could be issue, come back if still broke 
            {
                State8080->l++;
                if (State8080->l == 0) {
                    State8080->h++;
                }
            }
            break;
        case 0x24: // INR H
            {
                uint16_t answer = (uint16_t) State8080->h + (uint16_t) 1;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.p = parity(answer, 8);
                State8080->h = answer & 0xff;
            }
            break;
        case 0x25: // DCR H
            {
                uint16_t answer = (uint16_t) State8080->h - (uint16_t) 1;
                State8080->cc.z = (answer == 0); 
                State8080->cc.s = ((answer & 0x80) == 0x80); 
                State8080->cc.p = parity(answer, 8);
                State8080->h = answer;
            }
            break;
        case 0x26: // MVI H,byte
            State8080->h = opcode[1];
            State8080->pc++;
            break;
        case 0x27: UnimplementedInstruction(State8080); break;
        case 0x28: break; // NOP 
        case 0x29: // DAD H
            {
                uint32_t hl = (uint32_t) State8080->h << 8 | State8080->l;
                uint32_t answer = hl + hl;
                State8080->h = (answer & 0xff00) >> 8;
                State8080->l = (answer & 0xff);
                State8080->cc.cy = ((answer & 0xffff0000) != 0);
            }
            break;
        case 0x2a: UnimplementedInstruction(State8080); break;
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
                State8080->cc.p = parity(answer, 8);
                State8080->l = answer & 0xff;
            }
            break;
        case 0x2d: // DCR L
            {
                uint16_t answer = (uint16_t) State8080->l - (uint16_t) 1;
                State8080->cc.z = (answer == 0); 
                State8080->cc.s = ((answer & 0x80) == 0x80); 
                State8080->cc.p = parity(answer, 8);
                State8080->l = answer;
            }
            break;
        case 0x2e: // MVI L,byte
            State8080->l = opcode[1];
            State8080->pc++;
            break;
        case 0x2f: // CMA
            {
                State8080->a = ~State8080->a;
            }
            break;
        case 0x30: break; //NOP
        case 0x31: // LXI SP,word
            State8080->sp = (opcode[2] << 8) | opcode[1];
            State8080->pc += 2;
            break;
        case 0x32: // STA adr
            {
                uint16_t offset = (opcode[2] << 8) | (opcode[1]);
                State8080->memory[offset] = State8080->a;
                State8080->pc += 2;
            }
            break;
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
                State8080->cc.p = parity(answer, 8);
                State8080->memory[offset] = answer & 0xff;
            }
            break;
        case 0x35: // DCR M
            {
                uint16_t offset = (uint16_t) (State8080->h << 8) | (uint16_t) (State8080->l);
                uint16_t answer = (uint16_t) State8080->memory[offset] - (uint16_t) 1;
                State8080->cc.z = (answer == 0); 
                State8080->cc.s = ((answer & 0x80) == 0x80); 
                State8080->cc.p = parity(answer, 8);
                State8080->memory[offset] = answer;
            }
            break;
        case 0x36: // MVI M,byte
            {
                uint16_t offset = (uint16_t) (State8080->h << 8) | (State8080->l);
                State8080->memory[offset] = opcode[1];
                State8080->pc++;
            }
            break;
        case 0x37: // STC
            {
                State8080->cc.cy = 1;
            }
            break;
        case 0x38: break; // NOP
        case 0x39: // DAD SP
            {
                uint32_t hl = (uint32_t) State8080->h << 8 | (uint32_t) State8080->l;
                uint32_t answer = hl + State8080->sp;
                State8080->h = (answer & 0xff00) >> 8;
                State8080->l = (answer & 0xff);
                State8080->cc.cy = (answer > 0xffff);
            }
            break;
        
        case 0x3a: // LDA adr
            {
                uint16_t offset = (opcode[2] << 8) | (opcode[1]);
                State8080->a = State8080->memory[offset];
                State8080->pc += 2;
            }
            break;
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
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x3d: // DCR A
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) 1;
                State8080->cc.z = (answer == 0); 
                State8080->cc.s = ((answer & 0x80) == 0x80); 
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer;
            }
            break;
        case 0x3e: // MVI A,byte
            State8080->a = opcode[1];
            State8080->pc++;
            break;
        case 0x3f: // CMC
            {
                State8080->cc.cy = !State8080->cc.cy;
            }
            break;
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
                uint16_t offset = (State8080->h << 8) | (State8080->l);
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
                uint16_t offset = (State8080->h << 8) | (State8080->l);
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
                uint16_t offset = (State8080->h << 8) | (State8080->l);
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
                uint16_t offset = (State8080->h << 8) | (State8080->l);
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
                uint16_t offset = (State8080->h << 8) | (State8080->l);
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
                uint16_t offset = (State8080->h << 8) | (State8080->l);
                State8080->l = State8080->memory[offset];
            }
            break;
        case 0x6f: // MOV L,A
            State8080->l = State8080->a;
            break;
        case 0x70: // MOV M,B
            {
                uint16_t offset = (State8080->h << 8) | (State8080->l);
                State8080->memory[offset] = State8080->b;
            }
            break;
        case 0x71: // MOV M,C
            {
                uint16_t offset = (State8080->h << 8) | (State8080->l);
                State8080->memory[offset] = State8080->c;
            }
            break;
        case 0x72: // MOV M,D
            {
                uint16_t offset = (State8080->h << 8) | (State8080->l);
                State8080->memory[offset] = State8080->d;
            }
            break; 
        case 0x73: // MOV M,E
            {
                uint16_t offset = (State8080->h << 8) | (State8080->l);
                State8080->memory[offset] = State8080->e;
            }
            break; 
        case 0x74: // MOV M,H
            {
                uint16_t offset = (State8080->h << 8) | (State8080->l);
                State8080->memory[offset] = State8080->h;
            }
            break;
        case 0x75: // MOV M,L
            {
                uint16_t offset = (State8080->h << 8) | (uint16_t)(State8080->l);
                State8080->memory[offset] = State8080->l;
            }
            break;
        case 0x76: // HLT
            {
                exit(0);
            }
            break;
        case 0x77: // MOV M,A
            {
                uint16_t offset = (State8080->h << 8) | (State8080->l);
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
                uint16_t offset = (State8080->h << 8) | (State8080->l);
                State8080->a = State8080->memory[offset];
            }
            break;
        case 0x7f: UnimplementedInstruction(State8080); break;
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
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x81: // ADD C
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->c;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x82: // ADD D
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->d;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x83: // ADD E
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->e;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x84: // ADD H
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->h;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x85: // ADD L
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->l;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer, 8);
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
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x87: // ADD A
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->a;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer, 8);
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
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x89: // ADC C
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->c + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x8a: // ADC D
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->d + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x8b: // ADC E
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->e + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x8c: // ADC H
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->h + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x8d: // ADC L
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->l + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer, 8);
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
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x8f: // ADC A
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) State8080->a + (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer, 8);
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
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x91: // SUB C
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->c;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x92: // SUB D
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->d;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x93: // SUB E
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->e;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x94: // SUB H
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->h;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x95: // SUB L
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->l;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer, 8);
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
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x97: // SUB A
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->a;
                State8080->cc.z = ((answer & 0xff) == 0);  
                State8080->cc.s = ((answer & 0x80) != 0);  
                State8080->cc.cy = (answer > 0xff);  
                State8080->cc.p = parity(answer, 8);
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
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x99: // SBB C
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->c - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x9a: // SBB D
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->d - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x9b: // SBB E
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->e - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x9c: // SBB H
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->h - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x9d: // SBB L
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->l - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer, 8);
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
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0x9f: // SBB A
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->a - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) != 0);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity(answer, 8);
                State8080->a = answer & 0xff;
            }
            break;
        case 0xa0: // ANA B
            {
                State8080->a = State8080->a & State8080->b;
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0);
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
            }
            break;
        case 0xa1: // ANA C
            {
                State8080->a = State8080->a & State8080->c;
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0);
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
            }
            break;
        case 0xa2: // ANA D
            {
                State8080->a = State8080->a & State8080->d;
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0);
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
            }
            break;
        case 0xa3: // ANA E
            {
                State8080->a = State8080->a & State8080->e;
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0);
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
            }
            break;
        case 0xa4: // ANA H
            {
                State8080->a = State8080->a & State8080->h;
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0);
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
            }
            break;
        case 0xa5: // ANA L
            {
                State8080->a = State8080->a & State8080->l;
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0);
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
            }
            break;
        case 0xa6: // ANA M
            {
                uint16_t offset = (uint16_t) (State8080->h << 8) | (uint16_t) (State8080->l);
                State8080->a = State8080->a & State8080->memory[offset];
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0);
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
            }
            break;
        case 0xa7: // ANA A
            {
                State8080->a = State8080->a & State8080->a;
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) == 0x80);
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy =  State8080->cc.ac = 0;
            }
            break;
        case 0xa8: // XRA B
            {
                State8080->a = State8080->a ^ State8080->b;
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0);
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
            }
            break;
        case 0xa9: // XRA C
            {
                State8080->a = State8080->a ^ State8080->c;
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0);
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
            }
            break;
        case 0xaa: // XRA D
            {
                State8080->a = State8080->a ^ State8080->d;
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0); 
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
            }
            break;
        case 0xab: // XRA E
            {
                State8080->a = State8080->a ^ State8080->e;
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0); 
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
            }
            break;
        case 0xac: // XRA H
            {
                State8080->a = State8080->a ^ State8080->h;
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0); 
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
            }
            break;
        case 0xad: // XRA L
            {
                State8080->a = State8080->a ^ State8080->l;
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0); 
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
            }
            break;
        case 0xae: // XRA M
            {
                uint16_t offset = (uint16_t) (State8080->h << 8) | (uint16_t) (State8080->l);
                State8080->a = State8080->a ^ State8080->memory[offset];
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0); 
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
            }
            break;
        case 0xaf: // XRA A
            {
                State8080->a = State8080->a ^ State8080->a;
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0); 
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = State8080->cc.ac = 0;
            }
            break;
        case 0xb0: // ORA B
            {
                State8080->a = State8080->a | State8080->b;
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0); 
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
            }
            break;
        case 0xb1: // ORA C
            {
                State8080->a = State8080->a | State8080->c;
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0); 
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
            }
            break;
        case 0xb2: // ORA D
            {
                State8080->a = State8080->a | State8080->d;
                State8080->cc.z = (State8080->a == 0); 
                State8080->cc.s = ((State8080->a & 0x80) != 0); 
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0; 
            }
            break;
        case 0xb3: // ORA E
            {
                State8080->a = State8080->a | State8080->e;
                State8080->cc.z = (State8080->a == 0); 
                State8080->cc.s = ((State8080->a & 0x80) != 0); 
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0; 
            }
            break;
        case 0xb4: // ORA H
            {
                State8080->a = State8080->a | State8080->h;
                State8080->cc.z = (State8080->a == 0); 
                State8080->cc.s = ((State8080->a & 0x80) != 0); 
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0; 
            }
            break;
        case 0xb5: // ORA L
            {
                State8080->a = State8080->a | State8080->l;
                State8080->cc.z = (State8080->a == 0); 
                State8080->cc.s = ((State8080->a & 0x80) != 0); 
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0; 
            }
            break;
        case 0xb6: // ORA M
            {
                uint16_t offset = (uint16_t) (State8080->h << 8) | (uint16_t) (State8080->l);
                State8080->a = State8080->a | State8080->memory[offset];
                State8080->cc.z = (State8080->a == 0); 
                State8080->cc.s = ((State8080->a & 0x80) != 0); 
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0; 
            }
            break;
        case 0xb7: // ORA A
            {
                State8080->a = State8080->a | State8080->a;
                State8080->cc.z = (State8080->a == 0); 
                State8080->cc.s = ((State8080->a & 0x80) != 0); 
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0; 
            }
            break;
        case 0xb8: // CMP B
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->b;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer, 8);
            }
            break;
        case 0xb9: // CMP C
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->c;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer, 8);
            }
            break;
        case 0xba: // CMP D
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->d;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer, 8);
            }
            break;
        case 0xbb: // CMP E
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->e;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer, 8);
            }
            break;
        case 0xbc: // CMP H
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->h;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer, 8);
            }
            break;
        case 0xbd: // CMP L
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->l;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer, 8);
            }
            break;
        case 0xbe: // CMP M
            {
                uint16_t offset = (uint16_t) (State8080->h << 8) | (uint16_t) (State8080->l);
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->memory[offset];
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer, 8);
            }
            break;
        case 0xbf: // CMP A
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) State8080->a;
                State8080->cc.z = ((answer & 0xff) == 0); 
                State8080->cc.s = ((answer & 0x80) != 0); 
                State8080->cc.cy = (answer > 0xff); 
                State8080->cc.p = parity(answer, 8);
            }
            break;
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
        case 0xc1: // POP B
            {
                State8080->c = State8080->memory[State8080->sp];
                State8080->b = State8080->memory[State8080->sp+1];
                State8080->sp += 2;
            }
            break;
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
        case 0xc5: // PUSH B
            {
                State8080->memory[State8080->sp-1] = State8080->b;
                State8080->memory[State8080->sp-2] = State8080->c;
                State8080->sp = State8080->sp - 2;
            }
            break;
        case 0xc6: // ADI byte 
            {
                uint16_t answer = (uint16_t) State8080->a + (uint16_t) opcode[1];
                State8080->cc.z = ((answer & 0xff) == 0);   
                State8080->cc.s = ((answer & 0x80) == 0x80);   
                State8080->cc.cy = (answer > 0xff);   
                State8080->cc.p = parity((answer&0xff), 8);
                State8080->a = (uint8_t) answer;
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
        case 0xcb: break; // NOP 
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
        if(CPU_TEST == 1){   // Code to test the CPU when running the test file 
            if (5 ==  ((opcode[2] << 8) | opcode[1]))    
            {    
                if (State8080->c == 9)    
                {    
                    uint16_t offset = (State8080->d<<8) | (State8080->e);    
                    char *str = &State8080->memory[offset+3];  //skip the prefix bytes    
                    while (*str != '$')    
                        printf("%c", *str++);    
                    printf("\n");    
                }    
                else if (State8080->c == 2)    
                {    
                    //saw this in the inspected code, never saw it called    
                    printf ("print char routine called\n");    
                }    
            }    
            else if (0 ==  ((opcode[2] << 8) | opcode[1]))    
            {    
                exit(0);    
            }       
        }else {
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
                State8080->cc.p = parity(answer, 8);
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
        case 0xd1: // POP D
            {
                State8080->e = State8080->memory[State8080->sp];
                State8080->d = State8080->memory[State8080->sp+1];
                State8080->sp += 2;
            }
            break;
        case 0xd2: // JNC adr 
            {
                if (State8080->cc.cy == 0){
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        case 0xd3: // OUT byte 
            {
                uint8_t port = opcode[1];
                machine_out(State8080, port, State8080->a);
                State8080->pc++;
            }
            break;
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
        case 0xd5: // PUSH D
            {
                State8080->memory[State8080->sp-1] = State8080->d;
                State8080->memory[State8080->sp-2] = State8080->e;
                State8080->sp = State8080->sp - 2;
            }
            break;
        case 0xd6: // SUI byte
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) opcode[1];
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer, 8);
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
        case 0xd9: break; // NOP
        case 0xda: // JC ADR
            {
                if(State8080->cc.cy == 1){
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        case 0xdb: // IN byte TODO come back later to implement fully 
            {
                uint8_t port = opcode[1];
                State8080->a = machine_in(State8080, port);
                State8080->pc++;
            }
            break;
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
        case 0xdd: break; // NOP
        case 0xde: // SBI byte
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) opcode[1] - (uint16_t) State8080->cc.cy;
                State8080->cc.z = ((answer & 0xff) == 0);
                State8080->cc.s = ((answer & 0x80) != 0);
                State8080->cc.cy = (answer > 0xff);
                State8080->cc.p = parity(answer, 8);
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
        case 0xe1: // POP H
            {
                State8080->l = State8080->memory[State8080->sp];
                State8080->h = State8080->memory[State8080->sp+1];
                State8080->sp += 2;
            }
            break;
        case 0xe2: // JPO ADR 
            {
                if(State8080->cc.p == 0){
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        case 0xe3: // XTHL 
            {
                uint8_t temp;
                temp = State8080->l;
                State8080->l = State8080->memory[State8080->sp];
                State8080->memory[State8080->sp] = temp;
                temp = State8080->h;
                State8080->h = State8080->memory[State8080->sp+1];
                State8080->memory[State8080->sp+1] = temp;
            }
            break;
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
        case 0xe5: // PUSH H
            {
                State8080->memory[State8080->sp-1] = State8080->h;
                State8080->memory[State8080->sp-2] = State8080->l;
                State8080->sp = State8080->sp - 2;
            }
            break;
        case 0xe6: // ANI byte; note might have to cast opcode to uint16, if theres an error 
            {
                State8080->a = State8080->a & opcode[1];
                State8080->cc.cy = State8080->cc.ac = 0;
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) == 0x80);
                State8080->cc.p = parity(State8080->a, 8);
                State8080->pc++;
            }
            break;
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
        case 0xeb: // XCHG 
            {
                uint8_t temp1 = State8080->d;
                uint8_t temp2 = State8080->e;
                State8080->d = State8080->h;
                State8080->e = State8080->l;
                State8080->h = temp1;
                State8080->l = temp2;
            }
            break;
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
        case 0xed: break; // NOP 
        case 0xee: // XRI D8
            {
                State8080->a = State8080->a ^ opcode[1];
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0);
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
                State8080->pc++;
            }
            break;
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
        case 0xf1: // POP PSW 
            {
                State8080->a = State8080->memory[State8080->sp+1];
                uint8_t psw = State8080->memory[State8080->sp];
                State8080->cc.z = (0x01 == (psw & 0x01));
                State8080->cc.s = (0x02 == (psw & 0x02));
                State8080->cc.p = (0x04 == (psw & 0x04));
                State8080->cc.cy = (0x05 == (psw & 0x08));
                State8080->cc.ac = (0x10 == (psw & 0x10));
                State8080->sp += 2;
            }
            break;
        case 0xf2: // JP ADR 
            {
                if(State8080->cc.s == 0){
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        case 0xf3: // DI 
            {
                State8080->int_enable = 0;
                State8080->pc++;
            }
            break;
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
        case 0xf5: // PUSH PSW 
            {
                State8080->memory[State8080->sp-1] = State8080->a;
                uint8_t psw = (State8080->cc.z |
                               State8080->cc.s << 1 |
                               State8080->cc.p << 2 |
                               State8080->cc.cy << 3 |
                               State8080->cc.ac << 4 );
                State8080->memory[State8080->sp-2] = psw;
                State8080->sp = State8080->sp - 2;
            }
            break;
        case 0xf6: // ORI D8
            {
                State8080->a = State8080->a | opcode[1];
                State8080->cc.z = (State8080->a == 0);
                State8080->cc.s = ((State8080->a & 0x80) != 0);
                State8080->cc.p = parity(State8080->a, 8);
                State8080->cc.cy = 0;
                State8080->pc++;
            }
            break;
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
        case 0xf9: // SPHL 
            {
                State8080->sp = (State8080->h << 8) | State8080->l;
            }
            break;
        case 0xfa: // JM ADR 
            {
                if(State8080->cc.s == 1){
                    State8080->pc = (opcode[2] << 8) | opcode[1];
                }else{
                    State8080->pc += 2;
                }
            }
            break;
        case 0xfb: // EI 
            {
                State8080->int_enable = 1;
            }
            break;
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
        case 0xfd: break; // NOP
        case 0xfe: // CPI D8
            {
                uint16_t answer = (uint16_t) State8080->a - (uint16_t) opcode[1];
                State8080->cc.z = (answer == 0);
                State8080->cc.s = ((answer & 0x80) == 0x80);
                State8080->cc.cy = (State8080->a < opcode[1]);
                State8080->cc.p = parity(answer, 8);
                State8080->pc++;
            }
            break;
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
    

int Emulate8080Op(State8080* state, int CPU_TEST) {
    // disassemble the next instruction 
    unsigned char *opcode = &state->memory[state->pc];
    Disassemble8080Op(state->memory, state->pc);
    state->pc++;
    // execute instruction 
    executeInstruction(state, opcode, CPU_TEST);
    printf("\t");
	printf("%c", state->cc.z ? 'z' : '.');
	printf("%c", state->cc.s ? 's' : '.');
	printf("%c", state->cc.p ? 'p' : '.');
	printf("%c", state->cc.cy ? 'c' : '.');
	printf("%c  ", state->cc.ac ? 'a' : '.');
	printf("A $%02x B $%02x C $%02x D $%02x E $%02x H $%02x L $%02x SP %04x\n", state->a, state->b, state->c,
				state->d, state->e, state->h, state->l, state->sp);
    return 0;
}
