

#include "registers.h"
#include <stdio.h>
#include <stdint.h>

/* ----- simple pass/fail counter ----- */
static int tests_run  = 0;
static int tests_pass = 0;

#define CHECK(cond, msg)                                         \
    do {                                                         \
        tests_run++;                                             \
        if (cond) {                                              \
            tests_pass++;                                        \
            printf("  PASS : %s\n", msg);                       \
        } else {                                                 \
            printf("  FAIL : %s\n", msg);                       \
        }                                                        \
    } while (0)



static void test_init(void)
{
    printf("\n--- Test 1: Initialisation ---\n");
    initRegisters();
    int all_zero = 1;
    for (int i = 0; i < 64; i++)
    {
        if (GPR[i] != 0) { all_zero = 0; break; }
    }
    CHECK(all_zero,         "All 64 GPRs initialised to 0");
    CHECK(SREG == 0b00000000,     "SREG initialised to 0x00");
}

static void test_get_set(void)
{
    printf("\n--- Test 2: getRegister / setRegister ---\n");
    initRegisters();

    setRegister(0,  10,  "EX");
    setRegister(1,  -5,  "EX");
    setRegister(63, 127, "EX");

    CHECK(getRegister(0)  ==  10,  "R0  = 10");
    CHECK(getRegister(1)  ==  -5,  "R1  = -5");
    CHECK(getRegister(63) == 127,  "R63 = 127");


    int8_t bad = getRegister(64);
    CHECK(bad == 0, "getRegister(64) returns 0 (out of range)");

    setRegister(0, 42, "EX");
    CHECK(getRegister(0) == 42, "R0 overwritten to 42");
}

static void test_carry_flag(void)
{
    printf("\n--- Test 3: Carry Flag (C) ---\n");
    initRegisters();


    int8_t v1 = (int8_t)200; 
    int8_t v2 = 100;
    int8_t result = (int8_t)(v1 + v2); 
    updateFlagsADD(v1, v2, result);
    CHECK(FLAG_C == 1, "Carry: 200u + 100u = 300 → C=1");

    initRegisters();
    result = (int8_t)(10 + 20);
    updateFlagsADD(10, 20, result);
    CHECK(FLAG_C == 0, "No carry: 10 + 20 = 30 → C=0");


    initRegisters();
    v1 = -5; v2 = 5;
    result = (int8_t)(v1 + v2); 
    updateFlagsADD(v1, v2, result);
    CHECK(FLAG_C == 1, "Carry: (-5)+5 → unsigned 251+5=256 → C=1");
}

static void test_overflow_flag(void)
{
    printf("\n--- Test 4: Overflow Flag (V) ---\n");
    initRegisters();

   
    int8_t v1 = 64, v2 = 64;
    int8_t result = (int8_t)(v1 + v2); 
    updateFlagsADD(v1, v2, result);
    CHECK(FLAG_V == 1, "ADD Overflow: 64+64=-128 (signed) → V=1");
    CHECK(FLAG_C == 0, "ADD Overflow: 64+64 no unsigned carry → C=0");

    initRegisters();
    v1 = 64; v2 = -64;
    result = (int8_t)(v1 + v2);
    updateFlagsADD(v1, v2, result);
    CHECK(FLAG_V == 0, "ADD No overflow: 64+(-64)=0 → V=0");
    CHECK(FLAG_C == 1, "ADD No overflow: 64+(-64), unsigned 64+192=256 → C=1");

    
    initRegisters();
    v1 = 127; v2 = -1;
    result = (int8_t)(v1 - v2); 
    updateFlagsSUB(v1, v2, result);
    CHECK(FLAG_V == 1, "SUB Overflow: 127-(-1) → V=1");

   
    initRegisters();
    v1 = 5; v2 = 3;
    result = (int8_t)(v1 - v2);
    updateFlagsSUB(v1, v2, result);
    CHECK(FLAG_V == 0, "SUB No overflow: 5-3=2 → V=0");
}

static void test_negative_flag(void)
{
    printf("\n--- Test 5: Negative Flag (N) ---\n");
    initRegisters();

    int8_t v1 = -10, v2 = -5;
    int8_t result = (int8_t)(v1 + v2); /* -15 */
    updateFlagsADD(v1, v2, result);
    CHECK(FLAG_N == 1, "Negative result -15 → N=1");

    initRegisters();
    result = (int8_t)(10 + 5); /* 15 */
    updateFlagsADD(10, 5, result);
    CHECK(FLAG_N == 0, "Positive result 15 → N=0");

    initRegisters();
    updateFlagsNZ(0);
    CHECK(FLAG_N == 0, "Zero result → N=0");
}

static void test_sign_flag(void)
{
    printf("\n--- Test 6: Sign Flag (S = N XOR V) ---\n");

   
    initRegisters();
    int8_t result = (int8_t)(5 + 3);
    updateFlagsADD(5, 3, result);
    CHECK(FLAG_S == (FLAG_N ^ FLAG_V), "S = N XOR V (positive result)");

   
    initRegisters();
    result = (int8_t)(64 + 64); 
    updateFlagsADD(64, 64, result);
    
    CHECK(FLAG_S == 0, "S: N=1 XOR V=1 = 0");

   
    initRegisters();
    result = (int8_t)(-10 - 5); /* -15 */
    updateFlagsSUB(-10, 5, result);
    CHECK(FLAG_S == (FLAG_N ^ FLAG_V), "S = N XOR V (SUB case)");
}

static void test_zero_flag(void)
{
    printf("\n--- Test 7: Zero Flag (Z) ---\n");
    initRegisters();

    /* 5 - 5 = 0 → Z=1 */
    int8_t result = (int8_t)(5 - 5);
    updateFlagsSUB(5, 5, result);
    CHECK(FLAG_Z == 1, "5-5=0 → Z=1");

    initRegisters();
    result = (int8_t)(5 + 1);
    updateFlagsADD(5, 1, result);
    CHECK(FLAG_Z == 0, "5+1=6 → Z=0");

   
    initRegisters();
    updateFlagsNZ(0);
    CHECK(FLAG_Z == 1, "NZ bundle with 0 → Z=1");

    
    initRegisters();
    updateFlagsNZ(-7);
    CHECK(FLAG_Z == 0, "NZ bundle with -7 → Z=0");
    CHECK(FLAG_N == 1, "NZ bundle with -7 → N=1");
}

static void test_sreg_reserved_bits(void)
{
    printf("\n--- Test 8: SREG bits [7:5] always 0 ---\n");
    initRegisters();

   
    SREG = 0b11111111;
    int8_t result = (int8_t)(10 + 20);
    updateFlagsADD(10, 20, result);

    CHECK((SREG & 0b11100000) == 0b00000000, "After flag update, SREG[7:5] are cleared");
}

static void test_full_sequence(void)
{
    printf("\n--- Test 9: Full pipeline sequence ---\n");
    initRegisters();

    
    setRegister(5, 10, "EX");
    CHECK(getRegister(5) == 10, "R5 = 10 after MOVI");

    setRegister(3, 20, "EX");
    int8_t a = getRegister(5);
    int8_t b = getRegister(3);
    int8_t res = (int8_t)(a + b);
    setRegister(5, res, "EX");
    updateFlagsADD(a, b, res);
    CHECK(getRegister(5) == 30, "R5 = 30 after ADD R5 R3");
    CHECK(FLAG_Z == 0, "Z=0 (result 30 != 0)");
    CHECK(FLAG_N == 0, "N=0 (result 30 > 0)");
    CHECK(FLAG_C == 0, "C=0 (10+20=30, no carry)");

   
    a = getRegister(5);  /* 30 */
    b = getRegister(3);  /* 20 */
    res = (int8_t)(a - b);
    setRegister(5, res, "EX");
    updateFlagsSUB(a, b, res);
    CHECK(getRegister(5) == 10, "R5 = 10 after SUB R5 R3");
    CHECK(FLAG_Z == 0, "Z=0 after SUB");
    CHECK(FLAG_V == 0, "V=0 after SUB (no overflow)");
}

static void test_print_functions(void)
{
    printf("\n--- Test 10: Print functions ---\n");
    initRegisters();
    setRegister(0,   42, "EX");
    setRegister(1,   -7, "EX");
    setRegister(63, 100, "EX");

  
    int8_t a = 64, b = 64;
    int8_t res = (int8_t)(a + b);
    updateFlagsADD(a, b, res);

    printf("\n  [Printing all registers...]\n");
    printRegisters();

    printf("\n  [Printing SREG...]\n");
    printSREG();

    printf("  (Visual inspection required for print tests)\n");
    tests_run++;
    tests_pass++; 
    printf("  PASS : printRegisters() and printSREG() did not crash\n");
}



int main(void)
{
    printf("=====================================================\n");
    printf("  PACKAGE 3  -  Register File & SREG Unit Tests\n");
    printf("=====================================================\n");

    test_init();
    test_get_set();
    test_carry_flag();
    test_overflow_flag();
    test_negative_flag();
    test_sign_flag();
    test_zero_flag();
    test_sreg_reserved_bits();
    test_full_sequence();
    test_print_functions();

    printf("\n=====================================================\n");
    printf("  Results: %d / %d tests passed\n", tests_pass, tests_run);
    printf("=====================================================\n");

    return (tests_pass == tests_run) ? 0 : 1;
}
