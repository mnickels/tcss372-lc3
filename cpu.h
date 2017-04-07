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

typedef unsigned short Register;

typedef struct alu_s {
	Register A;
	Register B;
	Register R;
} ALU_s;

typedef struct inst_reg {
	Register ir;
	unsigned short opcode:4;
	unsigned short rd:3;
	unsigned short rs1:3;
	unsigned short rs2:3;
	unsigned short immed6:6;
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

#endif