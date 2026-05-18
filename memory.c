

#include "memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


uint16_t instructionMemory[1024];
int8_t dataMemory[2048];
uint16_t PC;



void initMemory()
{
 
    memset(instructionMemory, 0, sizeof(instructionMemory));
    memset(dataMemory, 0, sizeof(dataMemory));
    PC = 0;
    printf("[Memory] Instruction memory (1024 x 16-bit) initialised to 0.\n");
    printf("[Memory] Data memory        (2048 x  8-bit) initialised to 0.\n");
    printf("[Memory] PC reset to 0.\n");
}


void storeInstruction(uint16_t address, uint16_t encodedInstruction)
{
    if (address >= 1024)
    {
        printf("[Memory] ERROR: storeInstruction - address %u is out of "
               "bounds (max 1023).\n",
               address);
        return;
    }
    instructionMemory[address] = encodedInstruction;
}


uint16_t readInstruction(uint16_t address)
{
    if (address >= 1024)
    {
        printf("[Memory] ERROR: readInstruction - address %u is out of "
               "bounds (max 1023).\n",
               address);
        return 0;
    }
    return instructionMemory[address];
}


int8_t readData(uint16_t address)
{
    if (address >= 2048)
    {
        printf("[Memory] ERROR: readData - address %u is out of "
               "bounds (max 2047).\n",
               address);
        return 0;
    }
    return dataMemory[address];
}


void writeData(uint16_t address, int8_t value)
{
    if (address >= 2048)
    {
        printf("[Memory] ERROR: writeData - address %u is out of "
               "bounds (max 2047).\n",
               address);
        return;
    }
    dataMemory[address] = value;

   
    printf("  [EX] Data Memory Updated: MEM[%u] = %d\n", address, value);
}


uint16_t encodeRFormat(uint8_t opcode, uint8_t r1, uint8_t r2)
{
    return (uint16_t)(((opcode & 0b00001111) << 12) | /* 15-12 */
                      ((r1 & 0b00111111) << 6) |      /*11-6  */
                      ((r2 & 0b00111111))             /*5-0  */
    );
}


uint16_t encodeIFormat(uint8_t opcode, uint8_t r1, int8_t imm)
{
    return (uint16_t)(((opcode & 0b00001111) << 12) |
                      ((r1 & 0b00111111) << 6) |      
                      ((imm & 0b00111111))            
    );
}


typedef struct
{
    char mnemonic[8];
    uint8_t opcode;
    char format; 
} OpcodeEntry;

static OpcodeEntry opcodeTable[12] = {
    {"ADD", 0, 'R'},
    {"SUB", 1, 'R'},
    {"MUL", 2, 'R'},
    {"MOVI", 3, 'I'},
    {"BEQZ", 4, 'I'},
    {"ANDI", 5, 'I'},
    {"EOR", 6, 'R'},
    {"BR", 7, 'R'},
    {"SLC", 8, 'I'},
    {"SRC", 9, 'I'},
    {"LDR", 10, 'I'},
    {"STR", 11, 'I'}};

/* "R5" -> 5*/
static int parseRegister(const char *token)
{
    if ((token[0] == 'R' || token[0] == 'r') && token[1] != '\0')
    {
        int num = atoi(token + 1);
        if (num >= 0 && num <= 63)
            return num;
    }
    printf("[Parser] ERROR: invalid register token '%s'\n", token);
    return 0;
}


static void printBinary16(uint16_t val)
{
    for (int b = 15; b >= 0; b--)
    {
        printf("%d", (val >> b) & 1);
        if (b == 12 || b == 6)
            printf(" "); 
    }
}

/* Returns the number of instructions successfully loaded */
int loadProgram(const char *filename)
{

    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        printf("[Parser] ERROR: Cannot open '%s'.\n", filename);
        return -1;
    }

    printf("\n[Parser] Loading program from '%s'...\n", filename);
    printf("%-6s %-20s %-8s %-18s %s\n",
           "Addr", "Assembly", "Format", "Binary (op|r1|r2/imm)", "Hex");
    printf("--------------------------------------------------------------\n");

    char line[64];
    uint16_t address = 0;

    while (fgets(line, sizeof(line), fp) != NULL)
    {

        
        line[strcspn(line, "\r\n")] = '\0';

        
        if (strlen(line) == 0)
            continue;
        if (line[0] == '#')
            continue;
        if (line[0] == '/' && line[1] == '/')
            continue;

        if (address >= 1024)
        {
            printf("[Parser] WARNING: More than 1024 instructions - "
                   "truncating at address 1023.\n");
            break;
        }

       
        char mnemonic[8] = {0};
        char arg1[8] = {0};
        char arg2[8] = {0};

        int tokenCount = sscanf(line, "%7s %7s %7s", mnemonic, arg1, arg2);

        if (tokenCount < 1)
            continue; 

        
        OpcodeEntry *entry = NULL;
        for (int i = 0; i < 12; i++)
        {
            if (strcmp(mnemonic, opcodeTable[i].mnemonic) == 0)
            {
                entry = &opcodeTable[i];
                break;
            }
        }
        if (entry == NULL)
        {
            printf("[Parser] ERROR: Unknown mnemonic '%s' on line: %s\n",
                   mnemonic, line);
            continue;
        }

        
        uint16_t encoded = 0;

        if (entry->format == 'R')
        {
           
            if (tokenCount < 3)
            {
                printf("[Parser] ERROR: R-format instruction '%s' needs "
                       "2 registers.\n",
                       mnemonic);
                continue;
            }
            int r1 = parseRegister(arg1);
            int r2 = parseRegister(arg2);
            encoded = encodeRFormat(entry->opcode,
                                    (uint8_t)r1,
                                    (uint8_t)r2);
        }
        else
        {
            
            if (tokenCount < 3)
            {
                printf("[Parser] ERROR: I-format instruction '%s' needs "
                       "a register and an immediate.\n",
                       mnemonic);
                continue;
            }
            int r1 = parseRegister(arg1);
            int imm = atoi(arg2); 
            
            if (entry->opcode == 8 || entry->opcode == 9 ||
                entry->opcode == 10 || entry->opcode == 11)
            {
                if (imm < 0 || imm > 63)
                {
                    printf("[Parser] WARNING: Immediate %d out of "
                           "unsigned 6-bit range [0,63].\n",
                           imm);
                }
            }
            else
            {
                if (imm < -32 || imm > 31)
                {
                    printf("[Parser] WARNING: Immediate %d out of signed "
                           "6-bit range [-32,31].\n",
                           imm);
                }
            }

            encoded = encodeIFormat(entry->opcode,
                                    (uint8_t)r1,
                                    (int8_t)imm);
        }

   
        storeInstruction(address, encoded);

        printf("[%4u] %-20s %-8c ", address, line,
               entry->format);
        printBinary16(encoded);
        printf("  0x%04X\n", encoded);

        address++;
    }

    fclose(fp);
    printf("--------------------------------------------------------------\n");
    printf("[Parser] %u instruction(s) loaded into instruction memory.\n\n",
           address);

    return (int)address;
}


void printInstructionMemory()
{
    printf("\n============================================================\n");
    printf("  INSTRUCTION MEMORY DUMP  (1024 x 16-bit)\n");
    printf("============================================================\n");
    printf("%-8s %-8s %-20s\n", "Address", "Hex", "Binary (op|r1|r2/imm)");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < 1024; i++)
    {
        printf("IM[%-4d]  0x%04X   ", i, instructionMemory[i]);
        printBinary16(instructionMemory[i]);
        printf("\n");
    }

    printf("============================================================\n");
}


void printDataMemory()
{
    printf("\n============================================================\n");
    printf("  DATA MEMORY DUMP  (non-zero locations only)\n");
    printf("============================================================\n");
    printf("%-10s %-8s %-12s\n", "Address", "Decimal", "Binary");
    printf("------------------------------------------------------------\n");

    int printed = 0;
    for (int i = 0; i < 2048; i++)
    {
        int8_t val = dataMemory[i];
        if (val == 0) continue;
        printf("DM[%-4d]   %-8d  ", i, val);
        for (int b = 7; b >= 0; b--)
        {
            printf("%d", (val >> b) & 1);
        }
        printf("\n");
        printed++;
    }

    if (printed == 0)
        printf("  (all 2048 locations are 0)\n");

    printf("============================================================\n");
}
