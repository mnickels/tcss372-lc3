#include "cpu.h"

unsigned short memory[MEMORY_SIZE];

int controller(CPU_p cpu) {
	unsigned int state;
	state = FETCH;

	for (;;) {
		switch (state) {
			case FETCH: // microstates 18, 33, 35 in the book
                printf("Here in FETCH\n");
                // get memory[PC] into IR - memory is a global array
    
                cpu->pc = &memory[0];// should this be done in main?
                cpu->mar = cpu->pc;// PC TO MAR
                cpu ->mdr = cpu->mar;// MAR TO MDR
                cpu->ir.ir = *cpu->mdr// MDR TO IR
                        
              // increment PC? what does this look like if we are only taking in one instruction.

                        
                printf("Contents of IR = %04X\n", cpu->ir);
                
	
               state = DECODE;
                break;
            case DECODE: // microstate 32
                // get the fields out of the IR
                parseIR(cou->ir);
                
                
                // make sure opcode is in integer form
				// hint: use four unsigned int variables, opcode, Rd, Rs, and immed7
				// extract the bit fields from the IR into these variables
                state = EVAL_ADDR;
                break;
            case EVAL_ADDR: // Look at the LD instruction to see microstate 2 example
                switch (cpu->ir.opcode) {
                // different opcodes require different handling
                // compute effective address, e.g. add sext(immed7) to register
                }
                state = FETCH_OP;
                break;
            case FETCH_OP: // Look at ST. Microstate 23 example of getting a value out of a register
                switch (cpu->ir.opcode) {
                    // get operands out of registers into A, B of ALU
                    // or get memory for load instr.
                }
                state = EXECUTE;
                break;
            case EXECUTE: // Note that ST does not have an execute microstate
                switch (cpu->ir.opcode) {
                    // do what the opcode is for, e.g. ADD
                    // in case of TRAP: call trap(int trap_vector) routine, see below for TRAP x25 (HALT)
                }
                state = STORE;
                break;
            case STORE: // Look at ST. Microstate 16 is the store to memory
                switch (cpu->ir.opcode) {
                    // write back to register or store MDR into memory
                }
                // do any clean up here in prep for the next complete cycle
                state = FETCH;
                break;
		}
	}
}

int main(int argc, char* argv[]) {
	printf("test");
	char* temp;
	memory[0] = strtol(argv[1], &temp, 16);
	printf("memory[0]: %4X = %d", memory[0], memory[0]);
}