/* =========================================================
 *  PACKAGE 3 - Member 5: Pipeline Controller
 *  pipeline.c  -  3-stage pipeline implementation
 * =========================================================
 *
 *  Stages:
 *    IF  (Instruction Fetch)   - reads from instruction memory
 *    ID  (Instruction Decode)  - decodes binary, reads registers
 *    EX  (Execute)             - ALU + memory access + writeback
 *
 *  Key spec rules implemented here:
 *    - All instructions pass through all 3 stages (no shortcuts)
 *    - Clock formula: 3 + ((n-1) * 1)
 *    - Branch/jump: PC updated in EX, ID and IF cancelled that cycle
 *    - Data hazards: forwarding from EX result back to ID reads
 *    - Stop condition: no more instructions to fetch AND pipeline empty
 *    - Print mandatory cycle-by-cycle report each cycle
 * ========================================================= */

#include "pipeline.h"
#include <stdio.h>
#include <string.h>

/* =========================================================
 *  GLOBAL PIPELINE STATE
 * ========================================================= */

IF_ID_Register IF_ID;
ID_EX_Register ID_EX;

int clockCycle            = 1;
int totalInstructions     = 0;
int completedInstructions = 0;

/* Forwarding: value produced by EX, available for ID next cycle */
static int8_t  forwardValue = 0;
static int     forwardReg   = -1;
static int     forwardValid = 0;

/* Branch flag: set by flushPipeline(), read by runCycle() */
static int branchTaken = 0;

/* Tracks how many instructions have been fetched */
static int fetchedCount = 0;

/* =========================================================
 *  HELPER: print a 16-bit value as binary string
 * ========================================================= */
static void printBin16(uint16_t val)
{
    for (int b = 15; b >= 0; b--)
        printf("%d", (val >> b) & 1);
}

/* =========================================================
 *  SECTION 1 - INITIALISATION
 * ========================================================= */

void initPipeline(int numInstructions)
{
    totalInstructions     = numInstructions;
    completedInstructions = 0;
    fetchedCount          = 0;
    clockCycle            = 1;
    forwardValid          = 0;
    forwardReg            = -1;
    forwardValue          = 0;
    branchTaken           = 0;

    memset(&IF_ID, 0, sizeof(IF_ID));
    memset(&ID_EX, 0, sizeof(ID_EX));
    IF_ID.valid = 0;
    ID_EX.valid = 0;

    printf("[Pipeline] Initialised. %d instruction(s) to execute.\n",
           totalInstructions);
    printf("[Pipeline] Starting simulation at cycle 1.\n\n");
}

/* =========================================================
 *  SECTION 2 - STAGE: INSTRUCTION FETCH (IF)
 * ========================================================= */

void stageFetch(void)
{
    if (fetchedCount >= totalInstructions) {
        IF_ID.valid = 0;
        printf("  [IF] No instruction to fetch (all %d fetched).\n",
               totalInstructions);
        return;
    }

    uint16_t fetchedInstruction = readInstruction(PC);
    uint16_t fetchedPC          = PC;

    printf("  [IF] Fetching instruction at PC=%u -> 0b", fetchedPC);
    printBin16(fetchedInstruction);
    printf(" (0x%04X)\n", fetchedInstruction);

    IF_ID.instruction = fetchedInstruction;
    IF_ID.pc          = fetchedPC;
    IF_ID.valid       = 1;

    PC++;
    fetchedCount++;

    printf("  [IF] PC incremented to %u.\n", PC);
}

/* =========================================================
 *  SECTION 3 - STAGE: INSTRUCTION DECODE (ID)
 * ========================================================= */

void stageDecode(void)
{
    if (!IF_ID.valid) {
        ID_EX.valid = 0;
        printf("  [ID] No instruction in latch (bubble).\n");
        return;
    }

    Instruction decoded = decode(IF_ID.instruction);

    printf("  [ID] Decoding: %s", decoded.mnemonic);

    int8_t val1 = getRegister((uint8_t)decoded.r1);
    int8_t val2 = 0;

    if (decoded.type == 'R') {
        val2 = getRegister((uint8_t)decoded.r2);
        printf(" R%d R%d  =>  val1=%d, val2=%d\n",
               decoded.r1, decoded.r2, val1, val2);
    } else {
        val2 = (int8_t)decoded.immediate;
        printf(" R%d %d  =>  val1=%d, imm=%d\n",
               decoded.r1, decoded.immediate, val1, val2);
    }

    /* ---- DATA HAZARD: Forwarding ---- */
    if (forwardValid) {
        if (decoded.r1 == forwardReg) {
            printf("  [ID] FORWARD: R%d val1 updated from %d -> %d "
                   "(forwarded from EX)\n",
                   forwardReg, val1, forwardValue);
            val1 = forwardValue;
        }
        if (decoded.type == 'R' && decoded.r2 == forwardReg) {
            printf("  [ID] FORWARD: R%d val2 updated from %d -> %d "
                   "(forwarded from EX)\n",
                   forwardReg, val2, forwardValue);
            val2 = forwardValue;
        }
    }

    ID_EX.decoded = decoded;
    ID_EX.val1    = val1;
    ID_EX.val2    = val2;
    ID_EX.pc      = IF_ID.pc;
    ID_EX.valid   = 1;
}

/* =========================================================
 *  SECTION 4 - STAGE: EXECUTE (EX)
 * ========================================================= */

void stageExecute(void)
{
    /* Reset forwarding from previous cycle */
    forwardValid = 0;
    forwardReg   = -1;

    if (!ID_EX.valid) {
        printf("  [EX] No instruction to execute (bubble).\n");
        return;
    }

    Instruction inst = ID_EX.decoded;
    int8_t   val1    = ID_EX.val1;
    int8_t   val2    = ID_EX.val2;
    uint16_t instPC  = ID_EX.pc;

    printf("  [EX] Executing: %s", inst.mnemonic);
    if (inst.type == 'R')
        printf(" R%d R%d  (val1=%d, val2=%d)\n",
               inst.r1, inst.r2, val1, val2);
    else
        printf(" R%d %d   (val1=%d, imm=%d)\n",
               inst.r1, inst.immediate, val1, val2);

    switch (inst.opcode) {

        /* ---- 0: ADD R1 R2   R1 = R1 + R2 ---- */
        case 0: {
            int8_t result = execute(0, val1, val2);
            setRegister((uint8_t)inst.r1, result, "EX");
            forwardValue = result;
            forwardReg   = inst.r1;
            forwardValid = 1;
            break;
        }

        /* ---- 1: SUB R1 R2   R1 = R1 - R2 ---- */
        case 1: {
            int8_t result = execute(1, val1, val2);
            setRegister((uint8_t)inst.r1, result, "EX");
            forwardValue = result;
            forwardReg   = inst.r1;
            forwardValid = 1;
            break;
        }

        /* ---- 2: MUL R1 R2   R1 = R1 * R2 ---- */
        case 2: {
            int8_t result = execute(2, val1, val2);
            setRegister((uint8_t)inst.r1, result, "EX");
            forwardValue = result;
            forwardReg   = inst.r1;
            forwardValid = 1;
            break;
        }

        /* ---- 3: MOVI R1 IMM   R1 = IMM ---- */
        case 3: {
            setRegister((uint8_t)inst.r1, val2, "EX");
            forwardValue = val2;
            forwardReg   = inst.r1;
            forwardValid = 1;
            break;
        }

        /* ---- 4: BEQZ R1 IMM   if(R1==0) PC = PC+1+IMM ---- */
        case 4: {
            printf("  [EX] BEQZ: checking R%d == 0? val1=%d\n",
                   inst.r1, val1);
            if (val1 == 0) {
                uint16_t target = calculateBranch(instPC,
                                                  (int8_t)inst.immediate);
                printf("  [EX] BEQZ TAKEN: PC set to %u "
                       "(was %u+1+%d)\n",
                       target, instPC, inst.immediate);
                PC = target;
                flushPipeline();
            } else {
                printf("  [EX] BEQZ NOT TAKEN: continuing sequentially.\n");
            }
            break;
        }

        /* ---- 5: ANDI R1 IMM   R1 = R1 & IMM ---- */
        case 5: {
            int8_t result = execute(5, val1, val2);
            setRegister((uint8_t)inst.r1, result, "EX");
            forwardValue = result;
            forwardReg   = inst.r1;
            forwardValid = 1;
            break;
        }

        /* ---- 6: EOR R1 R2   R1 = R1 XOR R2 ---- */
        case 6: {
            int8_t result = execute(6, val1, val2);
            setRegister((uint8_t)inst.r1, result, "EX");
            forwardValue = result;
            forwardReg   = inst.r1;
            forwardValid = 1;
            break;
        }

        /* ---- 7: BR R1 R2   PC = R1 || R2 ---- */
        case 7: {
            uint8_t  highByte = (uint8_t)val1;
            uint8_t  lowByte  = (uint8_t)val2;
            uint16_t target   = ((uint16_t)highByte << 8) |
                                  (uint16_t)lowByte;
            printf("  [EX] BR: PC set to %u "
                   "(R%d=0x%02X || R%d=0x%02X)\n",
                   target, inst.r1, highByte, inst.r2, lowByte);
            PC = target;
            flushPipeline();
            break;
        }

        /* ---- 8: SLC R1 IMM ---- */
        case 8: {
            int8_t result = execute(8, val1, val2);
            setRegister((uint8_t)inst.r1, result, "EX");
            forwardValue = result;
            forwardReg   = inst.r1;
            forwardValid = 1;
            break;
        }

        /* ---- 9: SRC R1 IMM ---- */
        case 9: {
            int8_t result = execute(9, val1, val2);
            setRegister((uint8_t)inst.r1, result, "EX");
            forwardValue = result;
            forwardReg   = inst.r1;
            forwardValid = 1;
            break;
        }

        /* ---- 10: LDR R1 ADDRESS   R1 = MEM[ADDRESS] ---- */
        case 10: {
            uint16_t addr   = (uint16_t)(uint8_t)val2;
            int8_t   loaded = readData(addr);
            printf("  [EX] LDR: R%d = MEM[%u] = %d\n",
                   inst.r1, addr, loaded);
            setRegister((uint8_t)inst.r1, loaded, "EX");
            forwardValue = loaded;
            forwardReg   = inst.r1;
            forwardValid = 1;
            break;
        }

        /* ---- 11: STR R1 ADDRESS   MEM[ADDRESS] = R1 ---- */
        case 11: {
            uint16_t addr = (uint16_t)(uint8_t)val2;
            printf("  [EX] STR: MEM[%u] = R%d = %d\n",
                   addr, inst.r1, val1);
            writeData(addr, val1);
            break;
        }

        default:
            printf("  [EX] ERROR: Unknown opcode %d\n", inst.opcode);
            break;
    }

    completedInstructions++;
}

/* =========================================================
 *  SECTION 5 - HAZARD DETECTION
 * ========================================================= */

int detectHazard(void)
{
    if (!forwardValid || !IF_ID.valid)
        return 0;

    Instruction next = decode(IF_ID.instruction);

    if (next.r1 == forwardReg)
        return 1;
    if (next.type == 'R' && next.r2 == forwardReg)
        return 1;

    return 0;
}

/* =========================================================
 *  SECTION 6 - PIPELINE FLUSH
 * ========================================================= */

void flushPipeline(void)
{
    printf("  [EX] FLUSH: Dropping instructions in IF and ID stages.\n");

    IF_ID.valid  = 0;
    ID_EX.valid  = 0;
    fetchedCount = PC;   /* resume fetching from branch target next cycle */
    forwardValid = 0;
    forwardReg   = -1;
    branchTaken  = 1;    /* tell runCycle to cancel ID and IF this cycle */
}

/* =========================================================
 *  SECTION 7 - SINGLE CYCLE EXECUTION
 * ========================================================= */

void runCycle(void)
{
    printf("\n");
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  CLOCK CYCLE %-3d                                     ║\n",
           clockCycle);
    printf("╚══════════════════════════════════════════════════════╝\n");

    /* Save IF_ID latch before any stage runs */
    IF_ID_Register saved_IF_ID = IF_ID;

    /* Reset branch flag before EX runs */
    branchTaken = 0;

    /* --- Stage EX: runs on current ID_EX latch --- */
    stageExecute();

    if (branchTaken) {
        /*
         * A branch was taken inside EX this cycle.
         * ID and IF must NOT run — their inputs are stale.
         * flushPipeline() already cleared both latches.
         * Next cycle IF will fetch from the new PC.
         */
        printf("  [ID] Cancelled — branch taken this cycle.\n");
        printf("  [IF] Cancelled — branch taken this cycle.\n");
    } else {
        /*
         * Normal cycle — restore IF_ID so ID reads the state
         * from the START of this cycle, then IF fetches next.
         */
        IF_ID = saved_IF_ID;
        stageDecode();
        stageFetch();
    }

    clockCycle++;
}

/* =========================================================
 *  SECTION 8 - FULL SIMULATION LOOP
 * ========================================================= */

void runPipeline(void)
{
    printf("\n");
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  PIPELINE SIMULATION START                           ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");

    while (1) {
        runCycle();

        int fetchDone     = (fetchedCount >= totalInstructions);
        int pipelineEmpty = (!IF_ID.valid && !ID_EX.valid);

        if (fetchDone && pipelineEmpty)
            break;
    }

    /* End of simulation mandatory prints */
    printf("\n");
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  SIMULATION COMPLETE                                 ║\n");
    printf("║  Total clock cycles: %-3d                             ║\n",
           clockCycle - 1);
    printf("║  Instructions completed: %-3d                         ║\n",
           completedInstructions);
    printf("╚══════════════════════════════════════════════════════╝\n");

    printRegisters();
    printSREG();

    printf("\n  PC (final) = %u\n", PC);

    printInstructionMemory();
    printDataMemory();
}