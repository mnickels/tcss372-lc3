#ifndef CPU_HEADER
#define CPU_HEADER

// Standard C library includes
#include <stdio.h>
#include <stdlib.h>

// Microstates of the CPU
#define FETCH 0
#define DECODE 1
#define EVAL_ADDR 2
#define FETCH_OP 3
#define EXECUTE 4
#define STORE 5

// Single bit bitmasks (labeled as 'BIT_#', where the bit is in the 2^# place in binary representation)
#define BIT_15 0x8000
#define BIT_0 0x1
#define BIT_1 0x2
#define BIT_2 0x4
#define BIT_4 0x8
#define BIT_5 0x10
#define BIT_8 0x100

// Immediate value related bitmasks (include octets as binary in a comment)
#define IMMED_5_MASK 0x1F   // 0000 0000 0001 1111
#define SEXT_5_MASK 0xFFE0  // 1111 1111 1110 0000
#define IMMED_9_MASK 0x1FF  // 0000 0001 1111 1111
#define SEXT_9_MASK 0xFE00  // 1111 1110 0000 0000

// Opcodes (sorted by value)
#define BR_OPCODE 0x0
#define ADD_OPCODE 0x1
#define LD_OPCODE 0x2
#define ST_OPCODE 0x3
#define AND_OPCODE 0x5
#define NOT_OPCODE 0x9
#define JMP_OPCODE 0xC
#define TRAP_OPCODE 0xF

// Other constants
#define MEMORY_SIZE 32
#define REGISTER_FILE_SIZE 8

// define a Register as a 16 bit unsigned short
typedef unsigned short Register;

// ALU struct
typedef struct alu_s {
	Register A;
	Register B;
	Register R;
} ALU_s;

// Instruction Register struct (goes in CPU)
typedef struct inst_reg {
	Register ir;
	unsigned short opcode;
	unsigned short rd;
	unsigned short rs1;
	unsigned short rs2;
	unsigned short immed6;  // should this be 5?
                  unsigned short off9;
                  unsigned short trapvector;


} INST_REG_s;

// CPU struct
typedef struct cpu_s {
	Register reg_file[REGISTER_FILE_SIZE];
	INST_REG_s ir;
	Register pc;
	Register sext, mar, mdr;
	ALU_s alu;
} CPU_s;
typedef CPU_s* CPU_p;

// function declarations
int controller(CPU_p cpu);
int main(int argc, char* argv[]);
unsigned short parseIR(INST_REG_s ir);
unsigned short sext5(unsigned short immed5);
unsigned short sext9(unsigned short immed9);
unsigned short zext(unsigned short trapvector);

#endif