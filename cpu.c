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
                cpu->pc += 1;

                cpu ->mdr = memory[cpu->mar];

                cpu->ir.ir = cpu->mdr;

                printf("  IR=%04X\n", cpu->ir.ir);
                state = DECODE;
                break;

            case DECODE: // microstate 32
                printf("\nDECODE\n");

                // get the fields out of the IR
                parseIR(cpu->ir);

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
                        if (cpu->ir.ir & BIT_5) {
                            op2 = sext5(cpu->ir.immed5);
                        } else {
                            op2 = cpu->ir.rs2;
                        }
                        break;
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
                        cpu->alu.B = op2;
                        cpu->alu.A = cpu->ir.rs1;
                        break;
                    case NOT_OPCODE:
                        cpu->alu.A = cpu->ir.rs1;
                        break;

                    case ST_OPCODE:
                        cpu->mdr = cpu->reg_file[cpu->ir.rd];  //state 23
                        break;
                    case LD_OPCODE:
                        cpu->mdr = memory[cpu->mar]; // state 25
                        break;
                    case TRAP_OPCODE:
                        cpu->mdr = memory[cpu->mar];
                        cpu->reg_file[7] = cpu->pc; // pc to reg 7
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
                        break;
                    case AND_OPCODE:
                        cpu->alu.R = cpu->ir.rs1 & op2;
                        break;
                    case NOT_OPCODE:
                        cpu->alu.R = ~cpu->alu.A;
                        break;
                    case TRAP_OPCODE:
                        cpu->pc = cpu->mdr;
                        //state 30
                        break;
                    case JMP_OPCODE:
                        cpu->pc = cpu->ir.rs1;
                        break;
                    case BR_OPCODE:
                        if (ben) {
                            cpu->pc = cpu->pc + sext9(cpu->ir.off9); // state 22
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
                        cpu->reg_file[cpu->ir.rd] = cpu->alu.R;
                        break;
                    case ADD_OPCODE:
                        cpu->reg_file[cpu->ir.rd] = cpu->alu.R;
                        break;
                    case ST_OPCODE:
                        memory[cpu->mar] =  cpu->mdr; //    state 16
                        break;
                    case LD_OPCODE:
                        cpu->reg_file[cpu->ir.rd] = cpu->mdr; // state 27
                        break;
                    case NOT_OPCODE:
                        cpu->reg_file[cpu->ir.rd] = cpu->alu.R;
                        break;
                    // write back to register or store MDR into memory
                }

                // do any clean up here in prep for the next complete cycle
                state = FETCH;
                break;

		}
        printf("\nRegister File:\n");
        int i;
        for (i = 0; i < REGISTER_FILE_SIZE; i++) {
            printf("  R[%d]=%4X\n", i, cpu->reg_file[i]);
        }
        printf("\nExecution paused, press any key to continue...\n");
        getch();
	}
}

int main(int argc, char* argv[]) {
	char* temp;

    if (argc != 2) {
        printf("Invalid number of command line arguments. Exiting program.\n");
        return 1;
    }

	memory[0] = strtol(argv[1], &temp, 16);
	printf("memory[0]: %4X=%d", memory[0], memory[0]);

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
 * Getting a character input without requiring the user to hit enter afterwards
 */
char getch() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}

/*
 * Parses the ir.ir into the INST_REG_s struct's other fields as appropriate
 * pre: ir.ir must contain the current IR value that needs to be parsed
 * post: ir.opcode, ir.rd, ir.sr1, ir.sr2, ir.immed5, ir.off9, and ir.trapvector
 *       will all be filled with the appropriate values from ir.ir
 */
unsigned short parseIR(INST_REG_s ir) {
    ir.opcode = ir.ir >> OPCODE_SHIFT;
    ir.rd = (ir.ir & RD_MASK) >> RD_SHIFT;
    ir.rs1 = (ir.ir & RS1_MASK) >> RS1_SHIFT;
    ir.rs2 = ir.ir & RS2_MASK;
    ir.immed5 = ir.ir & IMMED5_MASK;
    ir.off9 = ir.ir & OFF9_MASK;
    ir.trapvector = ir.ir & TRAPVECTOR_MASK;
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