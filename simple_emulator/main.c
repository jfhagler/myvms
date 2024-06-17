#include <stdio.h>
#include <stdbool.h>

int stack[256]; 
int pc = 0; 
int sp = -1;
bool running = true; 

typedef enum InstructionSet{
	PSH, //pushes following program element to the stack 
	POP, //decrement stack pointer 
	ADD, //add previous two values in the stack, decrement SP twice 
	MUL, //multiplication of previous two values in the stack, decrement SP twice
	HLT  //end program 
} InstructionSet; 

typedef enum {
	A,B,C,D,E,F,G,SP,PC,NUM_REGS 
} Registers; 

void fetch_eval_execute(int *program) {
	int instr = program[pc]; 
	switch (instr) {
		case HLT: 
			running = false;
			break;
				
		case PSH: 
			stack[++sp] = program[++pc]; 	
			break;
		case POP:
			sp--; 
		case ADD:		
			int x = stack[sp--]; 
			int y = stack[sp--];
			int result = x+y;
			stack[++sp] = result;
			break; 	
		default: 	
			break; 
	}
	pc++;  
}


// define test program and run it. emulator is code above, below is the test
int program[] = {PSH, 5, PSH, 6, ADD}; 

int main() { 
	while (running) {
		fetch_eval_execute(program); 
	}
	return 0;
}



