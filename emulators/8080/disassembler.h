#include <stdint.h> 
#include <stdio.h>

int Disassemble8080Op(unsigned char *codebuffer, int pc); 

typedef struct ConditionCodes {
    uint8_t z:1;
    uint8_t s:1;
    uint8_t p:1;
    uint8_t cy:1;
    uint8_t ac:1;
    uint8_t pad:3;
} ConditionCodes;


// TODO - this is a hack, fix it
// NOTE -- these are the ports of the space invaders machine, not the 8080 a cpu, move it eventually 
typedef struct Ports {
    uint8_t read1; // read 1 -- coin, start, shoot, joystick 
    uint8_t read2; // read 2 -- dip switches 
    uint8_t shift1; // read 3 -- shift register 1 
    uint8_t shift0; // read 3 -- shift register 2 
    uint8_t shift_offset; // write 2 -- shift register offset
    uint8_t sound; // write 3 -- sound 1 TODO 
    uint8_t sound2; // write 5 -- sound 2 TODO 
    uint8_t write4; // write 4 -- fill shift register TODO 
    uint8_t write6; // write 6 -- debug port TODO? 
} Ports;

ConditionCodes CC_ZSPAC = {1, 1, 1, 0, 1 };

typedef struct State8080 {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp;
    uint16_t pc;
    uint8_t *memory;
    struct ConditionCodes cc;
    struct Ports ports; 
    uint8_t int_enable;
} State8080;