/* =========================================================
 *  PACKAGE 3 - Register File & Status Specialist
 *  registers.c  -  Full implementation
 * =========================================================
 *
 *  Sections:
 *    1. Global variables (the actual register arrays)
 *    2. Initialisation
 *    3. General-Purpose Register access (getRegister, setRegister)
 *    4. Individual flag updaters (C, V, N, S, Z)
 *    5. Convenience flag updater bundles (ADD, SUB, NZ)
 *    6. End-of-simulation print functions
 * ========================================================= */

#include "registers.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* 64 General-Purpose Registers, each 8-bit signed.
 * Indexed directly: GPR[0] = R0, GPR[1] = R1, ..., GPR[63] = R63.
 * int8_t matches the spec's "char / int8_t" requirement exactly.  */
int8_t GPR[64];

/* Status Register - 8 bits, only bits [4:0] active.
 * Bit layout: | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *             | 0 | 0 | 0 | C | V | N | S | Z |
 * Bits [7:5] are always 0 per spec requirement.                   */
uint8_t SREG;

/* =========================================================
 *  SECTION 2 - INITIALISATION
 * ========================================================= */

void initRegisters(void)
{
    /* Zero-fill all 64 GPRs at once */
    memset(GPR, 0, sizeof(GPR));

    /* Zero out SREG — all flags start cleared */
    SREG = 0x00;

    printf("[Registers] 64 GPRs (R0-R63, 8-bit each) initialised to 0.\n");
    printf("[Registers] SREG (Status Register) initialised to 0x00.\n");
}

/* =========================================================
 *  SECTION 3 - GENERAL-PURPOSE REGISTER ACCESS
 * ========================================================= */

/* --- getRegister ---
 *
 *  Returns the 8-bit signed value of register regNum (0–63).
 *  Performs bounds checking and returns 0 on invalid access.
 *
 *  Note: In Package 3, ALL registers (including R0) are normal
 *  writable GPRs. Unlike Packages 1/2, there is no hard-wired
 *  zero register.                                              */
int8_t getRegister(uint8_t regNum)
{
    if (regNum > 63)
    {
        printf("[Registers] ERROR: getRegister - R%u is out of "
               "bounds (max R63).\n",
               regNum);
        return 0;
    }
    return GPR[regNum];
}

/* --- setRegister ---
 *
 *  Writes value to register regNum (0–63).
 *  Automatically prints the mandatory spec log line:
 *    "[<stage>] Register Updated: R<n> = <value>"
 *
 *  stageName : the pipeline stage performing the write (e.g. "EX").
 *              Pass NULL or "" to suppress stage label.
 *
 *  Spec point 3: "if you changed the value of a location in the
 *  memory or the register file, you need to print that this
 *  location or register value has changed alongside the new value
 *  (and in which stage did the value change)."                   */
void setRegister(uint8_t regNum, int8_t value, const char *stageName)
{
    if (regNum > 63)
    {
        printf("[Registers] ERROR: setRegister - R%u is out of "
               "bounds (max R63).\n",
               regNum);
        return;
    }

    GPR[regNum] = value;

    /* Mandatory spec print */
    if (stageName != NULL && stageName[0] != '\0')
    {
        printf("  [%s] Register Updated: R%u = %d\n",
               stageName, regNum, value);
    }
    else
    {
        printf("  [--] Register Updated: R%u = %d\n", regNum, value);
    }
}

/* =========================================================
 *  SECTION 4 - INDIVIDUAL FLAG UPDATERS
 *
 *  Each function updates exactly one flag in SREG.
 *  They are called from the convenience bundles in Section 5
 *  and can also be called directly by the ALU (Member 4).
 *
 *  Key spec rule (point 22, page 21):
 *    "A flag value can only be updated by the instructions
 *     related to it."
 *  These individual updaters enforce that contract by only
 *  touching their own bit and leaving all others untouched.
 * ========================================================= */

/* --- updateCarryFlag (C) ---
 *
 *  Spec: "Check on 9th bit (bit 8) of UNSIGNED[val1] OP UNSIGNED[val2]"
 *
 *  Method from spec "Carry Flag Check" section:
 *    int temp1 = val1 & 0xFF   (get unsigned 8-bit representation)
 *    int temp2 = val2 & 0xFF
 *    if (((temp1 + temp2) & 0x100) == 0x100) --> Carry = 1
 *
 *  The 0x100 mask isolates bit 8 (the 9th bit) of the 9-bit result.
 *
 *  Called by: ADD only                                        */
void updateCarryFlag(int val1, int val2)
{
    /* Strip to unsigned 8-bit view */
    int temp1 = val1 & 0xFF;
    int temp2 = val2 & 0xFF;
    int result9bit = temp1 + temp2;

    /* Bit 8 == 1 means carry out of the 8-bit result */
    if ((result9bit & 0x100) == 0x100)
    {
        SREG |= (1 << SREG_C_BIT); /* Set C */
    }
    else
    {
        SREG &= ~(1 << SREG_C_BIT); /* Clear C */
    }

    /* Always ensure bits [7:5] stay 0 (spec requirement) */
    SREG &= 0x1F;
}

/* --- updateOverflowFlag (V) ---
 *
 *  Spec rules for V:
 *    ADD: overflow if both operands have the SAME sign but the
 *         result has the OPPOSITE sign.
 *    SUB: overflow if the operands have DIFFERENT signs and the
 *         result has the SAME sign as the subtrahend (val2).
 *
 *  Quick XOR trick (mentioned in spec "Overflow vs Carry" section):
 *    V = carry_from_bit6_to_7  XOR  carry_from_bit7
 *
 *  We implement the sign-based rule here — it is equivalent and
 *  more readable.
 *
 *  isSubtraction : 0 for ADD, 1 for SUB
 *  Called by: ADD, SUB                                        */
void updateOverflowFlag(int8_t val1, int8_t val2, int8_t result8,
                        int isSubtraction)
{
    int overflow = 0;

    if (!isSubtraction)
    {
        /* ADD: overflow iff same-sign operands produce opposite-sign result */
        /* sign bits: negative = 1 (MSB set), positive = 0 */
        int sign1 = (val1 >> 7) & 1;
        int sign2 = (val2 >> 7) & 1;
        int signRes = (result8 >> 7) & 1;

        if (sign1 == sign2 && sign1 != signRes)
            overflow = 1;
    }
    else
    {
        /* SUB (val1 - val2): overflow iff signs differ AND result sign
         * equals the subtrahend (val2) sign                          */
        int sign1 = (val1 >> 7) & 1;
        int sign2 = (val2 >> 7) & 1;
        int signRes = (result8 >> 7) & 1;

        if (sign1 != sign2 && signRes == sign2)
            overflow = 1;
    }

    if (overflow)
        SREG |= (1 << SREG_V_BIT);
    else
        SREG &= ~(1 << SREG_V_BIT);

    SREG &= 0x1F;
}

/* --- updateNegativeFlag (N) ---
 *
 *  N = 1 if result is negative (MSB of 8-bit result is 1).
 *  N = 0 if result is zero or positive.
 *
 *  Called by: ADD, SUB, MUL, ANDI, EOR, SLC, SRC             */
void updateNegativeFlag(int8_t result8)
{
    if (result8 < 0)
        SREG |= (1 << SREG_N_BIT);
    else
        SREG &= ~(1 << SREG_N_BIT);

    SREG &= 0x1F;
}

/* --- updateSignFlag (S) ---
 *
 *  S = N XOR V  (expected sign, not actual sign).
 *  MUST be called AFTER both N and V have already been updated
 *  in the same cycle.
 *
 *  Called by: ADD, SUB                                        */
void updateSignFlag(void)
{
    int n = FLAG_N;
    int v = FLAG_V;
    int s = n ^ v; /* XOR */

    if (s)
        SREG |= (1 << SREG_S_BIT);
    else
        SREG &= ~(1 << SREG_S_BIT);

    SREG &= 0x1F;
}

/* --- updateZeroFlag (Z) ---
 *
 *  Z = 1 if result is exactly 0.
 *  Z = 0 otherwise.
 *
 *  Called by: ADD, SUB, MUL, ANDI, EOR, SLC, SRC             */
void updateZeroFlag(int8_t result8)
{
    if (result8 == 0)
        SREG |= (1 << SREG_Z_BIT);
    else
        SREG &= ~(1 << SREG_Z_BIT);

    SREG &= 0x1F;
}

/* =========================================================
 *  SECTION 5 - CONVENIENCE FLAG UPDATER BUNDLES
 *
 *  These are the functions the ALU (Member 4) calls after
 *  computing a result.  Each one updates exactly the flags
 *  the spec requires for that instruction type.
 *
 *  Calling order within each bundle matters: N and V must
 *  be set before S, because S = N XOR V.
 * ========================================================= */

/* --- updateFlagsADD ---
 *
 *  Called after:  ADD R1 R2   (R1 = R1 + R2)
 *  Updates flags: C, V, N, S, Z
 *
 *  val1, val2 : the original signed 8-bit operands.
 *  result8    : (int8_t)(val1 + val2) — the truncated 8-bit result.  */
void updateFlagsADD(int8_t val1, int8_t val2, int8_t result8)
{
    /* C — must pass as int to get the full 9-bit addition */
    updateCarryFlag((int)val1, (int)val2);

    /* V — addition, so isSubtraction = 0 */
    updateOverflowFlag(val1, val2, result8, 0);

    /* N — set before S */
    updateNegativeFlag(result8);

    /* S = N XOR V — must come after N and V */
    updateSignFlag();

    /* Z */
    updateZeroFlag(result8);

    /* Log SREG state after update */
    printf("  [EX] SREG Updated (ADD): C=%d V=%d N=%d S=%d Z=%d  "
           "(SREG=0x%02X)\n",
           FLAG_C, FLAG_V, FLAG_N, FLAG_S, FLAG_Z, SREG);
}

/* --- updateFlagsSUB ---
 *
 *  Called after:  SUB R1 R2   (R1 = R1 - R2)
 *  Updates flags: V, N, S, Z  (C is NOT updated by SUB)
 *
 *  val1, val2 : the original signed 8-bit operands.
 *  result8    : (int8_t)(val1 - val2)                               */
void updateFlagsSUB(int8_t val1, int8_t val2, int8_t result8)
{
    /* V — subtraction, so isSubtraction = 1 */
    updateOverflowFlag(val1, val2, result8, 1);

    /* N — set before S */
    updateNegativeFlag(result8);

    /* S = N XOR V */
    updateSignFlag();

    /* Z */
    updateZeroFlag(result8);

    printf("  [EX] SREG Updated (SUB): V=%d N=%d S=%d Z=%d  "
           "(SREG=0x%02X)\n",
           FLAG_V, FLAG_N, FLAG_S, FLAG_Z, SREG);
}

/* --- updateFlagsNZ ---
 *
 *  Called after:  MUL, ANDI, EOR, SLC, SRC
 *  Updates flags: N, Z only
 *
 *  result8 : the 8-bit result of the operation.                     */
void updateFlagsNZ(int8_t result8)
{
    updateNegativeFlag(result8);
    updateZeroFlag(result8);

    printf("  [EX] SREG Updated (NZ):  N=%d Z=%d  (SREG=0x%02X)\n",
           FLAG_N, FLAG_Z, SREG);
}

/* =========================================================
 *  SECTION 6 - END-OF-SIMULATION PRINT FUNCTIONS
 *
 *  The spec says:
 *    "At the end of your program, you need to print the values of
 *     all registers (general and special purpose including the PC
 *     and SREG), and the full instruction and data memory locations."
 * ========================================================= */

/* Helper: print an 8-bit value as an 8-character binary string */
static void printBinary8(uint8_t val)
{
    for (int b = 7; b >= 0; b--)
    {
        printf("%d", (val >> b) & 1);
    }
}

/* --- printRegisters ---
 *
 *  Prints all 64 GPR values in a clean table:
 *  Register | Decimal (signed) | Hex | Binary                    */
void printRegisters(void)
{
    printf("\n============================================================\n");
    printf("  GENERAL-PURPOSE REGISTER FILE DUMP  (64 x 8-bit)\n");
    printf("============================================================\n");
    printf("%-8s %-10s %-6s %-10s\n",
           "Register", "Decimal", "Hex", "Binary");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < 64; i++)
    {
        /* Cast to uint8_t for hex/binary display so negative numbers
         * show their two's complement bit pattern correctly.         */
        uint8_t raw = (uint8_t)GPR[i];
        printf("R%-7d %-10d 0x%02X   ", i, GPR[i], raw);
        printBinary8(raw);
        printf("\n");
    }

    printf("============================================================\n");
}

/* --- printSREG ---
 *
 *  Prints the SREG value with a labelled flag breakdown.
 *  Bits [7:5] are always shown as 0.                             */
void printSREG(void)
{
    /* Defensive: always ensure reserved bits are 0 before printing */
    SREG &= 0x1F;

    printf("\n============================================================\n");
    printf("  STATUS REGISTER (SREG) DUMP\n");
    printf("============================================================\n");
    printf("  SREG = 0x%02X  (binary: ", SREG);
    printBinary8(SREG);
    printf(")\n\n");
    printf("  Bit layout:  | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |\n");
    printf("  Flag name:   | 0 | 0 | 0 | C | V | N | S | Z |\n");
    printf("  Current val: | 0 | 0 | 0 | %d | %d | %d | %d | %d |\n",
           FLAG_C, FLAG_V, FLAG_N, FLAG_S, FLAG_Z);
    printf("\n");
    printf("  C (Carry)    = %d\n", FLAG_C);
    printf("  V (Overflow) = %d\n", FLAG_V);
    printf("  N (Negative) = %d\n", FLAG_N);
    printf("  S (Sign)     = %d  (= N XOR V = %d XOR %d)\n",
           FLAG_S, FLAG_N, FLAG_V);
    printf("  Z (Zero)     = %d\n", FLAG_Z);
    printf("============================================================\n");
}
