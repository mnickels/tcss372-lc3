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
	unsigned short opcode;
	unsigned short rd;
	unsigned short rs1;
	unsigned short rs2;
	unsigned short immed6;  //Mike I couldnt find anything online to explain how to use those bit amount identifiers so i just removed them for now and will shift bits around to the front
                                //
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



#endif