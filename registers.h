#ifndef REGISTERS_H
#define REGISTERS_H



#include <stdint.h>

extern int8_t GPR[64];

extern uint8_t SREG;

#define SREG_Z_BIT  0   
#define SREG_S_BIT  1  
#define SREG_N_BIT  2   
#define SREG_V_BIT  3   
#define SREG_C_BIT  4   

/* Convenience macros to read individual flags from SREG */
#define FLAG_Z  ((SREG >> SREG_Z_BIT) & 1)
#define FLAG_S  ((SREG >> SREG_S_BIT) & 1)
#define FLAG_N  ((SREG >> SREG_N_BIT) & 1)
#define FLAG_V  ((SREG >> SREG_V_BIT) & 1)
#define FLAG_C  ((SREG >> SREG_C_BIT) & 1)

/* ---------------------------------------------------------
 *  INITIALISATION
 * --------------------------------------------------------- */

/* Zero-fill all 64 GPRs and reset SREG to 0x00. */
void initRegisters(void);

/* ---------------------------------------------------------
 *  GENERAL-PURPOSE REGISTER ACCESS
 * --------------------------------------------------------- */

/* Read the 8-bit signed value stored in register number regNum (0–63).
 * Returns 0 on out-of-range access.
 * Note: R0 through R63 are ALL valid GPRs in Package 3
 *       (unlike Packages 1/2, Package 3 has no "zero register"). */
int8_t getRegister(uint8_t regNum);

/* Write value to register regNum (0–63).
 * Prints a log line automatically (spec requirement).
 * stageName is the pipeline stage doing the write, e.g. "EX".  */
void setRegister(uint8_t regNum, int8_t value, const char *stageName);

/* ---------------------------------------------------------
 *  STATUS REGISTER (SREG) FLAG UPDATES
 *
 *  The spec specifies exactly which instructions update which flags:
 *
 *  C (Carry)    : ADD only
 *  V (Overflow) : ADD, SUB
 *  N (Negative) : ADD, SUB, MUL, ANDI, EOR, SLC, SRC
 *  S (Sign)     : ADD, SUB        (S = N XOR V)
 *  Z (Zero)     : ADD, SUB, MUL, ANDI, EOR, SLC, SRC
 *
 *  "A flag value can only be updated by the instructions related to it."
 *  Callers pass only the flags relevant to their opcode.
 * --------------------------------------------------------- */

/* Update the Carry flag (C) only.
 * val1, val2 : the two unsigned operands (passed as int for width).
 * result     : the 8-bit truncated result.
 * Called by: ADD                                              */
void updateCarryFlag(int val1, int val2);

/* Update the Overflow flag (V) only.
 * val1, val2 : signed operands; isSubtraction = 1 for SUB, 0 for ADD.
 * result8    : the 8-bit signed result after truncation.
 * Called by: ADD, SUB                                         */
void updateOverflowFlag(int8_t val1, int8_t val2, int8_t result8,
                        int isSubtraction);

/* Update the Negative flag (N) only.
 * result8 : the 8-bit signed result.
 * Called by: ADD, SUB, MUL, ANDI, EOR, SLC, SRC              */
void updateNegativeFlag(int8_t result8);

/* Update the Sign flag (S) only.
 * S = N XOR V — must be called AFTER N and V are already updated.
 * Called by: ADD, SUB                                         */
void updateSignFlag(void);

/* Update the Zero flag (Z) only.
 * result8 : the 8-bit result.
 * Called by: ADD, SUB, MUL, ANDI, EOR, SLC, SRC              */
void updateZeroFlag(int8_t result8);

/* ---------------------------------------------------------
 *  CONVENIENCE FLAG UPDATERS (groups for common instruction types)
 *
 *  These bundle the flag calls required for each instruction
 *  and are the primary interface the ALU (Member 4) will use.
 * --------------------------------------------------------- */

/* Called after ADD R1 R2.
 * Updates: C, V, N, S, Z
 * val1, val2 : original signed 8-bit operands (widened to int for carry). */
void updateFlagsADD(int8_t val1, int8_t val2, int8_t result8);

/* Called after SUB R1 R2.
 * Updates: V, N, S, Z  (no Carry update for SUB) */
void updateFlagsSUB(int8_t val1, int8_t val2, int8_t result8);

/* Called after MUL, ANDI, EOR, SLC, SRC.
 * Updates: N, Z only */
void updateFlagsNZ(int8_t result8);

/* ---------------------------------------------------------
 *  END-OF-SIMULATION PRINTING  (spec requires full dump)
 * --------------------------------------------------------- */

/* Print all 64 GPR values (address + decimal + binary). */
void printRegisters(void);

/* Print SREG value with individual flag breakdown. */
void printSREG(void);

#endif /* REGISTERS_H */
