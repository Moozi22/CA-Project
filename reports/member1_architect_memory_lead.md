# Member 1 Report: Architect & Memory Lead

## Role Overview
This part of the project implements the memory system and the program loading flow for the Package 3 Harvard CPU simulator. The goal is to keep the instruction path and the data path separate, make the memories behave with the correct sizes, and print memory activity exactly when values change.

## What Was Implemented
The memory layer is defined in [memory.h](../memory.h) and implemented in [memory.c](../memory.c).

The main changes are:
- `InstructionMemory` was implemented as a 1024 x 16-bit array.
- `DataMemory` was implemented as a 2048 x 8-bit array.
- `PC` was stored as a 16-bit value and reset with memory initialization.
- `initMemory()` clears both memories and resets `PC` to 0.
- `storeInstruction(address, encodedInstruction)` writes a 16-bit encoded instruction into instruction memory.
- `readInstruction(address)` fetches the instruction used by the IF stage.
- `readData(address)` and `writeData(address, value)` handle data memory access during load/store instructions.
- `encodeRFormat()` and `encodeIFormat()` convert assembly fields into a 16-bit machine instruction.
- `loadProgram()` reads the text assembly file, parses each line, encodes it, and stores it starting at address 0.
- `printInstructionMemory()` and `printDataMemory()` print the final dumps required at the end of execution.

## Why These Changes Matter
The PDF requires the program to be read from a text file, encoded, and stored in instruction memory before simulation starts. It also says the PC starts at 0, the first instruction is stored at address 0, and the final output must include the instruction memory and data memory dumps.

The implementation matches that flow:
- the loader keeps parsing separate from execution,
- the pipeline fetches binary instructions from memory,
- the data memory only changes when `STR` writes or `LDR` reads a value.

## Example
A simple example from the sample program is:

- `MOVI R1 5`
- `MOVI R2 3`
- `STR R1 10`
- `LDR R3 10`

What happens:
- `MOVI R1 5` is encoded and stored in instruction memory at address 0.
- `STR R1 10` writes the current value of `R1` into `DataMemory[10]`.
- `LDR R3 10` reads that same value back from `DataMemory[10]`.

This is a good demonstration that instruction memory, data memory, and the PC all work together correctly.

## PDF Reference
The PDF pages that support this part are the general description pages about memory and loading behavior. The most relevant points are:
- PC starts from 0 and the first instruction is stored at address 0.
- Instructions must be encoded from the text file before execution.
- Any memory update must be printed with the new value.
- The final run must print the full instruction and data memory dumps.

Reference file: [CSEN601 Project Description.pdf](../CSEN601%20Project%20Description.pdf)

## Short Closing
This module provides the base storage layer for the simulator and makes the rest of the pipeline possible.
