#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>

extern int8_t GPR[64];

extern uint8_t SREG;

#define SREG_Z_BIT 0
#define SREG_S_BIT 1
#define SREG_N_BIT 2
#define SREG_V_BIT 3
#define SREG_C_BIT 4

#define FLAG_Z ((SREG >> SREG_Z_BIT) & 1)
#define FLAG_S ((SREG >> SREG_S_BIT) & 1)
#define FLAG_N ((SREG >> SREG_N_BIT) & 1)
#define FLAG_V ((SREG >> SREG_V_BIT) & 1)
#define FLAG_C ((SREG >> SREG_C_BIT) & 1)

void initRegisters(void);


int8_t getRegister(uint8_t regNum);


void setRegister(uint8_t regNum, int8_t value, const char *stageName);


void updateCarryFlag(int val1, int val2);


void updateOverflowFlag(int8_t val1, int8_t val2, int8_t result8, int isSubtraction);

void updateNegativeFlag(int8_t result8);

void updateSignFlag(void);

void updateZeroFlag(int8_t result8);


void updateFlagsADD(int8_t val1, int8_t val2, int8_t result8);

void updateFlagsSUB(int8_t val1, int8_t val2, int8_t result8);

void updateFlagsNZ(int8_t result8);


void printRegisters(void);

void printSREG(void);

#endif 
