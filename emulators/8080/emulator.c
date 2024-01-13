#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disassembler.h"

void UnimplementedInstruction(State8080* state) {
    // pc will have advanced one, so undo that
    printf ("Error: Unimplemented instruction\n");
    exit(1);
}

void executeInstruction(State8080* State8080){
    unsigned char *opcode = &State8080->memory[State8080->pc];
    switch(*opcode) {
        case 0x00: break;
        case 0x01: 
            State8080->b = opcode[2];
            State8080->c = opcode[1];
            State8080->pc += 2;
            break;
        /*....*/
        case 0x41: 
            State8080->b = State8080->c;
            break;
        case 0x42:
            State8080->b = State8080->d;
            break;
        case 0x43:
            State8080->b = State8080->e;
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