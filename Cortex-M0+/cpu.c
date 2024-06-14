#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

struct CoreRegisters {
	// main registers
	uint32_t registers[13];
	uint32_t SP;
	uint32_t LR;
	uint32_t PC; 
	// special registers
	uint32_t PRIMASK;
	uint32_t CONTROL;
	uint32_t ASPR;
	uint32_t ISPR;
	uint32_t ESPR;
};

void CoreRegistersInit(struct CoreRegisters* c_regs) {
	c_regs->SP = *((uint32_t *)0x00000000);		
	c_regs->ISPR =  0x00000000; 
	c_regs->PRIMASK = 0x00000000;
	c_regs->CONTROL = 0x00000000; 
}

int main(){
	printf("hello");
	return 0;
} 
