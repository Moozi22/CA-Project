/* =========================================================
 *  PACKAGE 3 - CSEN601 CPU Simulator
 *  main.c  -  Entry point
 * =========================================================
 *
 *  Compile:
 *    gcc -Wall -o simulator main.c memory.c registers.c decoder.c alu.c pipeline.c
 *
 *  Run:
 *    ./simulator program.txt
 *
 *  Where program.txt is your assembly file, e.g.:
 *    MOVI R1 5
 *    MOVI R2 3
 *    ADD R1 R2
 *    STR R1 100
 * ========================================================= */

#include <stdio.h>
#include "memory.h"
#include "registers.h"
#include "pipeline.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <assembly_file.txt>\n", argv[0]);
        printf("Example: %s program.txt\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  CSEN601 Package 3 - Harvard CPU Simulator           ║\n");
    printf("║  3-Stage Pipeline: IF -> ID -> EX                    ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* Step 1: Initialize all hardware */
    initMemory();
    initRegisters();

    /* Step 2: Load and parse the assembly program */
    int numInstructions = loadProgram(filename);
    if (numInstructions <= 0) {
        printf("ERROR: No instructions loaded. Exiting.\n");
        return 1;
    }

    /* Step 3: Initialize the pipeline */
    initPipeline(numInstructions);

    /* Step 4: Run the simulation */
    runPipeline();

    return 0;
}