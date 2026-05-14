#ifndef PIPELINE_H
#define PIPELINE_H

#include <stdint.h>
#include "memory.h"
#include "registers.h"
#include "decoder.h"
#include "alu.h"

/* =========================================================
 *  PACKAGE 3 - Member 5: Pipeline Controller
 *  pipeline.h  -  Public interface for the 3-stage pipeline
 * =========================================================
 *
 *  Architecture: Harvard, 3-stage pipeline (IF -> ID -> EX)
 *  Up to 3 instructions run in parallel each clock cycle.
 *  Clock cycle formula: 3 + ((n-1) * 1)
 *
 *  Pipeline registers hold the state passing between stages.
 *  'valid' flag = 0 means the slot is empty (bubble/flush).
 * ========================================================= */

/* ---------------------------------------------------------
 *  PIPELINE INTER-STAGE REGISTERS
 * --------------------------------------------------------- */

/* Holds what the IF stage produced, to be consumed by ID next cycle */
typedef struct {
    uint16_t instruction;   /* raw 16-bit binary fetched from instruction memory */
    uint16_t pc;            /* PC of this instruction (stored for BEQZ branch calc) */
    int      valid;         /* 1 = real instruction, 0 = bubble (empty slot) */
} IF_ID_Register;

/* Holds what the ID stage produced, to be consumed by EX next cycle */
typedef struct {
    Instruction decoded;    /* fully decoded instruction (opcode, r1, r2, imm, type) */
    int8_t      val1;       /* value read from register r1 */
    int8_t      val2;       /* value read from register r2 (or sign-extended imm) */
    uint16_t    pc;         /* PC of this instruction (passed through for branching) */
    int         valid;      /* 1 = real instruction, 0 = bubble */
} ID_EX_Register;

/* ---------------------------------------------------------
 *  PIPELINE STATE
 * --------------------------------------------------------- */

/* The two inter-stage registers - accessible by logger (Member 6) */
extern IF_ID_Register IF_ID;
extern ID_EX_Register ID_EX;

/* Current clock cycle number */
extern int clockCycle;

/* Total number of instructions loaded from the program */
extern int totalInstructions;

/* Count of instructions that have completed (entered EX) */
extern int completedInstructions;

/* ---------------------------------------------------------
 *  MAIN PIPELINE INTERFACE
 * --------------------------------------------------------- */

/* Initialize the pipeline: zero out registers, set cycle to 1.
 * Call this AFTER initMemory(), initRegisters(), and loadProgram(). */
void initPipeline(int numInstructions);

/* Run the entire simulation until all instructions complete.
 * Calls runCycle() in a loop. Prints final state at the end. */
void runPipeline(void);

/* Execute one clock cycle: advance all stages simultaneously.
 * Order: EX first (uses current ID_EX), then ID (uses current IF_ID),
 * then IF (fetches next). This avoids using updated values in same cycle. */
void runCycle(void);

/* ---------------------------------------------------------
 *  INDIVIDUAL STAGE FUNCTIONS
 * --------------------------------------------------------- */

/* IF Stage: fetch instruction at current PC, increment PC, fill IF_ID. */
void stageFetch(void);

/* ID Stage: decode IF_ID instruction, read registers, fill ID_EX. */
void stageDecode(void);

/* EX Stage: execute ID_EX instruction (ALU / memory / writeback). */
void stageExecute(void);

/* ---------------------------------------------------------
 *  HAZARD & CONTROL FLOW
 * --------------------------------------------------------- */

/* Flush the pipeline: clear IF_ID and ID_EX (set valid=0).
 * Called when a branch/jump is taken during EX stage. */
void flushPipeline(void);

/* Check for RAW data hazards between pipeline stages.
 * Returns 1 if a hazard exists (stall needed), 0 otherwise.
 * (Forwarding is handled inside stageExecute.) */
int detectHazard(void);

#endif /* PIPELINE_H */