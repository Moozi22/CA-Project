#include "decoder.h"
#include <string.h>

static int isRFormat(int opcode) {
    return opcode==0 || opcode==1 || opcode==2
        || opcode==6 || opcode==7;
}

static int signExtend6(int value) {
    if (value & 0b100000)
        return value | 0b11111111111111111111111111000000;
    return value;
}

Instruction decode(uint16_t instruction) {
    Instruction inst;

    inst.opcode    = (instruction >> 12) & 0b1111;
    inst.r1        = (instruction >> 6)  & 0b111111;
    inst.r2        = instruction         & 0b111111;
    inst.immediate = instruction         & 0b111111;

    if (isRFormat(inst.opcode)) {
        inst.type = 'R';
        inst.immediate = 0;
    } else {
        inst.type = 'I';
        inst.r2 = 0;
        if (inst.opcode != 8 && inst.opcode != 9)
            inst.immediate = signExtend6(inst.immediate);
    }

    char *names[] = {"ADD","SUB","MUL","MOVI","BEQZ",
                         "ANDI","EOR","BR","SLC","SRC","LDR","STR"};
    strcpy(inst.mnemonic, names[inst.opcode]);

    return inst;
}