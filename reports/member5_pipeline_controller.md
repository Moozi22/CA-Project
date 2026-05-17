# Member 5 Report: Pipeline Controller

## Role Overview
This part of the project coordinates the 3-stage pipeline so the simulator can fetch, decode, and execute instructions across overlapping clock cycles.

## What Was Implemented
The pipeline controller is defined in [pipeline.h](../pipeline.h) and implemented in [pipeline.c](../pipeline.c).

The main changes are:
- `IF_ID_Register` and `ID_EX_Register` store the pipeline latches.
- `initPipeline()` resets the control state and prepares the simulation.
- `stageFetch()` reads the next instruction from instruction memory and increments the PC.
- `stageDecode()` decodes the instruction and collects operand values.
- `stageExecute()` runs the instruction and writes results back to registers or memory.
- `runCycle()` executes one clock cycle of the full pipeline.
- `runPipeline()` keeps cycling until the program is fetched and the pipeline drains.

## How the Pipeline Works
The simulation follows the 3-stage IF, ID, EX structure described in the PDF.

A useful way to describe the timing is:
- the first cycle mainly fetches the first instruction,
- the second cycle overlaps fetch and decode,
- the third cycle reaches execute,
- after that, the pipeline reaches steady-state overlap.

For `n` instructions, the ideal 3-stage pipeline completes in `n + 2` cycles, which is the same as `3 + ((n - 1) * 1)`.

## Example
Using the sample program:
- cycle 1 fetches `MOVI R1 5`,
- cycle 2 decodes the first instruction and fetches the second,
- cycle 3 executes the first instruction while the second is in decode,
- later cycles keep the overlap going until the pipeline is empty.

The controller also handles branch behavior by dropping instructions that should not continue after a taken branch.

## PDF Reference
The PDF requires the simulator to print which instruction is in each stage every clock cycle. It also describes how the PC is incremented during fetch and how the next instruction is fetched after the execute stage of a branch.

Reference file: [CSEN601 Project Description.pdf](../CSEN601%20Project%20Description.pdf)

## Short Closing
This module is what turns the individual components into a working pipeline simulation instead of a set of isolated functions.
