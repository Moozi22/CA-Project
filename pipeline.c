#include "pipeline.h"
#include <stdio.h>
#include <string.h>

IF_ID_Register IF_ID;
ID_EX_Register ID_EX;

int clockCycle            = 1;
int totalInstructions     = 0;
int completedInstructions = 0;

static int8_t  forwardValue = 0;
static int     forwardReg   = -1;
static int     forwardValid = 0;
static int     branchTaken  = 0;
static int     fetchedCount = 0;

static void printBin16(uint16_t val)
{
    for (int b = 15; b >= 0; b--)
        printf("%d", (val >> b) & 1);
}

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

    printf("[Pipeline] Initialised. %d instruction(s) to execute.\n", totalInstructions);
    printf("[Pipeline] Starting simulation at cycle 1.\n\n");
}

void stageFetch(void)
{
    if (fetchedCount >= totalInstructions) {
        IF_ID.valid = 0;
        printf("  [IF] No instruction to fetch (all %d fetched).\n", totalInstructions);
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
        printf(" R%d R%d  =>  val1=%d, val2=%d\n", decoded.r1, decoded.r2, val1, val2);
    } else {
        val2 = (int8_t)decoded.immediate;
        printf(" R%d %d  =>  val1=%d, imm=%d\n", decoded.r1, decoded.immediate, val1, val2);
    }

    if (forwardValid) {
        if (decoded.r1 == forwardReg) {
            printf("  [ID] FORWARD: R%d val1 updated from %d -> %d (forwarded from EX)\n",
                   forwardReg, val1, forwardValue);
            val1 = forwardValue;
        }
        if (decoded.type == 'R' && decoded.r2 == forwardReg) {
            printf("  [ID] FORWARD: R%d val2 updated from %d -> %d (forwarded from EX)\n",
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

void stageExecute(void)
{
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
        printf(" R%d R%d  (val1=%d, val2=%d)\n", inst.r1, inst.r2, val1, val2);
    else
        printf(" R%d %d   (val1=%d, imm=%d)\n", inst.r1, inst.immediate, val1, val2);

    switch (inst.opcode) {
        case 0: {
            int8_t result = execute(0, val1, val2);
            setRegister((uint8_t)inst.r1, result, "EX");
            forwardValue = result; forwardReg = inst.r1; forwardValid = 1;
            break;
        }
        case 1: {
            int8_t result = execute(1, val1, val2);
            setRegister((uint8_t)inst.r1, result, "EX");
            forwardValue = result; forwardReg = inst.r1; forwardValid = 1;
            break;
        }
        case 2: {
            int8_t result = execute(2, val1, val2);
            setRegister((uint8_t)inst.r1, result, "EX");
            forwardValue = result; forwardReg = inst.r1; forwardValid = 1;
            break;
        }
        case 3: {
            setRegister((uint8_t)inst.r1, val2, "EX");
            forwardValue = val2; forwardReg = inst.r1; forwardValid = 1;
            break;
        }
        case 4: {
            printf("  [EX] BEQZ: checking R%d == 0? val1=%d\n", inst.r1, val1);
            if (val1 == 0) {
                uint16_t target = calculateBranch(instPC, (int8_t)inst.immediate);
                printf("  [EX] BEQZ TAKEN: PC set to %u (was %u+1+%d)\n",
                       target, instPC, inst.immediate);
                PC = target;
                flushPipeline();
            } else {
                printf("  [EX] BEQZ NOT TAKEN: continuing sequentially.\n");
            }
            break;
        }
        case 5: {
            int8_t result = execute(5, val1, val2);
            setRegister((uint8_t)inst.r1, result, "EX");
            forwardValue = result; forwardReg = inst.r1; forwardValid = 1;
            break;
        }
        case 6: {
            int8_t result = execute(6, val1, val2);
            setRegister((uint8_t)inst.r1, result, "EX");
            forwardValue = result; forwardReg = inst.r1; forwardValid = 1;
            break;
        }
        case 7: {
            uint8_t  highByte = (uint8_t)val1;
            uint8_t  lowByte  = (uint8_t)val2;
            uint16_t target   = ((uint16_t)highByte << 8) | (uint16_t)lowByte;
            printf("  [EX] BR: PC set to %u (R%d=0x%02X || R%d=0x%02X)\n",
                   target, inst.r1, highByte, inst.r2, lowByte);
            PC = target;
            flushPipeline();
            break;
        }
        case 8: {
            int8_t result = execute(8, val1, val2);
            setRegister((uint8_t)inst.r1, result, "EX");
            forwardValue = result; forwardReg = inst.r1; forwardValid = 1;
            break;
        }
        case 9: {
            int8_t result = execute(9, val1, val2);
            setRegister((uint8_t)inst.r1, result, "EX");
            forwardValue = result; forwardReg = inst.r1; forwardValid = 1;
            break;
        }
        case 10: {
            uint16_t addr   = (uint16_t)(uint8_t)val2;
            int8_t   loaded = readData(addr);
            printf("  [EX] LDR: R%d = MEM[%u] = %d\n", inst.r1, addr, loaded);
            setRegister((uint8_t)inst.r1, loaded, "EX");
            forwardValue = loaded; forwardReg = inst.r1; forwardValid = 1;
            break;
        }
        case 11: {
            uint16_t addr = (uint16_t)(uint8_t)val2;
            printf("  [EX] STR: MEM[%u] = R%d = %d\n", addr, inst.r1, val1);
            writeData(addr, val1);
            break;
        }
        default:
            printf("  [EX] ERROR: Unknown opcode %d\n", inst.opcode);
            break;
    }

    completedInstructions++;
}

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

void flushPipeline(void)
{
    printf("  [EX] FLUSH: Dropping instructions in IF and ID stages.\n");
    IF_ID.valid  = 0;
    ID_EX.valid  = 0;
    fetchedCount = PC;
    forwardValid = 0;
    forwardReg   = -1;
    branchTaken  = 1;
}

void runCycle(void)
{
    printf("\n");
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  CLOCK CYCLE %-3d                                     ║\n", clockCycle);
    printf("╚══════════════════════════════════════════════════════╝\n");

    IF_ID_Register saved_IF_ID = IF_ID;
    branchTaken = 0;

    stageExecute();

    if (branchTaken) {
        printf("  [ID] Cancelled — branch taken this cycle.\n");
        printf("  [IF] Cancelled — branch taken this cycle.\n");
    } else {
        IF_ID = saved_IF_ID;
        stageDecode();
        stageFetch();
    }

    clockCycle++;
}

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

    printf("\n");
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  SIMULATION COMPLETE                                 ║\n");
    printf("║  Total clock cycles: %-3d                             ║\n", clockCycle - 1);
    printf("║  Instructions completed: %-3d                         ║\n", completedInstructions);
    printf("╚══════════════════════════════════════════════════════╝\n");

    printRegisters();
    printSREG();

    printf("\n  PC (final) = %u\n", PC);

    printInstructionMemory();
    printDataMemory();
}