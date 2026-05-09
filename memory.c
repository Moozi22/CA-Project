/* =========================================================
 *  PACKAGE 3 - Architect & Memory Lead
 *  memory.c  -  Full implementation
 * =========================================================
 *
 *  Sections:
 *    1. Global variables (the actual memory arrays)
 *    2. Initialisation
 *    3. Instruction memory operations
 *    4. Data memory operations
 *    5. Instruction encoding helpers
 *    6. Program loader / parser
 *    7. End-of-simulation print functions
 * ========================================================= */

#include "memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* =========================================================
 *  SECTION 1 - GLOBAL VARIABLES
 *
 *  These are the actual hardware being simulated.
 *  All other .c files access them through the extern
 *  declarations in memory.h.
 * ========================================================= */

/* 1024 x 16-bit instruction memory */
uint16_t instructionMemory[1024];

/* 2048 x 8-bit data memory */
int8_t dataMemory[2048];

/* 16-bit Program Counter */
uint16_t PC;

/* =========================================================
 *  SECTION 2 - INITIALISATION
 * ========================================================= */

void initMemory()
{
    /*  memset fills every byte with 0.
     *  For uint16_t and int8_t arrays this correctly sets
     *  every element to 0.                                  */
    memset(instructionMemory, 0, sizeof(instructionMemory));
    memset(dataMemory, 0, sizeof(dataMemory));
    PC = 0;
    printf("[Memory] Instruction memory (1024 x 16-bit) initialised to 0.\n");
    printf("[Memory] Data memory        (2048 x  8-bit) initialised to 0.\n");
    printf("[Memory] PC reset to 0.\n");
}

/* =========================================================
 *  SECTION 3 - INSTRUCTION MEMORY OPERATIONS
 * ========================================================= */

/* --- storeInstruction ---
 *
 *  Called by loadProgram() (the parser) once per line of
 *  assembly.  Stores the already-encoded 16-bit value into
 *  the correct slot.
 *
 *  The spec says (point 7):
 *    "You should create a decimal/binary/String of 0s and 1s
 *     concatenated instructions to be stored in the
 *     instruction memory."
 *  So we store the binary-encoded integer, not raw text.   */
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

/* --- readInstruction ---
 *
 *  Called every clock cycle by the pipeline's IF stage.
 *  Returns the 16-bit encoded instruction at PC.
 *
 *  Returns 0 and prints an error on out-of-bounds access.  */
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

/* =========================================================
 *  SECTION 4 - DATA MEMORY OPERATIONS
 * ========================================================= */

/* --- readData ---
 *
 *  Used by the LDR instruction during the EX stage.
 *  Returns the signed 8-bit value stored at address.       */
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

/* --- writeData ---
 *
 *  Used by the STR instruction during the EX stage.
 *  Stores value and immediately logs the change (the spec
 *  requires printing every memory update as it happens).   */
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

    /* Mandatory spec print: "if you changed the value of a location
     * in the memory... print that this location value has changed
     * alongside the new value"                                      */
    printf("  [EX] Data Memory Updated: MEM[%u] = %d\n", address, value);
}

/* =========================================================
 *  SECTION 5 - INSTRUCTION ENCODING HELPERS
 *
 *  Package 3 instruction formats (16-bit total):
 *
 *  R-Format:  [15:12] opcode (4 bits)
 *             [11: 6] R1     (6 bits)  <- destination / first source
 *             [ 5: 0] R2     (6 bits)  <- second source
 *
 *  I-Format:  [15:12] opcode    (4 bits)
 *             [11: 6] R1        (6 bits)
 *             [ 5: 0] IMMEDIATE (6 bits, signed 2's complement)
 *
 *  With 6-bit register fields we can address R0–R63 (2^6 = 64).
 *  With a signed 6-bit immediate the range is -32 to +31.
 * ========================================================= */

/* --- encodeRFormat ---
 *
 *  Packs opcode (4 bits), r1 (6 bits), r2 (6 bits) into
 *  a single 16-bit value.
 *
 *  Bit layout:
 *    15 14 13 12 | 11 10  9  8  7  6 |  5  4  3  2  1  0
 *    [  opcode  ] [       R1        ] [       R2        ]
 *
 *  The & masks make sure we never accidentally spill bits
 *  into the wrong field even if the caller passes a large
 *  number.                                                  */
uint16_t encodeRFormat(uint8_t opcode, uint8_t r1, uint8_t r2)
{
    return (uint16_t)(((opcode & 0x0F) << 12) | /* opcode occupies bits 15-12 */
                      ((r1 & 0x3F) << 6) |      /* r1     occupies bits 11-6  */
                      ((r2 & 0x3F))             /* r2     occupies bits  5-0  */
    );
}

/* --- encodeIFormat ---
 *
 *  Same layout but the lower 6 bits hold a signed immediate.
 *  We mask with 0x3F to keep only the 6 least-significant
 *  bits, which correctly preserves the 2's complement
 *  representation for negative numbers.
 *
 *  Example: imm = -1
 *    -1 as int8_t  = 0b11111111
 *    & 0x3F        = 0b00111111  (= 63 unsigned = -1 in 6-bit 2's complement)
 *  This is what we want stored.                             */
uint16_t encodeIFormat(uint8_t opcode, uint8_t r1, int8_t imm)
{
    return (uint16_t)(((opcode & 0x0F) << 12) | /* opcode occupies bits 15-12 */
                      ((r1 & 0x3F) << 6) |      /* r1     occupies bits 11-6  */
                      ((imm & 0x3F))            /* imm    occupies bits  5-0  */
    );
}

/* =========================================================
 *  SECTION 6 - PROGRAM LOADER / PARSER
 *
 *  Reads the assembly text file line by line.
 *  For each line it:
 *    1. Identifies the mnemonic and its opcode / format.
 *    2. Parses the register and immediate operands.
 *    3. Encodes the instruction to 16 bits.
 *    4. Stores it in instructionMemory[address++].
 *
 *  The spec (point 7) says we must NOT keep track of fields
 *  from the parsing stage during execution - we just store
 *  the binary and re-decode it fresh in the ID stage.
 *
 *  Package 3 instruction set (opcodes 0–11 in order):
 *    0  ADD   R  ADD  R1 R2      R1 = R1 + R2
 *    1  SUB   R  SUB  R1 R2      R1 = R1 - R2
 *    2  MUL   R  MUL  R1 R2      R1 = R1 * R2
 *    3  MOVI  I  MOVI R1 IMM     R1 = IMM
 *    4  BEQZ  I  BEQZ R1 IMM     if R1==0: PC=PC+1+IMM
 *    5  ANDI  I  ANDI R1 IMM     R1 = R1 & IMM
 *    6  EOR   R  EOR  R1 R2      R1 = R1 XOR R2
 *    7  BR    R  BR   R1 R2      PC = R1 || R2
 *    8  SLC   I  SLC  R1 IMM     R1 = R1 << IMM | R1 >> (8-IMM)
 *    9  SRC   I  SRC  R1 IMM     R1 = R1 >> IMM | R1 << (8-IMM)
 *   10  LDR   I  LDR  R1 ADDR    R1 = MEM[ADDR]
 *   11  STR   I  STR  R1 ADDR    MEM[ADDR] = R1
 * ========================================================= */

/* Internal lookup table used only by the parser */
typedef struct
{
    char mnemonic[8];
    uint8_t opcode;
    char format; /* 'R' or 'I' */
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

/* Helper: parse "R5" -> 5, "R0" -> 0, etc. */
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

/* Helper: print a uint16_t as a 16-character binary string */
static void printBinary16(uint16_t val)
{
    for (int b = 15; b >= 0; b--)
    {
        printf("%d", (val >> b) & 1);
        if (b == 12 || b == 6)
            printf(" "); /* visual separator per field */
    }
}

/* --- loadProgram ---
 *
 *  Main parser entry point.
 *  Returns the number of instructions successfully loaded,
 *  or -1 if the file could not be opened.                  */
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

        /* Strip trailing newline / carriage return */
        line[strcspn(line, "\r\n")] = '\0';

        /* Skip blank lines and comment lines starting with '#' or '//' */
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

        /* ---- Tokenise the line ---- */
        char mnemonic[8] = {0};
        char arg1[8] = {0};
        char arg2[8] = {0};

        int tokenCount = sscanf(line, "%7s %7s %7s", mnemonic, arg1, arg2);

        if (tokenCount < 1)
            continue; /* empty / whitespace-only line */

        /* ---- Look up the mnemonic ---- */
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

        /* ---- Encode the instruction ---- */
        uint16_t encoded = 0;

        if (entry->format == 'R')
        {
            /*  R-Format needs two register operands  */
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
            /*  I-Format needs one register + one immediate  */
            if (tokenCount < 3)
            {
                printf("[Parser] ERROR: I-format instruction '%s' needs "
                       "a register and an immediate.\n",
                       mnemonic);
                continue;
            }
            int r1 = parseRegister(arg1);
            int imm = atoi(arg2); /* signed decimal */

            /* Validate 6-bit signed range: -32 to 31
             * Exception: shift immediates are always positive (spec point 4) */
            if (entry->opcode == 8 || entry->opcode == 9)
            { /* SLC, SRC */
                if (imm < 0 || imm > 63)
                {
                    printf("[Parser] WARNING: Shift immediate %d out of "
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

        /* ---- Store and log ---- */
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

/* =========================================================
 *  SECTION 7 - END-OF-SIMULATION PRINT FUNCTIONS
 *
 *  The spec says:
 *    "At the end of your program, you need to print...
 *     the full instruction and data memory locations."
 * ========================================================= */

/* --- printInstructionMemory ---
 *
 *  Prints every instruction memory slot that is non-zero,
 *  showing address, hex value, and binary breakdown.       */
void printInstructionMemory()
{
    printf("\n============================================================\n");
    printf("  INSTRUCTION MEMORY DUMP  (1024 x 16-bit)\n");
    printf("============================================================\n");
    printf("%-8s %-8s %-20s\n", "Address", "Hex", "Binary (op|r1|r2/imm)");
    printf("------------------------------------------------------------\n");

    int printed = 0;
    for (int i = 0; i < 1024; i++)
    {
        if (instructionMemory[i] != 0)
        {
            printf("IM[%-4d]  0x%04X   ", i, instructionMemory[i]);
            printBinary16(instructionMemory[i]);
            printf("\n");
            printed++;
        }
    }

    if (printed == 0)
    {
        printf("  (all slots are 0)\n");
    }

    printf("============================================================\n");
}

/* --- printDataMemory ---
 *
 *  Prints every data memory slot.
 *  Prints ALL 2048 slots as the spec requests the "full"
 *  content - non-zero slots are marked for easy reading.   */
void printDataMemory()
{
    printf("\n============================================================\n");
    printf("  DATA MEMORY DUMP  (2048 x 8-bit)\n");
    printf("============================================================\n");
    printf("%-10s %-8s %-12s\n", "Address", "Decimal", "Binary");
    printf("------------------------------------------------------------\n");

    int printed = 0;
    for (int i = 0; i < 2048; i++)
    {
        if (dataMemory[i] != 0)
        {
            /* Print binary for 8-bit value */
            int8_t val = dataMemory[i];
            printf("DM[%-4d]   %-8d  ", i, val);
            for (int b = 7; b >= 0; b--)
            {
                printf("%d", (val >> b) & 1);
            }
            printf("\n");
            printed++;
        }
    }

    if (printed == 0)
    {
        printf("  (all slots are 0)\n");
    }

    printf("============================================================\n");
}
