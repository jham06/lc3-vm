#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <Windows.h>
#include <conio.h> 
#define MAX_MEMORY 65536

/*CREDIT: https://www.jmeiners.com/lc3-vm/ */

HANDLE hStdin = INVALID_HANDLE_VALUE;
DWORD fdwMode, fdwOldMode;

enum {
    TRAP_GETCHAR = 0x20, // get char from keyboard, not echoed to the terminal
    TRAP_OUT = 0x21, // output a char
    TRAP_PUTS = 0x22, // output a word str
    TRAP_INPUT = 0x23, // get char from keyboard, echoed into terminal
    TRAP_PUTSP = 0x24, // output a byte string
    TRAP_HALT = 0x25 // halt the program..
};
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

enum
{
    MR_KBSR = 0xFE00, /* keyboard status */
    MR_KBDR = 0xFE02  /* keyboard data */
};

uint16_t memory[MAX_MEMORY];
uint16_t registers[10]; // Create the register array with 10 total registers. 

void disable_input_buffering() 
{
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hStdin, &fdwOldMode); /* save old mode */
    fdwMode = fdwOldMode
            ^ ENABLE_ECHO_INPUT  /* no input echo */
            ^ ENABLE_LINE_INPUT; /* return when one or
                                    more characters are available */
    SetConsoleMode(hStdin, fdwMode); /* set new mode */
    FlushConsoleInputBuffer(hStdin); /* clear buffer */
}

void restore_input_buffering()
{
    SetConsoleMode(hStdin, fdwOldMode);
}

uint16_t check_key()
{
    return WaitForSingleObject(hStdin, 1000) == WAIT_OBJECT_0 && _kbhit();
}


void handle_interrupt(int signal)
{
    restore_input_buffering();
    printf("\n");
    exit(-2);
}


uint16_t swap16_bits(uint16_t value) {
    uint16_t swapped = (value >> 8) | (value << 8); // no need to mask. 
    return swapped;
} 

// Have to rememeber, lc3 machiens are big_endian...
void update_flag (uint16_t r) { // any time a value is written into the register, need to update flags to indicate its sign. 
    if (registers[r] == 0) {
        registers[R_COND] = FL_ZRO;
    } else if (registers[r] >> 1) { // If 15th bit is set i.e. its negative..
        registers[R_COND] = FL_NEG;
    } else {
        registers[R_COND] = FL_POS;
    }
}   

void read_image_file(FILE *file) {
    uint16_t origin_ptr; // need to set a original pointer in order to set where to store the img. 
    fread(&origin_ptr, sizeof(uint16_t), 1, file);
    origin_ptr = swap16_bits(origin_ptr);

    uint16_t *max_ptr = memory + origin_ptr;
    uint16_t max_read = MAX_MEMORY - origin_ptr;
    size_t reader = fread(max_ptr, sizeof(uint16_t), max_read, file); // this returns how many elements were read, with max_ptr now starting at the memory + original ptr (which is the after first 2 bytes. )

    // because C is a little endian (first byte on lsb, second byte on msb), need to swap. 
    while (reader-- > 0) {
        *max_ptr = swap16_bits(*max_ptr); // simply switch the bits. 
        max_ptr++;
    }

}


int read_image (const char* image_path) { // needs to read a string, so apply a char* in order to do so
        FILE *file_ptr = fopen(image_path, "rb");
        if(file_ptr == 0) {
            return 0;
        } 
        read_image_file(file_ptr);
        fclose(file_ptr);
        return 1;
}

uint16_t mem_read (uint16_t addr){
    if (addr == MR_KBSR) {
        if (check_key()) {
            memory[MR_KBSR] = (1 << 15);
            memory[MR_KBDR] = getchar();
        } else {
            memory[MR_KBSR] = 0;
        }
    }
    return memory[addr];
}

uint16_t mem_write (uint16_t dest, uint16_t value) {
    memory[dest] = value;

}

int main (int argc, const char *argv[]) {// Why do we use a const?

    signal(SIGINT, handle_interrupt);
    disable_input_buffering();

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
                uint16_t dr_add = (instr >> 9) & 0x0007; 
                uint16_t sr1_add = (instr >> 6) & 0x0007;
                if (bit5 == 0) {
                    uint16_t sr2 = (instr & 0x0007);
                    registers[dr_add] = registers[sr1_add] + registers[sr2];
                } else {
                    int16_t imm5 = (instr & 0x001F);
                    imm5 = imm5 << 11; // sign_extend.
                    imm5 = imm5 >> 11;
                    registers[dr_add] = imm5 + registers[sr1_add];
                }

                update_flag(dr_add);
                break;
            case AND_OP: // 0101
                uint16_t bit5_and = (instr >> 5) & 0x0001;
                uint16_t dr_and = (instr >> 9) & 0x0007; 
                uint16_t sr1_and = (instr >> 6) & 0x0007;
                if (bit5_and == 0) {
                    uint16_t sr2 = (instr & 0x0007);
                    registers[dr_and] = registers[sr1_and] & registers[sr2];
                } else {
                    int16_t imm5 = (instr & 0x001F);
                    imm5 = imm5 << 11; // sign_extend.
                    imm5 = imm5 >> 11;
                    registers[dr_and] = imm5 & registers[sr1_and];
                }
                update_flag(dr_and);
                break;
            case NOT_OP: // 1001
                uint16_t dr_not = (instr >> 9) & 0x0007; 
                uint16_t sr_not = (instr >> 6) & 0x0007;

                registers[dr_not] = ~(registers[sr_not]);
                update_flag(dr_not);
                break;
            case BR_OP: // 0000, need to test N, Z and P
                uint16_t n = (instr >> 11) & 0x0001;
                uint16_t z = (instr >> 10) & 0x0001;
                uint16_t p = (instr >> 9) & 0x0001;
                int16_t pc_offset_br = (instr & 0x01FF); // shouldve been an signed int. 
                pc_offset_br = pc_offset_br << 7;
                pc_offset_br = pc_offset_br >> 7;

                 // LETS come back to this.

                 if ((n && FL_NEG) || (z && FL_ZRO) || (p && FL_POS)) {
                    registers[R_PC] = registers[R_PC] + pc_offset_br;
                 }

                break;
            case JMP_OP: // 1100
                uint16_t base_reg = (instr >> 6) & 0x0003;
                registers[R_PC] = registers[base_reg];
                break;
            case JSR_OP: // 0100
                registers[R7] = registers[R_PC];
                uint16_t bit11 = (instr >> 11) & 0x0001;
                int16_t offset = (instr & 0x07FF);
                offset = offset << 5;
                offset = offset >> 5;
                if (bit11 == 1) {
                    registers[R_PC] = registers[R_PC] + offset;
                }
                break;
            case LD_OP: // 0010
                uint16_t dr_ld = (instr >> 9) & 0x0007;
                int16_t pc_offset_ld = (instr & 0x01FF);
                pc_offset_ld = pc_offset_ld << 7;
                pc_offset_ld = pc_offset_ld >> 7;

                registers[dr_ld] = mem_read(registers[R_PC] + pc_offset_ld); // use mem_read for now..
                update_flag(dr_ld);
                break;
            case LDI_OP: // 1010
                uint16_t dr_ldi = (instr >> 9) & 0x0007;
                int16_t pc_offset_ldi = (instr & 0x01FF);
                pc_offset_ldi = pc_offset_ldi << 7;
                pc_offset_ldi = pc_offset_ldi >> 7;

                registers[dr_ldi] = mem_read(mem_read(registers[R_PC] + pc_offset_ldi)); // In this case, store the address. 
                update_flag(dr_ldi);
                break;
            case LDR_OP: // 0110
                uint16_t dr_ldr = (instr >> 9) & 0x0007;
                uint16_t base_r = (instr >> 6) & 0x0007;
                int16_t offset6 = (instr & 0x003F);
                offset6 = offset6 << 10;
                offset6 = offset6 >> 10;

                registers[dr_ldr] = mem_read(registers[base_r] + offset6); // use mem_read for now..
                update_flag(dr_ldr);
                break;
            case LEA_OP: // 1110
                uint16_t dr_lea = (instr >> 9) & 0x0007;
                int16_t pc_offset_lea = (instr & 0x01FF);
                pc_offset_lea = pc_offset_lea << 7;
                pc_offset_lea = pc_offset_lea >> 7;

                registers[dr_lea] = (registers[R_PC] + pc_offset_lea); // In this case, store the address. 
                update_flag(dr_lea);
                break;
            case ST_OP: // 0011
                uint16_t sr_st = (instr >> 9) & 0x0007;
                int16_t pc_offset_st = (instr & 0x01FF);
                pc_offset_st = pc_offset_st << 7;
                pc_offset_st = pc_offset_st >> 7;

                uint16_t value_st = registers[sr_st];
                
                mem_write(registers[R_PC] + pc_offset_st, value_st);
                break;
            case STI_OP: // 1011
                uint16_t sr_sti = (instr >> 9) & 0x0007;
                int16_t pc_offset_sti = (instr & 0x01FF);
                pc_offset_sti = pc_offset_sti << 7;
                pc_offset_sti = pc_offset_sti >> 7;

                uint16_t value_sti = registers[sr_sti];
                
                mem_write(mem_read(registers[R_PC] + pc_offset_sti), value_sti);
                break;
            case STR_OP: // 0111
                uint16_t sr_str = (instr >> 9) & 0x0007;
                uint16_t baseR = (instr >> 6) & 0x0007;
                int16_t pc_offset_str = (instr & 0x003F);
                pc_offset_str = pc_offset_str << 10;
                pc_offset_str = pc_offset_str >> 10;

                uint16_t value_str = registers[sr_str];
                
                mem_write(registers[baseR] + pc_offset_str, value_str);
                break;
            case TRAP_OP: // 1111
                registers[R7] = registers[R_PC];
                uint16_t trap = (instr & 0x00FF);
                trap = (uint16_t)trap; // ensuring it zero extends. 

                switch (trap) {
                    case TRAP_GETCHAR:
                        registers[R0] = (uint16_t)getchar(); // so now it clears the high 8 bits
                        update_flag(R0);
                        // IN this case, the r0 is actually the 0th register, not the specified register like above. 
                        break;
                    case TRAP_OUT:
                        putc((char)registers[R0], stdout); // use putc, with a integer mask of char in order to get the lower 8 bits. 
                        fflush(stdout); // EMpty out stdout
                        break;
                    case TRAP_PUTS:
                        uint16_t *ptr_address = memory + registers[R0];
                        while (*ptr_address) { // so until it doesnt equal zero. 
                            char char1 = *ptr_address; // one character per memory location
                            putchar((char)char1);
                            ptr_address++;

                        }
                        fflush(stdout);
                        break;
                    case TRAP_INPUT:
                        printf("Input a character: ");
                        char ch = getchar();
                        putchar(ch);
                        fflush(stdout);
                        registers[R0] = (uint16_t)ch;
                        update_flag(R0);
                        break;
                    case TRAP_PUTSP:
                    uint16_t *ptr_addr = memory + registers[R0];
                        while (*ptr_addr) { // so until it doesnt equal zero. Since it has 2 bytes, need to print out each.
                            char char1 = (*ptr_addr) & 0xFF; // two characters per memory location. lsb byte written first. 
                            putchar(char1);
                            char char2 = (*ptr_addr) >> 8;
                            if (char2) putchar(char1);
                            ptr_addr++;
                        }
                        break;
                    case TRAP_HALT:
                        // it means the vm, not the whole program. 
                        printf("Program stopped....\n");
                        fflush(stdout);
                        run_loop = 0;

                        break;
                }
                //registers[R_PC] = mem_read(trap); // dr not set so update_flag not called. 
                break;
            case RES_OP: // 1101, do nothing
            case RTI_OP: // 1000, do nothing
            default:
                // Some opcode..
                printf("Miscellaneous opcode: %x\n", opcode);
                break;
        }

    }

    restore_input_buffering();
}