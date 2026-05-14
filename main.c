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

    initMemory();
    initRegisters();

    int numInstructions = loadProgram(filename);
    if (numInstructions <= 0) {
        printf("ERROR: No instructions loaded. Exiting.\n");
        return 1;
    }

    initPipeline(numInstructions);
    runPipeline();

    return 0;
}