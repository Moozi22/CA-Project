#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>


extern uint16_t instructionMemory[1024];


extern int8_t dataMemory[2048];


extern uint16_t PC;


void initMemory();


void storeInstruction(uint16_t address, uint16_t encodedInstruction);


uint16_t readInstruction(uint16_t address);


int8_t readData(uint16_t address);


void writeData(uint16_t address, int8_t value);


uint16_t encodeRFormat(uint8_t opcode, uint8_t r1, uint8_t r2);


uint16_t encodeIFormat(uint8_t opcode, uint8_t r1, int8_t imm);


int loadProgram(const char *filename);


void printInstructionMemory();


void printDataMemory();

#endif 
