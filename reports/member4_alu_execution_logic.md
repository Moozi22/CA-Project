# Member 4 Report: ALU & Execution Logic

## Role Overview
This part of the project is the execution core of the processor. It performs arithmetic, logic, shift, and branch-address calculations during the EX stage.

## What Was Implemented
The ALU is defined in [alu.h](../alu.h) and implemented in [alu.c](../alu.c).

The main changes are:
- `execute(opcode, val1, val2)` performs the actual operation for each ALU instruction.
- `calculateBranch(currentPC, immediate)` computes the branch target using `PC + 1 + IMM`.
- `testALU()` provides focused unit tests for arithmetic, logic, shift, and branch behavior.

## Operations Covered
The ALU supports the instructions needed by Package 3:
- `ADD` for addition.
- `SUB` for subtraction.
- `MUL` for multiplication.
- `ANDI` for bitwise AND.
- `EOR` for bitwise XOR.
- `SLC` for shift left circular.
- `SRC` for shift right circular.

It returns the truncated 8-bit result and calls the correct register-flag update helper after the operation.

## Example
Some clear examples are:
- `ADD 5, 3 -> 8`
- `SUB 10, 3 -> 7`
- `MUL 5, 3 -> 15`
- `ANDI 0xF0, 0x0F -> 0x00`
- `EOR 0xF0, 0xAA -> 0x5A`
- `SLC 0x55 -> 0xAA`
- `SRC 0xAA -> 0x55`

For branching, the project rule is:
- `PC = PC + 1 + IMM`

So if the current PC is `10` and the immediate is `5`, the branch target becomes `16`.

## PDF Reference
The PDF explains the branch formula clearly and notes that the `+1` part is already handled by the fetch stage. It also confirms that immediates are signed in general and that branch and jump behavior must be handled during execution.

Reference file: [CSEN601 Project Description.pdf](../CSEN601%20Project%20Description.pdf)

## Short Closing
This module turns decoded instructions into real state changes and is the main computational part of the simulator.
