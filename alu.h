#ifndef ALU_H
#define ALU_H

#include <stdint.h>

/* =========================================================
 *  PACKAGE 3 - Member 4: ALU & Execution Logic
 *  alu.h  -  Public interface for arithmetic logic operations
 * =========================================================
 *
 *  The ALU is the mathematical heart of the processor (EX stage).
 *  This module implements:
 *    - Arithmetic operations: ADD, SUB, MUL
 *    - Logical operations: ANDI, EOR
 *    - Shift operations: SLC (Shift Left Circular), SRC (Shift Right Circular)
 *    - Branch address calculation: PC = PC + 1 + IMM
 *
 *  All operations work with 8-bit signed integers and update
 *  the Status Register (SREG) flags as specified by the instruction set.
 * ========================================================= */

/* ---------------------------------------------------------
 *  ALU EXECUTION FUNCTION
 * --------------------------------------------------------- */

/**
 * execute() - Execute an ALU operation on two operands
 *
 * @opcode   : Instruction opcode (0-11), determines operation
 * @val1     : First operand (8-bit signed) - typically from R1 or immediate
 * @val2     : Second operand (8-bit signed) - typically from R2 or IMM
 *
 * Returns   : 8-bit signed result of the operation
 *
 * Operations by opcode:
 *   0 (ADD)   : val1 + val2, updates C, V, N, S, Z flags
 *   1 (SUB)   : val1 - val2, updates V, N, S, Z flags
 *   2 (MUL)   : val1 * val2 (16-bit product truncated to 8-bit), updates N, Z flags
 *   5 (ANDI)  : val1 & val2 (bitwise AND), updates N, Z flags
 *   6 (EOR)   : val1 ^ val2 (bitwise XOR), updates N, Z flags
 *   8 (SLC)   : val1 rotated left by val2 bits (wrap-around), updates N, Z flags
 *   9 (SRC)   : val1 rotated right by val2 bits (wrap-around), updates N, Z flags
 *
 * Other opcodes (3, 4, 7, 10, 11) are not ALU operations and return 0.
 */
int8_t execute(int opcode, int8_t val1, int8_t val2);

/* ---------------------------------------------------------
 *  BRANCH ADDRESS CALCULATION
 * --------------------------------------------------------- */

/**
 * calculateBranch() - Calculate target address for branch instructions
 *
 * @currentPC : Current program counter value (16-bit)
 * @immediate : Sign-extended 6-bit immediate value
 *
 * Returns   : New PC value = currentPC + 1 + immediate
 *
 * Used for:
 *   4 (BEQZ) : Branch if equal to zero (if result is zero from previous operation)
 *   7 (BR)   : Unconditional branch
 *
 * Note: The formula is PC_new = PC + 1 + IMM, where:
 *       - PC + 1 accounts for normal sequential execution
 *       - + IMM applies the branch offset
 */
uint16_t calculateBranch(uint16_t currentPC, int8_t immediate);

/* ---------------------------------------------------------
 *  INITIALIZATION & TESTING
 * --------------------------------------------------------- */

/**
 * testALU() - Comprehensive unit tests for all ALU operations
 *
 * Tests cover:
 *   1. Arithmetic operations (ADD, SUB, MUL)
 *   2. Logical operations (ANDI, EOR)
 *   3. Shift operations (SLC, SRC)
 *   4. Branch address calculation
 *   5. Flag updates verification
 *   6. Edge cases and overflow conditions
 *
 * Returns: 0 if all tests pass, non-zero otherwise
 */
int testALU(void);

#endif /* ALU_H */
