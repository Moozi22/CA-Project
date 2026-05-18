#ifndef ALU_H
#define ALU_H

#include <stdint.h>


int8_t execute(int opcode, int8_t val1, int8_t val2);


uint16_t calculateBranch(uint16_t currentPC, int8_t immediate);


int testALU(void);

#endif 
