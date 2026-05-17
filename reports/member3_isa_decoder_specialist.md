# Member 3 Report: ISA & Decoder Specialist

## Role Overview
This part of the project turns raw 16-bit instructions into structured operations that the pipeline can execute. It bridges the gap between stored binary instructions and the semantic meaning of each instruction.

## What Was Implemented
The instruction decoder is defined in [decoder.h](../decoder.h) and implemented in [decoder.c](../decoder.c).

The main changes are:
- `Instruction` was created as a structured representation of one instruction.
- `decode(uint16_t instruction)` extracts the opcode, registers, immediate value, type, and mnemonic.
- `isRFormat()` decides whether an opcode uses the R-format layout.
- `signExtend6()` converts 6-bit signed immediates into full signed integers.
- The mnemonic table maps opcodes to the 12 Package 3 instructions.

## How the Decoder Works
The decoder reads the 16-bit word as:
- bits 15 to 12 for the opcode,
- bits 11 to 6 for `R1`,
- bits 5 to 0 for `R2` or the immediate value.

For R-format instructions, both operand fields are treated as registers. For I-format instructions, the lower field is treated as an immediate, and signed values are sign-extended when needed.

This matches the project rule that parsing should not keep extra hidden fields from the text loader. The binary instruction is stored first, then decoded again during the ID stage.

## Example
Example I-format instruction:

- `MOVI R5 10`

After encoding and decoding:
- opcode = `3`
- `r1 = 5`
- `immediate = 10`
- type = `I`
- mnemonic = `MOVI`

Example signed branch-style value:

- `BEQZ R1 -2`

The immediate `-2` is stored in 6-bit form and then sign-extended back to the negative value during decode.

## PDF Reference
The PDF says that the program must be stored as binary instructions and that the decode stage must decode the instruction into all possible formats. It also explains that immediates are signed in general, with shifts being the exception.

Reference file: [CSEN601 Project Description.pdf](../CSEN601%20Project%20Description.pdf)

## Short Closing
This module makes the simulator instruction-aware and lets the pipeline work on decoded operations instead of raw bits.
