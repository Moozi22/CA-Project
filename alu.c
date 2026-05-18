#include "alu.h"
#include "registers.h"
#include <stdio.h>
#include <string.h>

int8_t execute(int opcode, int8_t val1, int8_t val2)
{
    int8_t result = 0;

    switch (opcode)
    {
    case 0: /* ADD*/
    {
        int sum = (int)val1 + (int)val2;
        result = (int8_t)sum;
        updateFlagsADD(val1, val2, result);
        break;
    }

    case 1: /* SUB */
    {
        int diff = (int)val1 - (int)val2;
        result = (int8_t)diff;
        updateFlagsSUB(val1, val2, result);
        break;
    }

    case 2: /* MUL */
    {
        int product = (int)val1 * (int)val2;
        result = (int8_t)product;
        updateFlagsNZ(result);
        break;
    }

    case 5: /* ANDI*/
    {
        result = val1 & val2;
        updateFlagsNZ(result);
        break;
    }

    case 6: /* EOR*/
    {
        result = val1 ^ val2;
        updateFlagsNZ(result);
        break;
    }

    case 8: /* SLC */
    {
        uint8_t val_unsigned = (uint8_t)val1;
        uint8_t rotate_count = ((uint8_t)val2) & 7;

        if (rotate_count == 0)
        {
            result = val1;
        }
        else
        {
            result = (int8_t)((val_unsigned << rotate_count) |
                              (val_unsigned >> (8 - rotate_count)));
        }
        updateFlagsNZ(result);
        break;
    }

    case 9: /* SRC*/
    {
        uint8_t val_unsigned = (uint8_t)val1;
        uint8_t rotate_count = ((uint8_t)val2) & 7;

        if (rotate_count == 0)
        {
            result = val1;
        }
        else
        {
            result = (int8_t)((val_unsigned >> rotate_count) |
                              (val_unsigned << (8 - rotate_count)));
        }
        updateFlagsNZ(result);
        break;
    }

    case 3:  /* MOVI*/
    case 4:  /* BEQZ*/
    case 7:  /* BR */
    case 10: /* LDR*/
    case 11: /* STR */
    default:
        
        result = 0;
        break;
    }

    return result;
}


uint16_t calculateBranch(uint16_t currentPC, int8_t immediate)
{
   
    return currentPC + 1 + immediate;
}



static int test_count = 0;
static int test_pass = 0;

#define TEST_ASSERT(cond, msg)           \
    do                                   \
    {                                    \
        test_count++;                    \
        if (cond)                        \
        {                                \
            test_pass++;                 \
            printf("  PASS: %s\n", msg); \
        }                                \
        else                             \
        {                                \
            printf("  FAIL: %s\n", msg); \
        }                                \
    } while (0)



int testALU(void)
{
    printf("\n========================================================\n");
    printf("  ALU & EXECUTION LOGIC UNIT TESTS (Member 4)\n");
    printf("========================================================\n");

    
    test_count = 0;
    test_pass = 0;
    initRegisters();

    
    printf("\n--- Test Group 1: Arithmetic Operations ---\n");

    {
        int8_t a = 5, b = 3;
        int8_t result = execute(0, a, b);
        TEST_ASSERT(result == 8, "ADD 5 + 3 = 8");
    }

   
    {
        initRegisters();
        int8_t a = -5, b = -3;
        int8_t result = execute(0, a, b);
        TEST_ASSERT(result == -8, "ADD -5 + (-3) = -8");
    }

   
    {
        initRegisters();
        int8_t a = 127, b = 1;
        int8_t result = execute(0, a, b);
        TEST_ASSERT(result == -128, "ADD 127 + 1 = -128 (overflow)");
    }

    
    {
        initRegisters();
        int8_t a = -128, b = -1;
        int8_t result = execute(0, a, b);
        TEST_ASSERT(result == 127, "ADD -128 + (-1) = 127 (underflow)");
    }

    
    {
        initRegisters();
        int8_t a = 10, b = 3;
        int8_t result = execute(1, a, b);
        TEST_ASSERT(result == 7, "SUB 10 - 3 = 7");
    }

   
    {
        initRegisters();
        int8_t a = 3, b = 10;
        int8_t result = execute(1, a, b);
        TEST_ASSERT(result == -7, "SUB 3 - 10 = -7");
    }

  
    {
        initRegisters();
        int8_t a = 5, b = 3;
        int8_t result = execute(2, a, b);
        TEST_ASSERT(result == 15, "MUL 5 * 3 = 15");
    }

    
    {
        initRegisters();
        int8_t a = 127, b = 0;
        int8_t result = execute(2, a, b);
        TEST_ASSERT(result == 0, "MUL 127 * 0 = 0");
    }

    
    {
        initRegisters();
        int8_t a = -5, b = 3;
        int8_t result = execute(2, a, b);
        TEST_ASSERT(result == -15, "MUL -5 * 3 = -15");
    }

    
    {
        initRegisters();
        int8_t a = 20, b = 20;
        int8_t result = execute(2, a, b);

        int expected = (int8_t)(20 * 20);
        TEST_ASSERT(result == expected, "MUL 20 * 20 with overflow");
    }

   
    printf("\n--- Test Group 2: Logical Operations ---\n");

   
    {
        initRegisters();
        int8_t a = (int8_t)0b11110000, b = (int8_t)0b10101010;
        int8_t result = execute(5, a, b);
        TEST_ASSERT(result == (int8_t)0b10100000, "ANDI 0b11110000 & 0b10101010 = 0b10100000");
    }

   
    {
        initRegisters();
        int8_t a = (int8_t)0b11110000, b = (int8_t)0b00001111;
        int8_t result = execute(5, a, b);
        TEST_ASSERT(result == 0, "ANDI 0b11110000 & 0b00001111 = 0b00000000");
    }

   
    {
        initRegisters();
        int8_t a = (int8_t)0b11110000, b = (int8_t)0b10101010;
        int8_t result = execute(6, a, b);
        TEST_ASSERT(result == (int8_t)0b01011010, "EOR 0b11110000 ^ 0b10101010 = 0b01011010");
    }

    
    {
        initRegisters();
        int8_t a = (int8_t)0b11110000, b = (int8_t)0b11110000;
        int8_t result = execute(6, a, b);
        TEST_ASSERT(result == 0, "EOR 0b11110000 ^ 0b11110000 = 0b00000000");
    }

   
    printf("\n--- Test Group 3: Shift Operations (Circular) ---\n");

    
    {
        initRegisters();
        int8_t a = (int8_t)0b01010101; 
        int8_t result = execute(8, a, 0);
        
        TEST_ASSERT(result == (int8_t)0b10101010, "SLC 0b01010101 << 1 = 0b10101010");
    }

    
    {
        initRegisters();
        int8_t a = (int8_t)0b10000000; 
        int8_t result = execute(8, a, 0);
       
        TEST_ASSERT(result == (int8_t)0b00000001, "SLC 0b10000000 << 1 = 0b00000001");
    }

   
    {
        initRegisters();
        int8_t a = (int8_t)0b10101010; 
        int8_t result = execute(9, a, 0);
       
        TEST_ASSERT(result == (int8_t)0b01010101, "SRC 0b10101010 >> 1 = 0b01010101");
    }

   
    {
        initRegisters();
        int8_t a = (int8_t)0b00000001; 
        int8_t result = execute(9, a, 0);
       
        TEST_ASSERT(result == (int8_t)0b10000000, "SRC 0b00000001 >> 1 = 0b10000000");
    }

    
    printf("\n--- Test Group 4: Branch Address Calculation ---\n");

    
    {
        uint16_t pc = 10;
        int8_t imm = 5;
        uint16_t result = calculateBranch(pc, imm);
        TEST_ASSERT(result == 16, "calculateBranch(10, 5) = 16 (10+1+5)");
    }

  
    {
        uint16_t pc = 20;
        int8_t imm = -5;
        uint16_t result = calculateBranch(pc, imm);
        TEST_ASSERT(result == 16, "calculateBranch(20, -5) = 16 (20+1-5)");
    }

    
    {
        uint16_t pc = 0;
        int8_t imm = 10;
        uint16_t result = calculateBranch(pc, imm);
        TEST_ASSERT(result == 11, "calculateBranch(0, 10) = 11 (0+1+10)");
    }

   
    {
        uint16_t pc = 100;
        int8_t imm = 0;
        uint16_t result = calculateBranch(pc, imm);
        TEST_ASSERT(result == 101, "calculateBranch(100, 0) = 101 (100+1+0)");
    }

    
    {
        uint16_t pc = 500;
        int8_t imm = 127;
        uint16_t result = calculateBranch(pc, imm);
        TEST_ASSERT(result == 628, "calculateBranch(500, 127) = 628 (500+1+127)");
    }

    
    {
        uint16_t pc = 500;
        int8_t imm = -128;
        uint16_t result = calculateBranch(pc, imm);
        TEST_ASSERT(result == 373, "calculateBranch(500, -128) = 373 (500+1-128)");
    }

    
    printf("\n--- Test Group 5: Flag Updates Verification ---\n");

   
    {
        initRegisters();
        int8_t a = (int8_t)200, b = (int8_t)100; 
        execute(0, a, b);
      
        int carry_set = (SREG >> 4) & 1;
        TEST_ASSERT(carry_set, "ADD sets Carry flag on unsigned overflow");
    }

    
    {
        initRegisters();
        int8_t a = 5, b = 3;
        execute(0, a, b);
        int zero_clear = ((SREG >> 0) & 1) == 0;
        int carry_clear = ((SREG >> 4) & 1) == 0;
        TEST_ASSERT(zero_clear && carry_clear,
                    "ADD clears Z and C flags on normal positive result");
    }

    
    {
        initRegisters();
        int8_t a = (int8_t)0b11110000;
        int8_t b = (int8_t)0b00001111;
        int8_t result = execute(5, a, b); 
        int zero_set = (SREG >> 0) & 1;
        TEST_ASSERT(zero_set && result == 0, "ANDI sets Zero flag when result is 0");
    }

   
    {
        initRegisters();
        int8_t a = (int8_t)0b01000000;
        int8_t result = execute(8, a, 0); 
        int negative_set = (SREG >> 2) & 1;
        TEST_ASSERT(negative_set && result < 0,
                    "SLC sets Negative flag when MSB is 1");
    }

    
    printf("\n--- Test Group 6: Edge Cases & Special Cases ---\n");

   
    {
        initRegisters();
        int8_t result = execute(3, 10, 20); 
        TEST_ASSERT(result == 0, "Non-ALU opcode (MOVI) returns 0");
    }


    {
        initRegisters();
        int8_t result = execute(0, 42, 0);
        TEST_ASSERT(result == 42, "ADD with zero: 42 + 0 = 42");
    }

   
    {
        initRegisters();
        int8_t result = execute(2, -5, -3);
        TEST_ASSERT(result == 15, "MUL -5 * -3 = 15 (positive)");
    }

    
    {
        initRegisters();
        int8_t a = (int8_t)0b11111111;
        int8_t b = (int8_t)0b10101010;
        int8_t result = execute(5, a, b);
        TEST_ASSERT(result == (int8_t)0b10101010, "ANDI 0b11111111 & 0b10101010 = 0b10101010");
    }

    
    {
        initRegisters();
        int8_t a = 42;
        int8_t result = execute(6, a, a);
        TEST_ASSERT(result == 0, "EOR a ^ a = 0");
    }

    
    printf("\n========================================================\n");
    printf("  TEST SUMMARY\n");
    printf("========================================================\n");
    printf("  Total tests: %d\n", test_count);
    printf("  Passed: %d\n", test_pass);
    printf("  Failed: %d\n", test_count - test_pass);
    if (test_count == test_pass)
    {
        printf("\n  ALL TESTS PASSED!\n");
    }
    else
    {
        printf("\n  SOME TESTS FAILED!\n");
    }
    printf("========================================================\n\n");

    return (test_count == test_pass) ? 0 : 1;
}
