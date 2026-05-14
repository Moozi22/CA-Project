#ifndef PIPELINE_H
#define PIPELINE_H

#include <stdint.h>
#include "memory.h"
#include "registers.h"
#include "decoder.h"
#include "alu.h"

typedef struct
{
    uint16_t instruction;
    uint16_t pc;
    int valid;
} IF_ID_Register;

typedef struct
{
    Instruction decoded;
    int8_t val1;
    int8_t val2;
    uint16_t pc;
    int valid;
} ID_EX_Register;

typedef struct
{
    int detected;
    int canForward;
    int requiresStall;
    int sourceRegister;
    int consumerRegister;
} HazardUnit;

typedef struct
{
    int cycle;
} Logger;

extern IF_ID_Register IF_ID;
extern ID_EX_Register ID_EX;

extern int clockCycle;
extern int totalInstructions;
extern int completedInstructions;

void initPipeline(int numInstructions);
void runPipeline(void);
void runCycle(void);
void stageFetch(void);
void stageDecode(void);
void stageExecute(void);
void flushPipeline(void);
void logState(void);
int detectHazards(void);
int detectHazard(void);

#endif