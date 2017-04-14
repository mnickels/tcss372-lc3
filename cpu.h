#ifndef CPU_HEADER
#define CPU_HEADER

#include <stdio.h>
#include <stdlib.h>

#define FETCH 0
#define DECODE 1
#define EVAL_ADDR 2
#define FETCH_OP 3
#define EXECUTE 4
#define STORE 5

#define MEMORY_SIZE 32
#define REGISTER_FILE_SIZE 8

#define BIT_15 0x8000
#define BIT_0 0x1
#define BIT_1 0x2
#define BIT_2 0x4
#define BIT_4 0x8
#define BIT_5 0x10
#define BIT_8 0x100

#define SEXT_5 0x1F         // 0000 0000 0001 1111
#define SEXT_5_MASK 0xFFE0  // 1111 1111 1110 0000

#define SEXT_9 0x1FF         // 0000 0001 1111 1111
#define SEXT_9_MASK 0xFE00  // 1111 1110 0000 0000

#define ADD_OPCODE 0x1
#define AND_OPCODE 0x5
#define NOT_OPCODE 0x9
#define TRAP_OPCODE 0xF
#define LD_OPCODE 0x2
#define ST_OPCODE 0x3
#define JMP_OPCODE 0xC
#define BR_OPCODE 0x0


typedef unsigned short Register;

typedef struct alu_s {
	Register A;
	Register B;
	Register R;
} ALU_s;

typedef struct inst_reg {
	Register ir;
	unsigned short opcode;
	unsigned short rd;
	unsigned short rs1;
	unsigned short rs2;
	unsigned short immed6;  // should this be 5?
                  unsigned short off9;
        
                   
} INST_REG_s;

typedef struct cpu_s {
	Register reg_file[REGISTER_FILE_SIZE];
	INST_REG_s ir;
	Register pc;
	Register sext, mar, mdr;
	ALU_s alu;
} CPU_s;
typedef CPU_s* CPU_p;

int controller(CPU_p cpu);
int main(int argc, char* argv[]);
unsigned short parseIR(INST_REG_s ir);
nsigned short sext5(unsigned short immed5);

#endif