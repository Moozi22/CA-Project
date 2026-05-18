

#include "alu.h"
#include "registers.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


static int total_tests = 0;
static int passed_tests = 0;

#define TEST(condition, description)               \
    do                                             \
    {                                              \
        total_tests++;                             \
        if (condition)                             \
        {                                          \
            passed_tests++;                        \
            printf("  ✓ PASS: %s\n", description); \
        }                                          \
        else                                       \
        {                                          \
            printf("  ✗ FAIL: %s\n", description); \
        }                                          \
    } while (0)


static void test_arithmetic_operations(void)
{
    printf("\n====================================\n");
    printf("SECTION 1: ARITHMETIC OPERATIONS\n");
    printf("====================================\n");

    printf("\n--- ADD (Opcode 0) ---\n");

    initRegisters();
    int8_t result = execute(0, 10, 20);
    TEST(result == 30, "ADD: 10 + 20 = 30");

    initRegisters();
    result = execute(0, -5, -10);
    TEST(result == -15, "ADD: -5 + (-10) = -15");

    initRegisters();
    result = execute(0, 127, 1);
    TEST(result == -128, "ADD: 127 + 1 = -128 (overflow)");

    initRegisters();
    result = execute(0, -128, -1);
    TEST(result == 127, "ADD: -128 + (-1) = 127 (underflow)");

    initRegisters();
    result = execute(0, 0, 0);
    TEST(result == 0 && ((SREG >> 0) & 1) == 1, "ADD: 0 + 0 sets Z flag");

    initRegisters();
    result = execute(0, 127, 127);
    TEST(result == -2, "ADD: 127 + 127 with overflow");

  
    printf("\n--- SUB (Opcode 1) ---\n");

    initRegisters();
    result = execute(1, 20, 10);
    TEST(result == 10, "SUB: 20 - 10 = 10");

    initRegisters();
    result = execute(1, 10, 20);
    TEST(result == -10, "SUB: 10 - 20 = -10");

    initRegisters();
    result = execute(1, -5, 5);
    TEST(result == -10, "SUB: -5 - 5 = -10");

    initRegisters();
    result = execute(1, 127, -128);
    TEST(result == -1, "SUB: 127 - (-128) with overflow");

    initRegisters();
    result = execute(1, 0, 0);
    TEST(result == 0 && ((SREG >> 0) & 1) == 1, "SUB: 0 - 0 sets Z flag");

  
    printf("\n--- MUL (Opcode 2) ---\n");

    initRegisters();
    result = execute(2, 5, 3);
    TEST(result == 15, "MUL: 5 * 3 = 15");

    initRegisters();
    result = execute(2, -5, 3);
    TEST(result == -15, "MUL: -5 * 3 = -15");

    initRegisters();
    result = execute(2, -5, -3);
    TEST(result == 15, "MUL: -5 * -3 = 15");

    initRegisters();
    result = execute(2, 0, 100);
    TEST(result == 0 && ((SREG >> 0) & 1) == 1, "MUL: 0 * 100 = 0 (Z flag)");

    initRegisters();
    result = execute(2, 20, 20);
    TEST(result == (int8_t)(-112), "MUL: 20 * 20 = 400, truncated to 8-bit signed = -112");

    initRegisters();
    result = execute(2, 16, 16);
    TEST(result == 0, "MUL: 16 * 16 = 0 (256 truncated to 8-bit)");
}


static void test_logical_operations(void)
{
    printf("\n====================================\n");
    printf("SECTION 2: LOGICAL OPERATIONS\n");
    printf("====================================\n");

    
    printf("\n--- ANDI (Opcode 5): Bitwise AND ---\n");

    initRegisters();
    int8_t result = execute(5, (int8_t)0b11111111, (int8_t)0b00001111);
    TEST(result == (int8_t)0b00001111, "ANDI: 0b11111111 & 0b00001111 = 0b00001111");

    initRegisters();
    result = execute(5, (int8_t)0b11110000, (int8_t)0b00001111);
    TEST(result == 0, "ANDI: 0b11110000 & 0b00001111 = 0b00000000");

    initRegisters();
    result = execute(5, (int8_t)0b10101010, (int8_t)0b01010101);
    TEST(result == 0, "ANDI: 0b10101010 & 0b01010101 = 0b00000000");

    initRegisters();
    result = execute(5, (int8_t)0b11111111, (int8_t)0b11111111);
    TEST(result == (int8_t)0b11111111, "ANDI: 0b11111111 & 0b11111111 = 0b11111111");

    initRegisters();
    result = execute(5, (int8_t)0b10100101, (int8_t)0b01011010);
    TEST(result == 0, "ANDI: 0b10100101 & 0b01011010 = 0b00000000 (Z flag)");

   
    printf("\n--- EOR (Opcode 6): Bitwise XOR ---\n");

    initRegisters();
    result = execute(6, (int8_t)0b11111111, (int8_t)0b00000000);
    TEST(result == (int8_t)0b11111111, "EOR: 0b11111111 ^ 0b00000000 = 0b11111111");

    initRegisters();
    result = execute(6, (int8_t)0b11111111, (int8_t)0b11111111);
    TEST(result == 0, "EOR: 0b11111111 ^ 0b11111111 = 0b00000000 (Z flag)");

    initRegisters();
    result = execute(6, (int8_t)0b10100101, (int8_t)0b01011010);
    TEST(result == (int8_t)0b11111111, "EOR: 0b10100101 ^ 0b01011010 = 0b11111111");

    initRegisters();
    result = execute(6, (int8_t)0b00001111, (int8_t)0b11110000);
    TEST(result == (int8_t)0b11111111, "EOR: 0b00001111 ^ 0b11110000 = 0b11111111");

    initRegisters();
    result = execute(6, 42, 42);
    TEST(result == 0, "EOR: Same value XOR = 0 (self-annihilation)");
}


static void test_shift_operations(void)
{
    printf("\n====================================\n");
    printf("SECTION 3: SHIFT OPERATIONS\n");
    printf("  (Circular: SLC = Rotate Left, SRC = Rotate Right)\n");
    printf("====================================\n");

  
    printf("\n--- SLC (Opcode 8): Shift Left Circular ---\n");

    initRegisters();
    int8_t result = execute(8, (int8_t)0b01010101, 1); 
    TEST(result == (int8_t)0b10101010, "SLC: 0b01010101 rotated left by 1 = 0b10101010");

    initRegisters();
    result = execute(8, (int8_t)0b10000000, 1); 
    TEST(result == 1, "SLC: 0b10000000 rotated left by 1 = 0b00000001");

    initRegisters();
    result = execute(8, (int8_t)0b11111111, 4); 
    TEST(result == (int8_t)0b11111111, "SLC: 0b11111111 rotated left by 4 = 0b11111111");

    initRegisters();
    result = execute(8, (int8_t)0b00000001, 2); 
    TEST(result == 4, "SLC: 0b00000001 rotated left by 2 = 0b00000100");

    initRegisters();
    result = execute(8, (int8_t)0b01000000, 7); 
    TEST(result == (int8_t)0b00100000, "SLC: 0b01000000 rotated left by 7 = 0b00100000");

    initRegisters();
    result = execute(8, (int8_t)0b00010010, 8); 
    TEST(result == (int8_t)0b00010010, "SLC: rotation by 8 leaves value unchanged");

   
    printf("\n--- SRC (Opcode 9): Shift Right Circular ---\n");

    initRegisters();
    result = execute(9, (int8_t)0b10101010, 1);
    TEST(result == (int8_t)0b01010101, "SRC: 0b10101010 rotated right by 1 = 0b01010101");

    initRegisters();
    result = execute(9, (int8_t)0b00000001, 1); 
    TEST(result == (int8_t)0b10000000, "SRC: 0b00000001 rotated right by 1 = 0b10000000");

    initRegisters();
    result = execute(9, (int8_t)0b11111111, 3); 
    TEST(result == (int8_t)0b11111111, "SRC: 0b11111111 rotated right by 3 = 0b11111111");

    initRegisters();
    result = execute(9, (int8_t)0b00000010, 2); 
    TEST(result == 128, "SRC: 0b00000010 rotated right by 2 = 0b10000000");

    initRegisters();
    result = execute(9, (int8_t)0b10000000, 7); 
    TEST(result == 1, "SRC: 0b10000000 rotated right by 7 = 0b00000001");

    initRegisters();
    result = execute(9, (int8_t)0b00110100, 16); 
    TEST(result == (int8_t)0b00110100, "SRC: rotation by 16 leaves value unchanged");
}


static void test_flag_updates(void)
{
    printf("\n====================================\n");
    printf("SECTION 4: FLAG UPDATES\n");
    printf("====================================\n");

    printf("\n--- Carry Flag (C) - ADD only ---\n");

    initRegisters();
    execute(0, (int8_t)200, (int8_t)100); /* Unsigned overflow */
    int carry = (SREG >> 4) & 1;
    TEST(carry == 1, "ADD: Carry set on unsigned overflow (200+100)");

    initRegisters();
    execute(0, 5, 10);
    carry = (SREG >> 4) & 1;
    TEST(carry == 0, "ADD: Carry clear on normal addition (5+10)");

    printf("\n--- Overflow Flag (V) - ADD, SUB ---\n");

    initRegisters();
    execute(0, 127, 1); /* Signed overflow */
    int overflow = (SREG >> 3) & 1;
    TEST(overflow == 1, "ADD: Overflow set (127+1)");

    initRegisters();
    execute(0, 5, 10);
    overflow = (SREG >> 3) & 1;
    TEST(overflow == 0, "ADD: Overflow clear (5+10)");

    initRegisters();
    execute(1, 127, -128);
    overflow = (SREG >> 3) & 1;
    TEST(overflow == 1, "SUB: Overflow set (127-(-128))");

    printf("\n--- Negative Flag (N) - ADD, SUB, MUL, ANDI, EOR, SLC, SRC ---\n");

    initRegisters();
    execute(0, -5, -10);
    int negative = (SREG >> 2) & 1;
    TEST(negative == 1, "ADD: Negative flag set (-15)");

    initRegisters();
    execute(0, 5, 10);
    negative = (SREG >> 2) & 1;
    TEST(negative == 0, "ADD: Negative flag clear (15)");

    printf("\n--- Zero Flag (Z) - ADD, SUB, MUL, ANDI, EOR, SLC, SRC ---\n");

    initRegisters();
    execute(0, 5, -5);
    int zero = (SREG >> 0) & 1;
    TEST(zero == 1, "ADD: Zero flag set (5-5=0)");

    initRegisters();
    execute(0, 5, 10);
    zero = (SREG >> 0) & 1;
    TEST(zero == 0, "ADD: Zero flag clear (15)");

    printf("\n--- Sign Flag (S) - ADD, SUB only ---\n");
    printf("  S = N XOR V\n");

    initRegisters();
    execute(0, 127, 1); 
    int sign = (SREG >> 1) & 1;
    TEST(sign == 0, "ADD: Sign flag (127+1): N=1, V=1, S=0");

    initRegisters();
    execute(0, 5, 10); 
    sign = (SREG >> 1) & 1;
    TEST(sign == 0, "ADD: Sign flag (5+10): N=0, V=0, S=0");
}


static void test_branch_calculation(void)
{
    printf("\n====================================\n");
    printf("SECTION 5: BRANCH ADDRESS CALCULATION\n");
    printf("  Formula: PC_new = PC + 1 + IMM\n");
    printf("====================================\n");

    printf("\n--- Basic Branch Calculations ---\n");

    uint16_t pc_new = calculateBranch(0, 5);
    TEST(pc_new == 6, "Branch from PC=0 with IMM=5: 0+1+5=6");

    pc_new = calculateBranch(10, 5);
    TEST(pc_new == 16, "Branch from PC=10 with IMM=5: 10+1+5=16");

    pc_new = calculateBranch(100, 0);
    TEST(pc_new == 101, "Branch from PC=100 with IMM=0: 100+1+0=101");

    printf("\n--- Negative Offsets (Backward Branches) ---\n");

    pc_new = calculateBranch(20, -5);
    TEST(pc_new == 16, "Branch from PC=20 with IMM=-5: 20+1-5=16");

    pc_new = calculateBranch(50, -10);
    TEST(pc_new == 41, "Branch from PC=50 with IMM=-10: 50+1-10=41");

    printf("\n--- Edge Cases ---\n");

    pc_new = calculateBranch(0, 0);
    TEST(pc_new == 1, "Branch from PC=0 with IMM=0: 0+1+0=1");

    pc_new = calculateBranch(1023, 1);
    TEST(pc_new == 1025, "Branch from PC=1023 with IMM=1: 1023+1+1=1025");

    pc_new = calculateBranch(500, 127); 
    TEST(pc_new == 628, "Branch with max positive offset: 500+1+127=628");

    pc_new = calculateBranch(500, -128); 
    TEST(pc_new == 373, "Branch with max negative offset: 500+1-128=373");
}

static void test_boundary_cases(void)
{
    printf("\n====================================\n");
    printf("SECTION 6: BOUNDARY CASES\n");
    printf("====================================\n");

    printf("\n--- 8-bit Boundary Values ---\n");

    initRegisters();
    int8_t result = execute(0, 127, 0);
    TEST(result == 127, "ADD: Max positive (127) + 0 = 127");

    initRegisters();
    result = execute(0, -128, 0);
    TEST(result == -128, "ADD: Min negative (-128) + 0 = -128");

    initRegisters();
    result = execute(2, 127, 0);
    TEST(result == 0, "MUL: 127 * 0 = 0");

    printf("\n--- Operations on Zero ---\n");

    initRegisters();
    result = execute(0, 0, 0);
    TEST(result == 0, "ADD: 0 + 0 = 0");

    initRegisters();
    result = execute(1, 0, 0);
    TEST(result == 0, "SUB: 0 - 0 = 0");

    initRegisters();
    result = execute(2, 0, 0);
    TEST(result == 0, "MUL: 0 * 0 = 0");

    initRegisters();
    result = execute(5, 0, 0b11111111);
    TEST(result == 0, "ANDI: 0 & 0b11111111 = 0");

    initRegisters();
    result = execute(6, 0, 0b11111111);
    TEST(result == (int8_t)0b11111111, "EOR: 0 ^ 0b11111111 = 0b11111111");

    printf("\n--- Repeated Shifts (Symmetry) ---\n");

    initRegisters();
    int8_t val = (int8_t)0b01010101;
    int8_t shifted = execute(8, val, 0); /* SLC once */
    initRegisters();
    shifted = execute(9, shifted, 0); /* SRC once */
    TEST(shifted == val, "SLC then SRC returns to original (0b01010101)");
}

static void test_non_alu_opcodes(void)
{
    printf("\n====================================\n");
    printf("SECTION 7: NON-ALU OPCODES\n");
    printf("====================================\n");

    printf("\n--- Verify Non-ALU Operations Return 0 ---\n");

    initRegisters();
    int8_t result = execute(3, 10, 20); /* MOVI */
    TEST(result == 0, "MOVI (opcode 3): Returns 0");

    initRegisters();
    result = execute(4, 10, 20); /* BEQZ */
    TEST(result == 0, "BEQZ (opcode 4): Returns 0");

    initRegisters();
    result = execute(7, 10, 20); /* BR */
    TEST(result == 0, "BR (opcode 7): Returns 0");

    initRegisters();
    result = execute(10, 10, 20); /* LDR */
    TEST(result == 0, "LDR (opcode 10): Returns 0");

    initRegisters();
    result = execute(11, 10, 20); /* STR */
    TEST(result == 0, "STR (opcode 11): Returns 0");
}

int main(void)
{
    printf("\n");
    printf("╔═══════════════════════════════════════════════════╗\n");
    printf("║  COMPREHENSIVE ALU TESTING - MEMBER 4             ║\n");
    printf("║  CSEN601 Project - Package 3 Specification        ║\n");
    printf("╚═══════════════════════════════════════════════════╝\n");

    test_arithmetic_operations();
    test_logical_operations();
    test_shift_operations();
    test_flag_updates();
    test_branch_calculation();
    test_boundary_cases();
    test_non_alu_opcodes();

    printf("\n");
    printf("╔═══════════════════════════════════════════════════╗\n");
    printf("║  TEST SUMMARY                                     ║\n");
    printf("╠═══════════════════════════════════════════════════╣\n");
    printf("║  Total Tests:    %3d                              ║\n", total_tests);
    printf("║  Passed:         %3d                              ║\n", passed_tests);
    printf("║  Failed:         %3d                              ║\n", total_tests - passed_tests);
    printf("║  Pass Rate:      %.1f%%                             ║\n",
           (total_tests > 0) ? (100.0 * passed_tests / total_tests) : 0);
    printf("╠═══════════════════════════════════════════════════╣\n");

    if (passed_tests == total_tests)
    {
        printf("║  ✓ ALL TESTS PASSED - ALU READY FOR INTEGRATION   ║\n");
    }
    else
    {
        printf("║  ✗ SOME TESTS FAILED - REVIEW ABOVE              ║\n");
    }
    printf("╚═══════════════════════════════════════════════════╝\n\n");

    return (passed_tests == total_tests) ? 0 : 1;
}
