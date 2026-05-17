# Member 6 Report: Hazard & System Logger

## Role Overview
This part of the project handles pipeline safety behavior and cycle-by-cycle reporting. It makes the simulation easier to trace and ensures incorrect post-branch instructions do not keep executing.

## What Was Implemented
The hazard and logging behavior is implemented in [pipeline.h](../pipeline.h) and [pipeline.c](../pipeline.c).

The main changes are:
- `HazardUnit` stores hazard-related state.
- `detectHazards()` checks for simple RAW dependency cases.
- `detectHazard()` is a wrapper for compatibility.
- `flushPipeline()` clears the IF and ID pipeline latches after a taken branch.
- `logState()` prints the stage snapshot for the current cycle.

## What the Logger Shows
Each cycle prints:
- the instruction in EX,
- the instruction in ID,
- the instruction in IF,
- the values entering the stage,
- forwarded values when needed,
- and the updated state after execution.

This matches the PDF requirement that each clock cycle must show the instruction in every stage and the values that entered and changed in that stage.

## Example
A typical hazard-related case is when one instruction produces a value and the very next instruction uses that same register.

Example:
- `ADD R1 R2`
- `SUB R3 R1`

In that situation, the logger can show the dependency and the forward path so the consumer instruction receives the correct value without breaking the pipeline flow.

For branches, when `BEQZ` or `BR` changes the PC, `flushPipeline()` drops the instructions that are no longer valid.

## PDF Reference
The PDF states that any instruction fetched while branch resolution is still happening must be dropped from the datapath, and it also requires cycle-by-cycle logging of stage contents and value changes.

Reference file: [CSEN601 Project Description.pdf](../CSEN601%20Project%20Description.pdf)

## Short Closing
This module provides the reporting and control logic that makes the pipeline output understandable and keeps the simulation behavior aligned with the assignment requirements.
