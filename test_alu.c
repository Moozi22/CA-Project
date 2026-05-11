/* =========================================================
 *  test_alu.c  -  Unit tests for ALU & Execution Logic
 *
 *  Compile:
 *    gcc -Wall -o test_alu test_alu.c alu.c registers.c -lm
 *  Run:
 *    ./test_alu
 *
 *  This test file independently exercises all ALU functions
 *  and verifies correct operation of all instructions.
 * ========================================================= */

#include "alu.h"
#include "registers.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    printf("========================================================\n");
    printf("  ALU & EXECUTION LOGIC TEST SUITE\n");
    printf("  Member 4: Arithmetic Logic Unit Implementation\n");
    printf("========================================================\n\n");
    
    printf("Running comprehensive ALU tests...\n\n");
    
    /* Run the full ALU test suite */
    int result = testALU();
    
    printf("\n========================================================\n");
    if (result == 0) {
        printf("  STATUS: All ALU tests completed successfully!\n");
        printf("  The ALU is ready for integration into the pipeline.\n");
    } else {
        printf("  STATUS: Some tests failed. Review output above.\n");
    }
    printf("========================================================\n");
    
    return result;
}
