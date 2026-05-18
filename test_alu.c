

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
