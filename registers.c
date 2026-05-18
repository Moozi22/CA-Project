

#include "registers.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>


int8_t GPR[64];


uint8_t SREG;



void initRegisters(void)
{
   
    memset(GPR, 0, sizeof(GPR));

    
    SREG = 0b00000000;

    printf("[Registers] 64 GPRs (R0-R63, 8-bit each) initialised to 0.\n");
    printf("[Registers] SREG (Status Register) initialised to 0x00.\n");
}


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

void updateCarryFlag(int val1, int val2)
{
    int temp1 = val1 & 0b11111111;
    int temp2 = val2 & 0b11111111;
    int result9bit = temp1 + temp2;

    if ((result9bit & 0b100000000) == 0b100000000)
    {
        SREG |= (1 << SREG_C_BIT); 
    }
    else
    {
        SREG &= ~(1 << SREG_C_BIT); 
    }
    SREG &= 0b00011111;
}

void updateOverflowFlag(int8_t val1, int8_t val2, int8_t result8,
                        int isSubtraction)
{
    int overflow = 0;

    if (!isSubtraction)
    {
        int sign1 = (val1 >> 7) & 1;
        int sign2 = (val2 >> 7) & 1;
        int signRes = (result8 >> 7) & 1;

        if (sign1 == sign2 && sign1 != signRes)
            overflow = 1;
    }
    else
    {
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

    SREG &= 0b00011111;
}


void updateNegativeFlag(int8_t result8)
{
    if (result8 < 0)
        SREG |= (1 << SREG_N_BIT);
    else
        SREG &= ~(1 << SREG_N_BIT);

    SREG &= 0b00011111;
}


void updateSignFlag(void)
{
    int n = FLAG_N;
    int v = FLAG_V;
    int s = n ^ v; 

    if (s)
        SREG |= (1 << SREG_S_BIT);
    else
        SREG &= ~(1 << SREG_S_BIT);

    SREG &= 0b00011111;
}

void updateZeroFlag(int8_t result8)
{
    if (result8 == 0)
        SREG |= (1 << SREG_Z_BIT);
    else
        SREG &= ~(1 << SREG_Z_BIT);

    SREG &= 0b00011111;
}


void updateFlagsADD(int8_t val1, int8_t val2, int8_t result8)
{
    updateCarryFlag((int)val1, (int)val2);

    updateOverflowFlag(val1, val2, result8, 0);

    updateNegativeFlag(result8);

    updateSignFlag();

    updateZeroFlag(result8);

    printf("  [EX] SREG Updated (ADD): C=%d V=%d N=%d S=%d Z=%d  "
           "(SREG=0x%02X)\n",
           FLAG_C, FLAG_V, FLAG_N, FLAG_S, FLAG_Z, SREG);
}

void updateFlagsSUB(int8_t val1, int8_t val2, int8_t result8)
{
    updateOverflowFlag(val1, val2, result8, 1);

    updateNegativeFlag(result8);

    updateSignFlag();

    updateZeroFlag(result8);

    printf("  [EX] SREG Updated (SUB): V=%d N=%d S=%d Z=%d  "
           "(SREG=0x%02X)\n",
           FLAG_V, FLAG_N, FLAG_S, FLAG_Z, SREG);
}

void updateFlagsNZ(int8_t result8)
{
    updateNegativeFlag(result8);
    updateZeroFlag(result8);

    printf("  [EX] SREG Updated (NZ):  N=%d Z=%d  (SREG=0x%02X)\n",
           FLAG_N, FLAG_Z, SREG);
}

static void printBinary8(uint8_t val)
{
    for (int b = 7; b >= 0; b--)
    {
        printf("%d", (val >> b) & 1);
    }
}

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
        uint8_t raw = (uint8_t)GPR[i];
        printf("R%-7d %-10d 0x%02X   ", i, GPR[i], raw);
        printBinary8(raw);
        printf("\n");
    }

    printf("============================================================\n");
}

void printSREG(void)
{
    SREG &= 0b00011111;

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
