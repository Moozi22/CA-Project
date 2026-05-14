# Pipeline Hazard and System Logger Notes

This document explains the part of the project that handles hazards and cycle logging in the pipeline.

## 1. What This Part Does

This module is responsible for two things:

- Detecting and resolving data hazards through forwarding.
- Logging the pipeline state every clock cycle so the execution can be explained and evaluated.

It works inside the 3-stage pipeline implemented in [pipeline.c](pipeline.c).

## 2. Hazard Handling

### Main Idea

When one instruction depends on the result of a previous instruction, the newer instruction must not use an old register value. In this project, the pipeline handles that through forwarding.

### Code Path

The hazard logic is implemented in `detectHazards()` and used together with the pipeline stages:

- `stageExecute()` computes the result of the current instruction.
- If that instruction writes to a register, the result is stored in `forwardValue` and the destination register is stored in `forwardReg`.
- `stageDecode()` checks whether the next instruction reads the same register.
- If it does, the new value is forwarded directly into the decode stage.

### What It Affects

This reduces stalls and prevents the decode stage from reading stale data.

Without forwarding, a dependent instruction could read the old register value before the previous instruction finishes updating it.

### Example

If the program does this:

```c
MOVI R1 5
ADD R1 R1
ADD R2 R1
```

then the second and third instructions depend on the updated value of `R1`.
The hazard unit makes sure the newer instruction sees the forwarded result instead of the old register contents.

### Branch-Related Flush

When a branch is taken, `flushPipeline()` clears the IF and ID pipeline registers.

That means:

- Instructions fetched after the branch are dropped.
- Wrong-path instructions do not write to registers or memory.
- Fetch restarts from the new PC.

This is the control-hazard side of the pipeline logic.

## 3. System Logger

### Main Idea

The logger prints the state of the pipeline every cycle so the execution can be traced step by step.

The logging function is `logState()`.

### What It Prints

Each cycle, the logger shows:

- The EX stage instruction and the values it used.
- The ID stage instruction and its decoded operands.
- The IF stage instruction currently waiting in the fetch/decode path.
- The active forwarding path, if one exists.

### Why It Matters

This is important for evaluation because it proves:

- Which instruction is in each stage.
- What values entered each stage.
- When forwarding was used.
- Whether a branch flush removed younger instructions.

It also helps explain why a register or memory value changed in a specific cycle.

## 4. Functions to Mention in Evaluation

If you need to explain the code during evaluation, the most important functions are:

- `detectHazards()` - checks for RAW dependencies.
- `flushPipeline()` - clears wrong-path instructions after a taken branch.
- `logState()` - prints the cycle snapshot.
- `runCycle()` - coordinates execute, decode, fetch, and logging.
- `runPipeline()` - loops until the program is fully completed.

## 5. Short Evaluation Summary

You can describe this part in one sentence like this:

"My part handles pipeline hazards by forwarding results from EX to ID and flushing younger instructions on a taken branch, while the system logger prints a full cycle-by-cycle snapshot of IF, ID, and EX so the execution can be traced and evaluated."

## 6. Related Test File

The hazard behavior is covered by [test_pipeline_hazards.c](test_pipeline_hazards.c).

That test checks:

- A dependent instruction chain using forwarding.
- A taken branch that flushes the younger instructions.
