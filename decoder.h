#ifndef DECODER_H
#define DECODER_H

#include <stdint.h>

typedef struct {
    int opcode;
    int r1;
    int r2;
    int immediate;
    char type;
    char mnemonic[8];
} Instruction;

Instruction decode(uint16_t raw);

#endif