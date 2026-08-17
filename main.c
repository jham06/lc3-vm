#include <stdio.h>
#include <stdint.h>
#define MAX_MEMORY 65536

enum { // Set the enum values for each registers. 
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

enum { // set the enum values for each operation. 
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


int main (int argc, const char *argv) {// Why do we use a const?
    registers[R_COND] = FL_ZRO; // only one condition should be set at any time, so set Z FLAG. 

    registers[R_PC] = 0x3000;

    int run_loop = 67;

    while (run_loop) { 
        uint16_t instr = mem_read(registers[R_PC]);

        registers[R_PC]++;

        uint16_t opcode = instr >> 12;

        switch (opcode) {
    
            case ADD_OP:
                break;
            case AND_OP:
                break;
            case NOT_OP:
                break;
            case BR_OP:
                break;
            case JMP_OP:
                break;
            case JSR_OP:
                break;
            case LD_OP:
                break;
            case LDI_OP:
                break;
            case LDR_OP:
                break;
            case LEA_OP:
                break;
            case ST_OP:
                break;
            case STI_OP:
                break;
            case STR_OP:
                break;
            case TRAP_OP:
                break;
            case RES_OP:
            case RTI_OP:
            default:
                // Some opcode..
                break;
        }

    }
}