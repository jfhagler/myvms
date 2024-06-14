#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>




struct MemoryRegion {
	uint32_t start_address;
	uint32_t size;
	uint32_t *memoryslice;  
}
