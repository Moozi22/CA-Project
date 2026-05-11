#include "alu.h"
#include "registers.h"
#include <stdio.h>
#include <string.h>

/* =========================================================
 *  ALU IMPLEMENTATION
 * ========================================================= */

/**
 * execute() - Execute ALU operations
 */
int8_t execute(int opcode, int8_t val1, int8_t val2)
{
    int8_t result = 0;
    
    switch (opcode) {
        case 0:  /* ADD: val1 + val2 */
        {
            int sum = (int)val1 + (int)val2;
            result = (int8_t)sum;
            updateFlagsADD(val1, val2, result);
            break;
        }
        
        case 1:  /* SUB: val1 - val2 */
        {
            int diff = (int)val1 - (int)val2;
            result = (int8_t)diff;
            updateFlagsSUB(val1, val2, result);
            break;
        }
        
        case 2:  /* MUL: val1 * val2 */
        {
            int product = (int)val1 * (int)val2;
            result = (int8_t)product;
            updateFlagsNZ(result);
            break;
        }
        
        case 5:  /* ANDI: val1 & val2 (bitwise AND) */
        {
            result = val1 & val2;
            updateFlagsNZ(result);
            break;
        }
        
        case 6:  /* EOR: val1 ^ val2 (bitwise XOR) */
        {
            result = val1 ^ val2;
            updateFlagsNZ(result);
            break;
        }
        
        case 8:  /* SLC: Shift Left Circular (rotate left) */
        {
            uint8_t val_unsigned = (uint8_t)val1;
            uint8_t carry_bit = (val_unsigned >> 7) & 1;
            result = (int8_t)((val_unsigned << 1) | carry_bit);
            updateFlagsNZ(result);
            break;
        }
        
        case 9:  /* SRC: Shift Right Circular (rotate right) */
        {
            uint8_t val_unsigned = (uint8_t)val1;
            uint8_t carry_bit = val_unsigned & 1;
            result = (int8_t)((val_unsigned >> 1) | (carry_bit << 7));
            updateFlagsNZ(result);
            break;
        }
        
        case 3:  /* MOVI: immediate move (not ALU operation) */
        case 4:  /* BEQZ: branch (handled separately) */
        case 7:  /* BR: branch (handled separately) */
        case 10: /* LDR: load from memory (not ALU) */
        case 11: /* STR: store to memory (not ALU) */
        default:
            /* Non-ALU operations return 0 */
            result = 0;
            break;
    }
    
    return result;
}

/**
 * calculateBranch() - Calculate target address for branch instructions
 */
uint16_t calculateBranch(uint16_t currentPC, int8_t immediate)
{
    /* Formula: PC_new = PC + 1 + immediate
     * The "+1" accounts for normal sequential instruction fetch
     * The "immediate" is the branch offset (sign-extended from 6 bits)
     */
    return currentPC + 1 + immediate;
}

/* =========================================================
 *  UNIT TESTS
 * ========================================================= */

static int test_count = 0;
static int test_pass = 0;

#define TEST_ASSERT(cond, msg)                                  \
    do {                                                        \
        test_count++;                                           \
        if (cond) {                                             \
            test_pass++;                                        \
            printf("  PASS: %s\n", msg);                        \
        } else {                                                \
            printf("  FAIL: %s\n", msg);                        \
        }                                                       \
    } while (0)

/**
 * testALU() - Comprehensive ALU unit tests
 */
int testALU(void)
{
    printf("\n========================================================\n");
    printf("  ALU & EXECUTION LOGIC UNIT TESTS (Member 4)\n");
    printf("========================================================\n");
    
    /* Initialize for fresh test run */
    test_count = 0;
    test_pass = 0;
    initRegisters();
    
    /* =====================================================
     *  TEST GROUP 1: ARITHMETIC OPERATIONS
     * ===================================================== */
    printf("\n--- Test Group 1: Arithmetic Operations ---\n");
    
    /* Test 1.1: ADD - basic addition */
    {
        int8_t a = 5, b = 3;
        int8_t result = execute(0, a, b);
        TEST_ASSERT(result == 8, "ADD 5 + 3 = 8");
    }
    
    /* Test 1.2: ADD - negative result */
    {
        initRegisters();
        int8_t a = -5, b = -3;
        int8_t result = execute(0, a, b);
        TEST_ASSERT(result == -8, "ADD -5 + (-3) = -8");
    }
    
    /* Test 1.3: ADD - overflow positive */
    {
        initRegisters();
        int8_t a = 127, b = 1;
        int8_t result = execute(0, a, b);
        TEST_ASSERT(result == -128, "ADD 127 + 1 = -128 (overflow)");
    }
    
    /* Test 1.4: ADD - overflow negative */
    {
        initRegisters();
        int8_t a = -128, b = -1;
        int8_t result = execute(0, a, b);
        TEST_ASSERT(result == 127, "ADD -128 + (-1) = 127 (underflow)");
    }
    
    /* Test 1.5: SUB - basic subtraction */
    {
        initRegisters();
        int8_t a = 10, b = 3;
        int8_t result = execute(1, a, b);
        TEST_ASSERT(result == 7, "SUB 10 - 3 = 7");
    }
    
    /* Test 1.6: SUB - negative result */
    {
        initRegisters();
        int8_t a = 3, b = 10;
        int8_t result = execute(1, a, b);
        TEST_ASSERT(result == -7, "SUB 3 - 10 = -7");
    }
    
    /* Test 1.7: MUL - basic multiplication */
    {
        initRegisters();
        int8_t a = 5, b = 3;
        int8_t result = execute(2, a, b);
        TEST_ASSERT(result == 15, "MUL 5 * 3 = 15");
    }
    
    /* Test 1.8: MUL - with zero */
    {
        initRegisters();
        int8_t a = 127, b = 0;
        int8_t result = execute(2, a, b);
        TEST_ASSERT(result == 0, "MUL 127 * 0 = 0");
    }
    
    /* Test 1.9: MUL - negative multiplication */
    {
        initRegisters();
        int8_t a = -5, b = 3;
        int8_t result = execute(2, a, b);
        TEST_ASSERT(result == -15, "MUL -5 * 3 = -15");
    }
    
    /* Test 1.10: MUL - overflow truncation */
    {
        initRegisters();
        int8_t a = 20, b = 20;
        int8_t result = execute(2, a, b);
        /* 20 * 20 = 400, truncated to 8-bit = 144 (in signed: -112 due to sign bit) */
        int expected = (int8_t)(20 * 20);
        TEST_ASSERT(result == expected, "MUL 20 * 20 with overflow");
    }
    
    /* =====================================================
     *  TEST GROUP 2: LOGICAL OPERATIONS
     * ===================================================== */
    printf("\n--- Test Group 2: Logical Operations ---\n");
    
    /* Test 2.1: ANDI - basic AND */
    {
        initRegisters();
        int8_t a = (int8_t)0b11110000, b = (int8_t)0b10101010;
        int8_t result = execute(5, a, b);
        TEST_ASSERT(result == (int8_t)0b10100000, "ANDI 0xF0 & 0xAA = 0xA0");
    }
    
    /* Test 2.2: ANDI - result zero */
    {
        initRegisters();
        int8_t a = (int8_t)0b11110000, b = (int8_t)0b00001111;
        int8_t result = execute(5, a, b);
        TEST_ASSERT(result == 0, "ANDI 0xF0 & 0x0F = 0x00");
    }
    
    /* Test 2.3: EOR - basic XOR */
    {
        initRegisters();
        int8_t a = (int8_t)0b11110000, b = (int8_t)0b10101010;
        int8_t result = execute(6, a, b);
        TEST_ASSERT(result == (int8_t)0b01011010, "EOR 0xF0 ^ 0xAA = 0x5A");
    }
    
    /* Test 2.4: EOR - result zero */
    {
        initRegisters();
        int8_t a = (int8_t)0b11110000, b = (int8_t)0b11110000;
        int8_t result = execute(6, a, b);
        TEST_ASSERT(result == 0, "EOR 0xF0 ^ 0xF0 = 0x00");
    }
    
    /* =====================================================
     *  TEST GROUP 3: SHIFT OPERATIONS (CIRCULAR)
     * ===================================================== */
    printf("\n--- Test Group 3: Shift Operations (Circular) ---\n");
    
    /* Test 3.1: SLC - shift left circular */
    {
        initRegisters();
        int8_t a = (int8_t)0b01010101;  /* 0x55 */
        int8_t result = execute(8, a, 0);
        /* Shift left: 0101_0101 -> 1010_1010 (carry bit 0 wraps to LSB) */
        TEST_ASSERT(result == (int8_t)0b10101010, "SLC 0x55 << 1 = 0xAA");
    }
    
    /* Test 3.2: SLC - with MSB set */
    {
        initRegisters();
        int8_t a = (int8_t)0b10000000;  /* 0x80 */
        int8_t result = execute(8, a, 0);
        /* Shift left: 1000_0000 -> 0000_0001 (carry bit 1 wraps to LSB) */
        TEST_ASSERT(result == (int8_t)0b00000001, "SLC 0x80 << 1 = 0x01");
    }
    
    /* Test 3.3: SRC - shift right circular */
    {
        initRegisters();
        int8_t a = (int8_t)0b10101010;  /* 0xAA */
        int8_t result = execute(9, a, 0);
        /* Shift right: 1010_1010 -> 0101_0101 (carry bit 0 wraps to MSB as 0) */
        TEST_ASSERT(result == (int8_t)0b01010101, "SRC 0xAA >> 1 = 0x55");
    }
    
    /* Test 3.4: SRC - with LSB set */
    {
        initRegisters();
        int8_t a = (int8_t)0b00000001;  /* 0x01 */
        int8_t result = execute(9, a, 0);
        /* Shift right: 0000_0001 -> 1000_0000 (carry bit 1 wraps to MSB) */
        TEST_ASSERT(result == (int8_t)0b10000000, "SRC 0x01 >> 1 = 0x80");
    }
    
    /* =====================================================
     *  TEST GROUP 4: BRANCH CALCULATION
     * ===================================================== */
    printf("\n--- Test Group 4: Branch Address Calculation ---\n");
    
    /* Test 4.1: BR - simple branch forward */
    {
        uint16_t pc = 10;
        int8_t imm = 5;
        uint16_t result = calculateBranch(pc, imm);
        TEST_ASSERT(result == 16, "calculateBranch(10, 5) = 16 (10+1+5)");
    }
    
    /* Test 4.2: BR - branch backward */
    {
        uint16_t pc = 20;
        int8_t imm = -5;
        uint16_t result = calculateBranch(pc, imm);
        TEST_ASSERT(result == 16, "calculateBranch(20, -5) = 16 (20+1-5)");
    }
    
    /* Test 4.3: BR - branch from start */
    {
        uint16_t pc = 0;
        int8_t imm = 10;
        uint16_t result = calculateBranch(pc, imm);
        TEST_ASSERT(result == 11, "calculateBranch(0, 10) = 11 (0+1+10)");
    }
    
    /* Test 4.4: BR - no branch offset */
    {
        uint16_t pc = 100;
        int8_t imm = 0;
        uint16_t result = calculateBranch(pc, imm);
        TEST_ASSERT(result == 101, "calculateBranch(100, 0) = 101 (100+1+0)");
    }
    
    /* Test 4.5: BR - large offset */
    {
        uint16_t pc = 500;
        int8_t imm = 127;
        uint16_t result = calculateBranch(pc, imm);
        TEST_ASSERT(result == 628, "calculateBranch(500, 127) = 628 (500+1+127)");
    }
    
    /* Test 4.6: BR - large negative offset */
    {
        uint16_t pc = 500;
        int8_t imm = -128;
        uint16_t result = calculateBranch(pc, imm);
        TEST_ASSERT(result == 373, "calculateBranch(500, -128) = 373 (500+1-128)");
    }
    
    /* =====================================================
     *  TEST GROUP 5: FLAG UPDATES
     * ===================================================== */
    printf("\n--- Test Group 5: Flag Updates Verification ---\n");
    
    /* Test 5.1: ADD sets Carry flag on overflow */
    {
        initRegisters();
        int8_t a = (int8_t)200, b = (int8_t)100;  /* Unsigned overflow */
        execute(0, a, b);
        /* 200 + 100 = 300 (0x12C), carry is set because unsigned sum > 255 */
        int carry_set = (SREG >> 4) & 1;
        TEST_ASSERT(carry_set, "ADD sets Carry flag on unsigned overflow");
    }
    
    /* Test 5.2: ADD clears flags on normal operation */
    {
        initRegisters();
        int8_t a = 5, b = 3;
        execute(0, a, b);
        int zero_clear = ((SREG >> 0) & 1) == 0;
        int carry_clear = ((SREG >> 4) & 1) == 0;
        TEST_ASSERT(zero_clear && carry_clear, 
                   "ADD clears Z and C flags on normal positive result");
    }
    
    /* Test 5.3: MUL/ANDI/EOR sets Zero flag */
    {
        initRegisters();
        int8_t a = (int8_t)0b11110000;
        int8_t b = (int8_t)0b00001111;
        int8_t result = execute(5, a, b);  /* ANDI -> 0 */
        int zero_set = (SREG >> 0) & 1;
        TEST_ASSERT(zero_set && result == 0, "ANDI sets Zero flag when result is 0");
    }
    
    /* Test 5.4: SLC/SRC sets Negative flag on negative result */
    {
        initRegisters();
        int8_t a = (int8_t)0b01000000;
        int8_t result = execute(8, a, 0);  /* SLC: becomes 0x80 which is negative */
        int negative_set = (SREG >> 2) & 1;
        TEST_ASSERT(negative_set && result < 0, 
                   "SLC sets Negative flag when MSB is 1");
    }
    
    /* =====================================================
     *  TEST GROUP 6: EDGE CASES & SPECIAL CASES
     * ===================================================== */
    printf("\n--- Test Group 6: Edge Cases & Special Cases ---\n");
    
    /* Test 6.1: Non-ALU opcodes return 0 */
    {
        initRegisters();
        int8_t result = execute(3, 10, 20);  /* MOVI is not ALU */
        TEST_ASSERT(result == 0, "Non-ALU opcode (MOVI) returns 0");
    }
    
    /* Test 6.2: ADD with zero */
    {
        initRegisters();
        int8_t result = execute(0, 42, 0);
        TEST_ASSERT(result == 42, "ADD with zero: 42 + 0 = 42");
    }
    
    /* Test 6.3: MUL negative * negative */
    {
        initRegisters();
        int8_t result = execute(2, -5, -3);
        TEST_ASSERT(result == 15, "MUL -5 * -3 = 15 (positive)");
    }
    
    /* Test 6.4: ANDI with all ones */
    {
        initRegisters();
        int8_t a = (int8_t)0xFF;
        int8_t b = (int8_t)0xAA;
        int8_t result = execute(5, a, b);
        TEST_ASSERT(result == (int8_t)0xAA, "ANDI 0xFF & 0xAA = 0xAA");
    }
    
    /* Test 6.5: EOR with self */
    {
        initRegisters();
        int8_t a = 42;
        int8_t result = execute(6, a, a);
        TEST_ASSERT(result == 0, "EOR a ^ a = 0");
    }
    
    /* =====================================================
     *  RESULTS SUMMARY
     * ===================================================== */
    printf("\n========================================================\n");
    printf("  TEST SUMMARY\n");
    printf("========================================================\n");
    printf("  Total tests: %d\n", test_count);
    printf("  Passed: %d\n", test_pass);
    printf("  Failed: %d\n", test_count - test_pass);
    if (test_count == test_pass) {
        printf("\n  ALL TESTS PASSED!\n");
    } else {
        printf("\n  SOME TESTS FAILED!\n");
    }
    printf("========================================================\n\n");
    
    return (test_count == test_pass) ? 0 : 1;
}
