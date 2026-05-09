#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

/* =========================================================
 *  PACKAGE 3 - Architect & Memory Lead
 *  memory.h  -  Public interface for all memory operations
 * =========================================================
 *
 *  Hardware being modelled:
 *    Instruction Memory : 1024 rows x 16 bits  (word-addressable)
 *    Data Memory        : 2048 rows x  8 bits  (byte/word-addressable)
 *    Program Counter    : 16-bit special register
 *
 *  Every other team member #includes this file to use
 *  readInstruction(), readData(), writeData(), etc.
 * ========================================================= */

/* ---------------------------------------------------------
 *  MEMORY ARRAYS  (defined in memory.c, shared via extern)
 * --------------------------------------------------------- */

/* Instruction memory: 1024 slots, each holding one 16-bit instruction.
 * uint16_t matches the spec's "short int / 16-bit" requirement exactly. */
extern uint16_t instructionMemory[1024];

/* Data memory: 2048 slots, each holding one 8-bit byte.
 * int8_t matches the spec's "char / int8_t" requirement exactly.
 * Signed so that values like -5 are stored and retrieved correctly. */
extern int8_t dataMemory[2048];

/* Program Counter - 16 bits wide (NOT 8 bits like the GPRs).
 * Owned here because it is tightly coupled to instruction fetching. */
extern uint16_t PC;

/* ---------------------------------------------------------
 *  INITIALISATION
 * --------------------------------------------------------- */

/* Zero-fill both memories and reset PC to 0. */
void initMemory();

/* ---------------------------------------------------------
 *  INSTRUCTION MEMORY  (read-only after loading)
 * --------------------------------------------------------- */

/* Store one encoded 16-bit instruction at the given address.
 * Called only by the parser / loadProgram() - never by the pipeline. */
void storeInstruction(uint16_t address, uint16_t encodedInstruction);

/* Fetch the 16-bit instruction at address.
 * Called by the pipeline's IF stage every clock cycle. */
uint16_t readInstruction(uint16_t address);

/* ---------------------------------------------------------
 *  DATA MEMORY  (read/write during execution)
 * --------------------------------------------------------- */

/* Load the 8-bit byte stored at address.
 * Used by the LDR instruction in the EX stage. */
int8_t readData(uint16_t address);

/* Store value at address.
 * Used by the STR instruction in the EX stage.
 * Prints a log line automatically (spec requirement). */
void writeData(uint16_t address, int8_t value);

/* ---------------------------------------------------------
 *  INSTRUCTION ENCODING  (used by the parser)
 * --------------------------------------------------------- */

/*  R-Format layout  [15:12]=opcode | [11:6]=R1 | [5:0]=R2
 *  Example: ADD R1 R2  ->  opcode=0, r1=1, r2=2             */
uint16_t encodeRFormat(uint8_t opcode, uint8_t r1, uint8_t r2);

/*  I-Format layout  [15:12]=opcode | [11:6]=R1 | [5:0]=IMM (signed 6-bit)
 *  Example: MOVI R5 10 ->  opcode=3, r1=5, imm=10            */
uint16_t encodeIFormat(uint8_t opcode, uint8_t r1, int8_t imm);

/* ---------------------------------------------------------
 *  PROGRAM LOADER  (parser: assembly text -> instruction memory)
 * --------------------------------------------------------- */

/* Read assembly from filename, encode each instruction into 16 bits,
 * and store it in instructionMemory starting at address 0.
 * Returns the number of instructions loaded.                  */
int loadProgram(const char *filename);

/* ---------------------------------------------------------
 *  END-OF-SIMULATION PRINTING  (spec requires full dump)
 * --------------------------------------------------------- */

/* Print every non-zero instruction memory slot (address + hex + binary). */
void printInstructionMemory();

/* Print every data memory slot (address + decimal value). */
void printDataMemory();

#endif /* MEMORY_H */
