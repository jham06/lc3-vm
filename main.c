#include <stdio.h>
#include <stdint.h>
#define MAX_MEMORY 65536

enum {
    R0 = 0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R_PC,
    R_COND,
    R_COUNT
};

enum {
    BR_OP = 0,
    ADD_OP,
    LD_OP,
    ST_OP,
    JSR_OP,
    AND_OP,
    LDR_OP,
    STR_OP,
    RTI_OP,
    NOT_OP,
    LDI_OP,
    STI_OP,
    JMP_OP,
    RES_OP,
    LEA_OP,
    TRAP_OP
};

enum {
    FL_POS = 1 << 0,
    FL_ZRO = 1 << 1,
    FL_NEG = 1 << 2,
};

uint16_t memory[MAX_MEMORY];
uint16_t registers[10]; // Create the register array with 10 total registers. 


int main (int argc, const char *argv) { // Why do we use a const?

}