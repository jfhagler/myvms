#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MEMORY_MAX (1<<16)
#define PC_START 0x3000

int bool running = 1; 
uint16_t memory[MEMORY_MAX];

enum {
	R_R0 = 0,
	R_R1,
	R_R2,
	R_R3,
	R_R4,
	R_R5,
	R_R6,
	R_R7,
	R_PC,
	R_COND,
	R_COUNT
};

uint16_t registers[R_COUNT];

enum {
	OP_BR = 0, //brach
	OP_ADD,    //add
	OP_LD,     //load
	OP_ST,     //store
	OP_JSR,    //jump register
	OP_AND,    //bitwise and
	OP_LDR,    //load register 
	OP_STR,    //store register 
	OP_RTI,    //unused 
	OP_NOT,    //bitwise not 
	OP_LDI,    //load indirect 
	OP_STI,    //store indirect
	OP_JMP,    //jump
	OP_RES,    //reserved unused 
	OP_LEA,    //load effetive address
	OP_TRAP,   //execute trap
};

enum {
	FL_POS = 1<<0,
	FL_ZERO = 1<<1,
	FL_NEG = 1<<2,
};


enum { 
	TRAP_GETC = 0x20,
	TRAP_OUT = 0x21,
	TRAP_PUTS = 0x22,
	TRAP_IN = 0x23,
	TRAP_PUTSP = 0x24,
	TRAP_HALT = 0x25,
};

uint16_t sign_extend(uint16_t x, int bit_count){	
	if ((x >> (bit_count - 1)) & 1) {
		x |= (0xFFFF << bit_count);
	}
	return x;
}

int mem_read() {
}

int mem_write() {
}

int update_flags(uint16_t r) {
	if (reg[r] == 0) {
		reg[R_COND] = FL_ZERO;
	} else if (reg >> 15) {
		reg[R_COND] = FL_NEG;
	} else {
		reg[R_COND] = FL_POS; 
	}
} 

int fetch_eval_execute() { 
	uint16_t instr = mem_read(reg[R_PC]++);
	uint16_t op = instr >> 12;
	switch (op) {
		case OP_ADD:
			uint16_t r0 = (instr >> 9) & 0x7;
			uint16_t r1 = (instr >> 6) & 0x7;
			uint16_t imm_flag = (instr >> 5) & 0x1;
			if (imm_flag) {
				uint16_t imm5 = sign_extend(instr & 0x1F, 5);
				reg[r0] = reg[r1] + imm5;
			} else {
				uint16_t r2 = instr & 0x7;
				reg[r0] = reg[r1] + reg[r2];
			}
			update_flags(r0);
			break;
		case OP_AND:
			uint16_t dr = (instr >> 9) & 0x7;
			uint16_t sr1 = (instr >> 6) & 0x7;
			uint16_t imm_flag = (instr >> 5) & 0x01;	
			if (imm_flag) {
				uint16_t imm5 = sign_extend(instr & 0x1F, 5);
				reg[dr] = reg[sr1] & imm5;			
			} else { 
				uint16_t sr2 = instr & 0x07;
				reg[dr] = reg[sr1] & reg[sr2]; 
			}			
			break;
		case OP_NOT:
			uint16_t dr = (instr >> 9) & 0x7;
			uint16_t sr = (instr >> 6) & 0x7;
			reg[dr] = ~reg[sr];
			update_flags(dr);
			break;
		case OP_BR:
			uint16_t n = (intsr >> 11) & 0x1;
			uint16_t z = (intsr >> 10) & 0x1;
			uint16_t p = (instr >> 9)  & 0x1;
			uint16_t pc_offset = sign_extend(instr & 0x1FF, 9); 
			if ((n|z|p) & reg[R_COND]) {	
				reg[R_PC] += pc_offset;	
			} 
			break;
		case OP_JMP:
			uint16_t br = (instr >> 6) & 0x7;
			reg[R_PC] = reg[br]; 
			break;
		case OP_JSR:
			uint16_t tesbit = (instr >> 11) & 0x1;
			if (testbit) {	
				uint16_t pc_offset = sign_extend(instr & 0x7FF, 11);
				reg[R_PC] += pc_offset;		
			} else {
				uint16_t br = (instr >> 6) & 0x7; 
				reg[R_PC] = reg[br];	
			}
                        break; 
		case OP_LD:
			uint16_t dr = (instr >> 9) & 0x7;
			uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
			reg[dr] = mem_read(reg[R_PC] + pc_offset); 
			update_flags(dr);
			break;
		case OP_LDI:
			uint16_t r0 = (instr >> 9) & 0x7;
			uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
			reg[r0] = mem_read(mem_read(reg[R_PC] + pc_offset));
			update_flags(r0);
			break;
		case OP_LDR:
			uint16_t dr = (instr >> 9) & 0x7;
			uint16_t br = (instr >> 6) & 0x7; 
			uint16_t offset = sign_extend( instr & 0x3F, 6);
			reg[dr] = mem_read(reg[br]+offset);
			update_flags(dr);
			break;
		case OP_LEA:
			uint16_t dr = (instr >> 9) & 0x7;
			uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
			reg[dr] = reg[R_PC] + pc_offset;
			update_flags(dr);  
			break;
		case OP_ST:
			uint16_t sr = (instr >> 9) & 0x7;
			uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
			mem_write(reg[R_PC] + pc_offset); 
			break;
		case OP_STI:
			uint16_t sr = (instr >> 9) & 0x7;
			uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
			mem_write(mem_read(reg[R_PC] + pc_offset), reg[sr]); 
			break;
		case OP_STR:
			uint16_t sr = (instr >> 9) & 0x7;
			uint16_t br = (instr >> 9) & 0x7;
			uint16_t pc_offset = sign_extend(instr & 0x3F, 6); 
			mem_write(reg[br] + pc_offset, reg[sr]);
			break;
		case OP_TRAP:
			reg[R_R7] = reg[R_PC];
			switch (instr & 0xFF) { 
				case TRAP_GETC:
					reg[R_R0] = (uint16_t)getchar();
					update_flags(R_R0);
					break;
				case TRAP_OUT:
					putc((char)reg[R_R0], stdout);
					fflush(stdout);
					break;
				case TRAP_PUTS:
					uint16_t *c = memory + reg[R_R0];
					while (*c) {
						putc((char)*c , stdout);
						++c;	
					}
					fflush(stdout); 
					break;
				case TRAP_IN:
					printf("Enter a character: ");
					char c = getchar();
					putc(c, stdout);
					fflush(stdout);
					reg[R_R0] = (uint16_t) c;
					update_flags(R_R0);
					break;
				case TRAP_PUTSP:
					uint16_t *c = memory + reg[R_R0];
					while (*c) {
				 		char char1 = (*c) & 0xFF;
						putc (char1, stdout);
						char char2 = (*c) >> 8;
						if (char2) putc(char2, stdout);
						++c; 
					}
					fflush(stdout); 	
					break;
				case TRAP_HALT:
					puts("HALT");
					fflush(stdout);
					running = 0; 
					break;
				default:
					break;	
			}	
			break;
		case OP_RES:
		case OP_RTI:
		default:
			break;
	} 
}

int main(int argc, const char **argv) {
	if (argv < 2) {	
		printf("lc3 [image-file1] .. \n");
		exit(2);
	}


	for (int j = 1; j < argc; j++) {
		if (!read_image(argv[j])) {
			printf("failed to load image: %s\n", argv[j]);
			exit(1);	
		}
	}
}
