#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emulator.h" 
#include "machine.h"

uint8_t machine_in(State8080* state, uint8_t port) {
    uint8_t out; 
    uint16_t offset; 
    switch(port) {
        case 1:
            out = ((Ports*) state->ports)->read1;
            break;
        case 2:
            out = ((Ports*) state->ports)->read2;
            break;
        case 3: 
            offset = (((Ports*) state->ports)->shift1 << 8) | (((Ports*) state->ports)->shift0);
            out = (offset >> (8 - ((Ports*) state->ports)->shift_offset)) & 0xff;
            break; 
        default:
            UnimplementedInstruction(state);
            exit(1);
    }
    return out;
}

// TODO - this is a hack, move it to correct location
void machine_out(State8080* state, uint8_t port, uint8_t value) {
    switch(port) {
        case 2:
            ((Ports*) state->ports)->shift_offset = value & 0x7;
            break;
        case 4:
            ((Ports*) state->ports)->shift0 = ((Ports*) state->ports)->shift1;
            ((Ports*) state->ports)->shift1 = value;
            break;
        default:
            UnimplementedInstruction(state);
            exit(1);
    }
}

int main (int argc, char** argv) {
    unsigned char *buffer;
    int CPU_TEST, done, fsize; 
    State8080* newState; 
    FILE *f;
    
    newState = calloc(1, sizeof(State8080));
    newState->memory = malloc(0x10000);  //16K
    newState->ports = calloc(1, sizeof(Ports));
    newState->machine_in = machine_in;
    newState->machine_out = machine_out;
    done = 0;

    // Read file into memory
    f = fopen(argv[1], "rb");
    if (f==NULL) {
        printf("error: Couldn't open %s\n", argv[1]);
        exit(1);
    }
    fseek(f, 0L, SEEK_END);
    fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);
    
    // Check if running test file 
    if (strcmp(argv[1], "roms/cpudiag.bin") == 0) {
        printf("Running tests...\n");
        buffer = &newState->memory[0x100];
        fread(buffer, fsize, 1, f);
        fclose(f);
        // fix first instruction to be JMP 0x100 
        newState->memory[0] = 0xc3;
        newState->memory[1] = 0;
        newState->memory[2] = 0x01;
        // fix stack pointer from 0x6ad to 0x7ad
        newState->memory[368] = 0x7;
        // make definition so we can modify some asm 
        CPU_TEST = 1;
    }else{
        buffer = &newState->memory[0];
        fread(buffer, fsize, 1, f);
        fclose(f);
        CPU_TEST = 0;
    }

    // TODO some keyboard interaction stuff 
    // TODO some graphics stuff
    while (done == 0) {
        done = Emulate8080Op(newState, CPU_TEST);
    }
    return 0;
}