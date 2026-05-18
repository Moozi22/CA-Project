/* =========================================================
 *  test_alu_comprehensive.c  -  Comprehensive ALU Testing
 *
 *  Based on CSEN601 Project - Package 3 Specification
 *  Tests Member 4: ALU & Execution Logic (EX Stage)
 *
 *  Compile:
 *    gcc -Wall -o test_alu_comprehensive test_alu_comprehensive.c alu.c registers.c -lm
 *  Run:
 *    ./test_alu_comprehensive
 * ========================================================= */

#include "alu.h"
#include "registers.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Test counters */
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

/* =========================================================
 *  SECTION 1: ARITHMETIC OPERATIONS (ADD, SUB, MUL)
 * ========================================================= */
static void test_arithmetic_operations(void)
{
    printf("\n====================================\n");
    printf("SECTION 1: ARITHMETIC OPERATIONS\n");
    printf("====================================\n");

    /* -------- ADD (Opcode 0) -------- */
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

    /* -------- SUB (Opcode 1) -------- */
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

    /* -------- MUL (Opcode 2) -------- */
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

/* =========================================================
 *  SECTION 2: LOGICAL OPERATIONS (ANDI, EOR)
 * ========================================================= */
static void test_logical_operations(void)
{
    printf("\n====================================\n");
    printf("SECTION 2: LOGICAL OPERATIONS\n");
    printf("====================================\n");

    /* -------- ANDI (Opcode 5) -------- */
    printf("\n--- ANDI (Opcode 5): Bitwise AND ---\n");

    initRegisters();
    int8_t result = execute(5, (int8_t)0xFF, (int8_t)0x0F);
    TEST(result == (int8_t)0x0F, "ANDI: 0xFF & 0x0F = 0x0F");

    initRegisters();
    result = execute(5, (int8_t)0xF0, (int8_t)0x0F);
    TEST(result == 0, "ANDI: 0xF0 & 0x0F = 0x00");

    initRegisters();
    result = execute(5, (int8_t)0xAA, (int8_t)0x55);
    TEST(result == 0, "ANDI: 0xAA & 0x55 = 0x00");

    initRegisters();
    result = execute(5, (int8_t)0xFF, (int8_t)0xFF);
    TEST(result == (int8_t)0xFF, "ANDI: 0xFF & 0xFF = 0xFF");

    initRegisters();
    result = execute(5, (int8_t)0xA5, (int8_t)0x5A);
    TEST(result == 0, "ANDI: 0xA5 & 0x5A = 0x00 (Z flag)");

    /* -------- EOR (Opcode 6) -------- */
    printf("\n--- EOR (Opcode 6): Bitwise XOR ---\n");

    initRegisters();
    result = execute(6, (int8_t)0xFF, (int8_t)0x00);
    TEST(result == (int8_t)0xFF, "EOR: 0xFF ^ 0x00 = 0xFF");

    initRegisters();
    result = execute(6, (int8_t)0xFF, (int8_t)0xFF);
    TEST(result == 0, "EOR: 0xFF ^ 0xFF = 0x00 (Z flag)");

    initRegisters();
    result = execute(6, (int8_t)0xA5, (int8_t)0x5A);
    TEST(result == (int8_t)0xFF, "EOR: 0xA5 ^ 0x5A = 0xFF");

    initRegisters();
    result = execute(6, (int8_t)0x0F, (int8_t)0xF0);
    TEST(result == (int8_t)0xFF, "EOR: 0x0F ^ 0xF0 = 0xFF");

    initRegisters();
    result = execute(6, 42, 42);
    TEST(result == 0, "EOR: Same value XOR = 0 (self-annihilation)");
}

/* =========================================================
 *  SECTION 3: SHIFT OPERATIONS (SLC, SRC - Circular)
 * ========================================================= */
static void test_shift_operations(void)
{
    printf("\n====================================\n");
    printf("SECTION 3: SHIFT OPERATIONS\n");
    printf("  (Circular: SLC = Rotate Left, SRC = Rotate Right)\n");
    printf("====================================\n");

    /* -------- SLC (Opcode 8): Shift Left Circular -------- */
    printf("\n--- SLC (Opcode 8): Shift Left Circular ---\n");

    initRegisters();
    int8_t result = execute(8, (int8_t)0x55, 1); /* 01010101 */
    TEST(result == (int8_t)0xAA, "SLC: 0x55 rotated left by 1 = 0xAA");

    initRegisters();
    result = execute(8, (int8_t)0x80, 1); /* 10000000 */
    TEST(result == 1, "SLC: 0x80 rotated left by 1 = 0x01");

    initRegisters();
    result = execute(8, (int8_t)0xFF, 4); /* 11111111 */
    TEST(result == (int8_t)0xFF, "SLC: 0xFF rotated left by 4 = 0xFF");

    initRegisters();
    result = execute(8, (int8_t)0x01, 2); /* 00000001 */
    TEST(result == 4, "SLC: 0x01 rotated left by 2 = 0x04");

    initRegisters();
    result = execute(8, (int8_t)0x40, 7); /* 01000000 */
    TEST(result == (int8_t)0x20, "SLC: 0x40 rotated left by 7 = 0x20");

    initRegisters();
    result = execute(8, (int8_t)0x12, 8); /* rotate by 8 -> no change */
    TEST(result == (int8_t)0x12, "SLC: rotation by 8 leaves value unchanged");

    /* -------- SRC (Opcode 9): Shift Right Circular -------- */
    printf("\n--- SRC (Opcode 9): Shift Right Circular ---\n");

    initRegisters();
    result = execute(9, (int8_t)0xAA, 1); /* 10101010 */
    TEST(result == (int8_t)0x55, "SRC: 0xAA rotated right by 1 = 0x55");

    initRegisters();
    result = execute(9, (int8_t)0x01, 1); /* 00000001 */
    TEST(result == (int8_t)0x80, "SRC: 0x01 rotated right by 1 = 0x80");

    initRegisters();
    result = execute(9, (int8_t)0xFF, 3); /* 11111111 */
    TEST(result == (int8_t)0xFF, "SRC: 0xFF rotated right by 3 = 0xFF");

    initRegisters();
    result = execute(9, (int8_t)0x02, 2); /* 00000010 */
    TEST(result == 128, "SRC: 0x02 rotated right by 2 = 0x80");

    initRegisters();
    result = execute(9, (int8_t)0x80, 7); /* 10000000 */
    TEST(result == 1, "SRC: 0x80 rotated right by 7 = 0x01");

    initRegisters();
    result = execute(9, (int8_t)0x34, 16); /* rotate by 16 -> no change */
    TEST(result == (int8_t)0x34, "SRC: rotation by 16 leaves value unchanged");
}

/* =========================================================
 *  SECTION 4: FLAG UPDATES VERIFICATION
 * ========================================================= */
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
    execute(0, 127, 1); /* N=1, V=1, S should be 0 */
    int sign = (SREG >> 1) & 1;
    TEST(sign == 0, "ADD: Sign flag (127+1): N=1, V=1, S=0");

    initRegisters();
    execute(0, 5, 10); /* N=0, V=0, S should be 0 */
    sign = (SREG >> 1) & 1;
    TEST(sign == 0, "ADD: Sign flag (5+10): N=0, V=0, S=0");
}

/* =========================================================
 *  SECTION 5: BRANCH ADDRESS CALCULATION
 * ========================================================= */
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

    pc_new = calculateBranch(500, 127); /* Max positive immediate */
    TEST(pc_new == 628, "Branch with max positive offset: 500+1+127=628");

    pc_new = calculateBranch(500, -128); /* Max negative immediate */
    TEST(pc_new == 373, "Branch with max negative offset: 500+1-128=373");
}

/* =========================================================
 *  SECTION 6: SPECIAL CASES & BOUNDARY CONDITIONS
 * ========================================================= */
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
    result = execute(5, 0, 0xFF);
    TEST(result == 0, "ANDI: 0 & 0xFF = 0");

    initRegisters();
    result = execute(6, 0, 0xFF);
    TEST(result == (int8_t)0xFF, "EOR: 0 ^ 0xFF = 0xFF");

    printf("\n--- Repeated Shifts (Symmetry) ---\n");

    initRegisters();
    int8_t val = (int8_t)0x55;
    int8_t shifted = execute(8, val, 0); /* SLC once */
    initRegisters();
    shifted = execute(9, shifted, 0); /* SRC once */
    TEST(shifted == val, "SLC then SRC returns to original (0x55)");
}

/* =========================================================
 *  SECTION 7: NON-ALU OPCODES
 * ========================================================= */
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

/* =========================================================
 *  MAIN TEST RUNNER
 * ========================================================= */
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

    /* Print summary */
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
