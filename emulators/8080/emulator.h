#include <stdint.h> 

typedef struct ConditionCodes {
    uint8_t z:1;
    uint8_t s:1;
    uint8_t p:1;
    uint8_t cy:1;
    uint8_t ac:1;
    uint8_t pad:3;
} ConditionCodes;

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
    void* ports; //can be defined by user/machine specific
    uint8_t (*machine_in)(struct State8080*, uint8_t); // user function to read from port
    void (*machine_out)(struct State8080*, uint8_t, uint8_t); // same for writing to port
    uint8_t int_enable;
} State8080;

void UnimplementedInstruction(State8080* state);
int Emulate8080Op(State8080* state, int CPU_TEST);
uint8_t machine_in(State8080* state, uint8_t port);
void machine_out(State8080* state, uint8_t port, uint8_t value);