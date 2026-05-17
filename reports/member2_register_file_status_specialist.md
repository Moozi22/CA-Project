# Member 2 Report: Register File & Status Specialist

## Role Overview
This part of the project manages the 64 general-purpose registers and the status register. It also contains the flag update rules used by arithmetic and logical instructions.

## What Was Implemented
The register system is defined in [registers.h](../registers.h) and implemented in [registers.c](../registers.c).

The main changes are:
- `GPR[64]` was implemented to represent `R0` through `R63`.
- `SREG` was implemented as an 8-bit status register with the active flags in the low 5 bits.
- `initRegisters()` clears all registers and resets `SREG`.
- `getRegister(regNum)` returns the value of a register with bounds checking.
- `setRegister(regNum, value, stageName)` updates a register and prints the required stage-aware log line.
- `updateCarryFlag()`, `updateOverflowFlag()`, `updateNegativeFlag()`, `updateSignFlag()`, and `updateZeroFlag()` update one flag at a time.
- `updateFlagsADD()`, `updateFlagsSUB()`, and `updateFlagsNZ()` bundle the correct flag updates for each instruction type.
- `printRegisters()` and `printSREG()` produce the final dumps at the end of the run.

## Why These Changes Matter
The PDF requires that any register update must be printed with the new value and the stage name. It also requires the final register file and SREG values to be printed at the end of the program.

The code follows that rule by logging updates directly from `setRegister()`. The flag update logic is also isolated so that only the instructions allowed to change a flag actually do so.

## Example
A small example is an `ADD` instruction such as:

- `MOVI R1 5`
- `MOVI R2 3`
- `ADD R1 R2`

What happens:
- `R1` becomes `5`.
- `R2` becomes `3`.
- `ADD` writes `8` back into `R1`.
- `updateFlagsADD()` updates `C`, `V`, `N`, `S`, and `Z` as required.

Another example is a zero result:

- `SUB R3 R3`

This clears the result to zero and sets the zero flag in `SREG`.

## PDF Reference
The PDF points out that register changes must be reported during execution and that the final output must include all registers and the status register. The carry-flag discussion in the general description is also relevant to the flag logic here.

Reference file: [CSEN601 Project Description.pdf](../CSEN601%20Project%20Description.pdf)

## Short Closing
This module is responsible for keeping the processor state visible, correct, and easy to debug cycle by cycle.
