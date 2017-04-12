#include "cpu.h"

unsigned short memory[MEMORY_SIZE];

unsigned short sext5(unsigned short immed5) {
    unsigned short sext;
    sext = immed5 & SEXT_5;
    if (immed5 & BIT_4) {
        sext |= SEXT_5_MASK;
    }
    return sext;
}

int controller(CPU_p cpu) {
	unsigned int state;
        unsigned int ben, op2;
        ben = 0;
        op2 = 0;
	state = FETCH;
        

	for (;;) {
            switch (state) {
            case FETCH: // microstates 18, 33, 35 in the book
                printf("Here in FETCH\n");
                // get memory[PC] into IR - memory is a global array
                
                cpu->pc = &memory[0];// should this be done in main?
                
                cpu->mar = cpu->pc;// PC TO MAR
                
                cpu ->mdr = *cpu->mar;// MAR TO MDR
                
                cpu->ir.ir = cpu->mdr// MDR TO IR
                    
                        
                        
              // increment PC? what does this look like if we are only taking in one instruction.

                        
                printf("contents of IR = %04X\n", cpu->ir);
                
                
               state = DECODE;
                break;
            case DECODE: // microstate 32
                
                  // get the fields out of the IR
                parseIR(cpu->ir);
                
               // This should take the IR[bits 9 to 11] and and them with the CC?
                
                if (cpu->alu.R == 0) {
                    ben |= (BIT_1 & (cpu->ir.rd & BIT_1)) >> 1;
                } else if (cpu->alu.R & BIT_15 == 0) {
                    ben |= (BIT_0 & (cpu->ir.rd & BIT_0));
                } else {
                    ben |= (BIT_2 & (cpu->ir.rd & BIT_2)) >> 2;
                }
                
                switch (cpu->ir.opcode) {
                    case AND_OPCODE:
                    case ADD_OPCODE:
                        if (cpu->ir.immed6 & BIT_5) {
                            op2 = sext5(cpu->ir.immed6);
                        } else {
                            op2 = cpu->ir.rs2;
                        }
                        break;
                    case BR_OPCODE:
                    case LD_OPCODE:
                    case ST_OPCODE:
                        // SEXT 9
                        break;
                }
                
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

unsigned short parseIR(INST_REG_s ir)
{   
    unsigned short mask = 3584 ;//  0b0000 1110 0000 0000 
    unsigned short temp = 0; // to store masking result
    
  
    ir.opcode = ir.ir >>12;
    
    
    temp = mask & ir.ir; 
    ir.rd = temp >> 9;
    
    mask = mask >> 3; // mask is 0b0000 0001 1100 0000
    temp = mask & ir.ir;
    ir.rs1 = temp >> 6;
    
    mask = mask >> 3; // mask is 0b0000 0000 0011 1000
    temp = mask & ir.ir;
    ir.rs2 = temp >> 3;
    
    mask = 63; //                0b0000 0000 0011 1111
    ir.immed6 = mask & ir.ir; 
    
    
    
}