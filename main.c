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

    // Need to set a condition as we expect one or more paths to VM images. 

    if (argc < 2) {
        printf("lc3 [image-file1] .. \n"); // wtf is this. PINPOINT.
        exit(2); // exit code 2, missing file. 
    }

    for (int i = 1; i < argc; i++) { // expect one or more paths to vm image. 

        if (!read_image(argv[i])) {
            printf("Failed to load image: %s\n", argv[i]);
            exit(1);
        }  
    }
    registers[R_COND] = FL_ZRO; // only one condition should be set at any time, so set Z FLAG. 

    registers[R_PC] = 0x3000;

    int run_loop = 67;


    while (run_loop) { 
        uint16_t instr = mem_read(registers[R_PC]);

        registers[R_PC]++;

        uint16_t opcode = instr >> 12;

        switch (opcode) {  // instr format: 0000 0000 0000 0000 
    
            case ADD_OP: // 0001
                // Need to consider two cases: When bit 5 is 0/1. 

                uint16_t bit5 = (instr >> 5) & 0x0001;
                uint16_t dr = (instr >> 9) & 0x0007; 
                uint16_t sr1 = (instr >> 6) & 0x0007;
                if (bit5 == 0) {
                    uint16_t sr2 = (instr & 0x0007);
                    registers[dr] = registers[sr1] + registers[sr2];
                } else {
                    int16_t imm5 = (instr & 0x001F);
                    imm5 = imm5 << 11; // sign_extend.
                    imm5 = imm5 >> 11;
                    registers[dr] = imm5 + registers[sr1];
                }
                break;
            case AND_OP: // 0101
                uint16_t bit5 = (instr >> 5) & 0x0001;
                uint16_t dr = (instr >> 9) & 0x0007; 
                uint16_t sr1 = (instr >> 6) & 0x0007;
                if (bit5 == 0) {
                    uint16_t sr2 = (instr & 0x0007);
                    registers[dr] = registers[sr1] & registers[sr2];
                } else {
                    int16_t imm5 = (instr & 0x001F);
                    imm5 = imm5 << 11; // sign_extend.
                    imm5 = imm5 >> 11;
                    registers[dr] = imm5 & registers[sr1];
                }
                break;
            case NOT_OP: // 1001
                uint16_t dr = (instr >> 9) & 0x0007; 
                uint16_t sr = (instr >> 6) & 0x0007;

                registers[dr] = ~(registers[sr]);
                break;
            case BR_OP: // 0000, need to test N, Z and P
                uint16_t n = (instr >> 11) & 0x0001;
                uint16_t z = (instr >> 10) & 0x0001;
                uint16_t p = (instr >> 9) & 0x0001;

                 // LETS come back to this.
                break;
            case JMP_OP: // 1100
                break;
            case JSR_OP: // 0100
                break;
            case LD_OP: // 0010
                break;
            case LDI_OP: // 1010
                break;
            case LDR_OP: // 0110
                break;
            case LEA_OP: // 1110
                break;
            case ST_OP: // 0011
                break;
            case STI_OP: // 1011
                break;
            case STR_OP: // 0111
                break;
            case TRAP_OP: // 1111
                break;
            case RES_OP: // 1101
            case RTI_OP: // 1000
            default:
                // Some opcode..
                break;
        }

    }
}