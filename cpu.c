/*
 * for TCSS 372 Spring 2017
 *
 * by Mike Nickels and Carter
 *
 * cpu.c
 * Simulates an LC-3 CPU.
 */

#include "cpu.h"

unsigned short memory[MEMORY_SIZE];

int controller(CPU_p cpu) {

    unsigned int state;
    unsigned int ben, op2;
    ben = 0;
    op2 = 0;
    state = FETCH;

	for (;;) {
        switch (state) {

            case FETCH: // microstates 18, 33, 35 in the book
                printf("\nFETCH\n");

                cpu->mar = cpu->pc;
                printf("  MAR <- x%X\n", cpu->mar);
                cpu->pc += 1;
                printf("  PC  <- x%X\n", cpu->pc);

                cpu ->mdr = memory[cpu->mar];
                printf("  MDR <- x%X\n", cpu->mdr);

                cpu->ir.ir = cpu->mdr;
                printf("  IR  <- x%X\n", cpu->ir.ir);

                state = DECODE;
                break;

            case DECODE: // microstate 32
                printf("\nDECODE\n");

                // get the fields out of the IR
                parseIR(&(cpu->ir));
                printf("  IR fields:\n");
                printf("    opcode = x%X\n", cpu->ir.opcode);
                printf("    DR = x%X\n", cpu->ir.rd);
                printf("    SR1 = x%X\n", cpu->ir.rs1);
                printf("    SR2 = x%X\n", cpu->ir.rs2);
                printf("    immed5 = x%X\n", cpu->ir.immed5);
                printf("    PCoffset9 = x%X\n", cpu->ir.off9);
                printf("    trapvect8 = x%X\n", cpu->ir.trapvector);

                if (cpu->alu.R == 0) {
                    ben |= (BIT_1 & (cpu->ir.rd & BIT_1)) >> 1;
                } else if (cpu->alu.R & BIT_15 == 0) {
                    ben |= (BIT_0 & (cpu->ir.rd & BIT_0));
                } else {
                    ben |= (BIT_2 & (cpu->ir.rd & BIT_2)) >> 2;
                }
                printf("  Branch ENabled = %d\n", ben);

                switch (cpu->ir.opcode) {

                }

                state = EVAL_ADDR;
                break;

            case EVAL_ADDR: // Look at the LD instruction to see microstate 2 example
                printf("\nEVALUATE ADDRESS\n");

                switch (cpu->ir.opcode) {
                    case ST_OPCODE:
                    case LD_OPCODE:
                        cpu->mar =  cpu->pc + sext9(cpu->ir.off9); // state 2,3,10,11
                        break;
                    case TRAP_OPCODE:
                        cpu->mar = zext(cpu->ir.trapvector); //state 15
                        break;
                }

                // different opcodes require different handling
                // compute effective address, e.g. add sext(immed7) to register

                state = FETCH_OP;
                break;

            case FETCH_OP: // Look at ST. Microstate 23   example of getting a value out of a register
                printf("\nFETCH OPERANDS\n");

                switch (cpu->ir.opcode) {
                    case ADD_OPCODE:
                    case AND_OPCODE:
                        if (cpu->ir.ir & BIT_5) {
                            printf("  OP2 <- immed5\n");
                            op2 = sext5(cpu->ir.immed5);
                        } else {
                            printf("  OP2 <- R[SR2]\n");
                            op2 = cpu->reg_file[cpu->ir.rs2];
                        }
                        printf("    OP2: x%X = %d\n", op2, op2);

                        printf("  ALU Registers:\n");
                        cpu->alu.A = cpu->ir.rs1;
                        printf("    A = x%X\n", cpu->alu.A);
                        cpu->alu.B = op2;
                        printf("    ALU Reg B = x%X\n", cpu->alu.B);
                        break;
                    case NOT_OPCODE:
                        cpu->alu.A = cpu->ir.rs1;
                        printf("  ALU Reg A = x%X\n", cpu->alu.A);
                        break;
                    case ST_OPCODE:
                        cpu->mdr = cpu->reg_file[cpu->ir.rd];  //state 23
                        printf("  MDR = x%X\n", cpu->mdr);
                        break;
                    case LD_OPCODE:
                        cpu->mdr = memory[cpu->mar]; // state 25
                        printf("  MDR = x%X\n", cpu->mdr);
                        break;
                    case TRAP_OPCODE:
                        cpu->mdr = memory[cpu->mar];
                        printf("  MDR = x%X\n", cpu->mdr);
                        cpu->reg_file[7] = cpu->pc; // pc to reg 7
                        printf("  R7 <- PC=x%X\n", cpu->pc);
                        //state 28
                    break;

                    // get operands out of registers into A, B of ALU
                    // or get memory for load instr.
                }

                state = EXECUTE;
                break;

            case EXECUTE: // Note that ST does not have an execute microstate
                printf("\nEXECUTE\n");
                switch (cpu->ir.opcode) {

                    case ADD_OPCODE:
                        cpu->alu.R = cpu->alu.A + cpu->alu.B;
                        printf("  x%X + x%X = x%X    (%d + %d = %d)", cpu->alu.A, cpu->alu.B, cpu->alu.R, cpu->alu.A, cpu->alu.B, cpu->alu.R);
                        break;
                    case AND_OPCODE:
                        cpu->alu.R = cpu->ir.rs1 & op2;
                        printf("  x%X & x%X = x%X    (%d & %d = %d)", cpu->alu.A, cpu->alu.B, cpu->alu.R, cpu->alu.A, cpu->alu.B, cpu->alu.R);
                        break;
                    case NOT_OPCODE:
                        cpu->alu.R = ~cpu->alu.A;
                        printf("  NOT x%X = x%X    (NOT %d = %d)", cpu->alu.A, cpu->alu.R, cpu->alu.A, cpu->alu.R);
                        break;
                    case TRAP_OPCODE:
                        cpu->pc = cpu->mdr;
                        printf("  PC <- x%X", cpu->pc);
                        //state 30
                        break;
                    case JMP_OPCODE:
                        cpu->pc = cpu->ir.rs1;
                        printf("  PC <- x%X", cpu->pc);
                        break;
                    case BR_OPCODE:
                    printf("BEN: %d", ben);
                        if (ben) {
                            cpu->pc = cpu->pc + sext9(cpu->ir.off9); // state 22
                            printf("  PC <- x%X", cpu->pc);
                        }
                        break;
                    // do what the opcode is for, e.g. ADD

                    // in case of TRAP: call trap(int trap_vector) routine, see below for TRAP x25 (HALT)
                }
                state = STORE;
                break;

            case STORE: // Look at ST. Microstate 16        and37?i s the store to memory
                printf("\nSTORE\n");
                switch (cpu->ir.opcode) {
                    case AND_OPCODE:
                    case ADD_OPCODE:
                        cpu->reg_file[cpu->ir.rd] = cpu->alu.R;
                        printf("  R%d <- x%X", cpu->ir.rd, cpu->alu.R);
                        break;
                    case ST_OPCODE:
                        memory[cpu->mar] =  cpu->mdr; //    state 16
                        printf("  M[%d] <- x%X", cpu->mar, memory[cpu->mar]);
                        break;
                    case LD_OPCODE:
                        cpu->reg_file[cpu->ir.rd] = cpu->mdr; // state 27
                        printf("  R%d <- x%X", cpu->ir.rd, cpu->reg_file[cpu->ir.rd]);
                        break;
                    case NOT_OPCODE:
                        cpu->reg_file[cpu->ir.rd] = cpu->alu.R;
                        printf("  R%d <- x%X", cpu->ir.rd, cpu->reg_file[cpu->ir.rd]);
                        break;
                    // write back to register or store MDR into memory
                }

                // do any clean up here in prep for the next complete cycle
                printf("\nRegister File:\n");
                int i;
                for (i = 0; i < REGISTER_FILE_SIZE; i++) {
                    printf("  R[%d] = x%4X\n", i, cpu->reg_file[i]);
                }

                printf("Memory[0:7]:\n");
                for (i = 0; i < 8; i++) {
                    printf("  M[%d] = x%4X\n", i, memory[i]);
                }

                printf("IR: x%4X\n", cpu->ir.ir);

                printf("PC: x%4X = %d\n", cpu->pc, cpu->pc);

                return 0;

                state = FETCH;
                break;

		}
	}
}

int main(int argc, char* argv[]) {
	char* temp;

    if (argc != 2) {
        printf("Invalid number of command line arguments. Exiting program.\n");
        return 1;
    }

	memory[0] = strtol(argv[1], &temp, 16);

    CPU_p cpu = malloc(sizeof(CPU_s));
    cpu->pc = 0;

    // initialize registers with data for testing
    cpu->reg_file[0] = 0xF;
    cpu->reg_file[1] = 0x1D;
    cpu->reg_file[2] = 0x0;
    cpu->reg_file[3] = 0xAA;

    controller(cpu);
}

/*
 * Parses the ir.ir into the INST_REG_s struct's other fields as appropriate
 * pre: ir.ir must contain the current IR value that needs to be parsed
 * post: ir.opcode, ir.rd, ir.sr1, ir.sr2, ir.immed5, ir.off9, and ir.trapvector
 *       will all be filled with the appropriate values from ir.ir
 */
unsigned short parseIR(INST_REG_s* ir) {
    ir->opcode = ir->ir >> OPCODE_SHIFT;
    ir->rd = (ir->ir & RD_MASK) >> RD_SHIFT;
    ir->rs1 = (ir->ir & RS1_MASK) >> RS1_SHIFT;
    ir->rs2 = ir->ir & RS2_MASK;
    ir->immed5 = ir->ir & IMMED5_MASK;
    ir->off9 = ir->ir & OFF9_MASK;
    ir->trapvector = ir->ir & TRAPVECTOR_MASK;
}

unsigned short sext5(unsigned short immed5) {
    unsigned short sext;
    sext = immed5 & IMMED5_MASK;
    if (immed5 & BIT_4) {
        sext |= SEXT5_MASK;
    }
    return sext;
}
unsigned short sext9(unsigned short immed9) {
    unsigned short sext;
    sext = immed9 & OFF9_MASK;
    if (immed9 & BIT_8) {
        sext |= SEXT9_MASK;
    }
    return sext;
}
unsigned short zext(unsigned short trapvector) {
    return trapvector;
}