# ALU QUICK REFERENCE GUIDE

## Overview
The ALU (Arithmetic Logic Unit) is Member 4's implementation of the mathematical operations for the processor's EX (Execution) stage.

## Quick Start

### Include the Header
```c
#include "alu.h"
```

### Main Functions

#### 1. Execute an Operation
```c
int8_t result = execute(opcode, val1, val2);
```

**Parameters:**
- `opcode`: Instruction opcode (0-11)
- `val1`: First operand (8-bit signed)
- `val2`: Second operand (8-bit signed)

**Returns:** 8-bit signed result

**Supported Opcodes:**
```
0 (ADD)   - Addition
1 (SUB)   - Subtraction
2 (MUL)   - Multiplication
5 (ANDI)  - Bitwise AND
6 (EOR)   - Bitwise XOR
8 (SLC)   - Shift Left Circular
9 (SRC)   - Shift Right Circular
3,4,7,10,11 - Return 0 (not ALU operations)
```

#### 2. Calculate Branch Address
```c
uint16_t newPC = calculateBranch(currentPC, immediate);
```

**Parameters:**
- `currentPC`: Current program counter (16-bit)
- `immediate`: Sign-extended offset (-128 to 127)

**Returns:** New PC = currentPC + 1 + immediate

---

## Operation Examples

### Arithmetic

```c
initRegisters();  // Clear all flags first

// ADD: 5 + 3 = 8
int8_t result = execute(0, 5, 3);
// Result: 8, Flags: C=0, V=0, N=0, Z=0

// SUB: 10 - 3 = 7
result = execute(1, 10, 3);
// Result: 7, Flags: V=0, N=0, Z=0

// MUL: -5 * 3 = -15
result = execute(2, -5, 3);
// Result: -15, Flags: N=1, Z=0
```

### Logical

```c
initRegisters();

// ANDI: 0xF0 & 0xAA = 0xA0
int8_t val1 = (int8_t)0xF0;
int8_t val2 = (int8_t)0xAA;
int8_t result = execute(5, val1, val2);
// Result: 0xA0, Flags: N=1, Z=0

// EOR: 0xF0 ^ 0xAA = 0x5A
result = execute(6, (int8_t)0xF0, (int8_t)0xAA);
// Result: 0x5A, Flags: N=0, Z=0
```

### Shift (Circular)

```c
initRegisters();

// SLC: Rotate left - 0x55 becomes 0xAA
int8_t result = execute(8, (int8_t)0x55, 0);
// MSB 0 wraps to LSB, result becomes 0xAA
// Binary: 01010101 → 10101010

// SRC: Rotate right - 0xAA becomes 0x55
result = execute(9, (int8_t)0xAA, 0);
// LSB 0 wraps to MSB, result becomes 0x55
// Binary: 10101010 → 01010101
```

### Branch Calculation

```c
// Forward branch from PC=10 with offset=5
// New PC = 10 + 1 + 5 = 16
uint16_t newPC = calculateBranch(10, 5);
// Returns: 16

// Backward branch from PC=20 with offset=-5
// New PC = 20 + 1 - 5 = 16
newPC = calculateBranch(20, -5);
// Returns: 16
```

---

## Flag Updates Reference

The execute() function automatically updates the Status Register (SREG) flags:

| Operation | C | V | N | S | Z |
|-----------|---|---|---|---|---|
| **ADD**   | ✅ | ✅ | ✅ | ✅ | ✅ |
| **SUB**   | ❌ | ✅ | ✅ | ✅ | ✅ |
| **MUL**   | ❌ | ❌ | ✅ | ❌ | ✅ |
| **ANDI**  | ❌ | ❌ | ✅ | ❌ | ✅ |
| **EOR**   | ❌ | ❌ | ✅ | ❌ | ✅ |
| **SLC**   | ❌ | ❌ | ✅ | ❌ | ✅ |
| **SRC**   | ❌ | ❌ | ✅ | ❌ | ✅ |

**Flag Legend:**
- **C (Carry)**: Set if unsigned result > 255 (ADD only)
- **V (Overflow)**: Set if signed overflow occurred (ADD, SUB)
- **N (Negative)**: Set if result MSB = 1
- **Z (Zero)**: Set if result = 0
- **S (Sign)**: S = N XOR V (ADD, SUB only)

---

## Common Patterns

### Conditional Branch (BEQZ - Branch if Equal to Zero)
```c
// Compute comparison result (would come from previous operation)
int8_t cmp_result = execute(0, some_value, 0);  // ADD

// If result is zero, branch
if (cmp_result == 0) {
    uint16_t target = calculateBranch(currentPC, branch_offset);
    // Jump to target address
}
```

### Unconditional Branch (BR)
```c
// Always branch to offset from current PC
uint16_t target = calculateBranch(currentPC, branch_offset);
// Jump to target address
```

### Shift Operation Chaining
```c
int8_t value = (int8_t)0xAA;
int8_t shifted = execute(8, value, 0);  // SLC
shifted = execute(9, shifted, 0);       // SRC on result
```

---

## Testing

Run the comprehensive test suite:

```bash
gcc -Wall -o test_alu test_alu.c alu.c registers.c -lm
./test_alu
```

**Expected Output:**
```
TEST SUMMARY
  Total tests: 33
  Passed: 33
  Failed: 0

  ALL TESTS PASSED!
```

---

## Common Pitfalls

❌ **Don't forget to call initRegisters() before testing**
```c
initRegisters();  // Always do this first!
int8_t result = execute(0, 5, 3);
```

❌ **Don't use unsigned values directly - cast to int8_t**
```c
// WRONG:
int8_t result = execute(5, 0xF0, 0xAA);  // May have issues

// RIGHT:
int8_t result = execute(5, (int8_t)0xF0, (int8_t)0xAA);
```

❌ **Don't forget the +1 in branch calculation**
```c
// Branch formula is always: PC + 1 + immediate
// The calculateBranch() function handles this automatically
```

---

## Performance Notes

- All operations execute in **O(1)** time (constant)
- No dynamic memory allocation
- Suitable for real-time execution in pipeline
- Branch calculation is simple arithmetic (< 1 cycle in hardware)

---

## Integration Checklist

- [x] Arithmetic operations (ADD, SUB, MUL)
- [x] Logical operations (ANDI, EOR)
- [x] Shift operations (SLC, SRC)
- [x] Branch calculation
- [x] Status register flag updates
- [x] Comprehensive testing (33 tests)
- [x] Documentation

---

## Pipeline Hazard and Logger Notes

This part is the pipeline-side support for your evaluation.

### What It Does

- Detects RAW data hazards using forwarding from EX to ID.
- Flushes younger instructions when a branch is taken.
- Logs the pipeline state every cycle so the execution can be explained step by step.

### Main Functions

- `detectHazards()` checks whether the next instruction needs a value that was just produced.
- `flushPipeline()` clears IF and ID after a taken branch so wrong-path instructions do not execute.
- `logState()` prints the EX, ID, and IF stage snapshot each cycle.
- `runCycle()` coordinates execute, decode, fetch, and logging.
- `runPipeline()` keeps running until the program is fully completed.

### What It Affects

- Prevents stale register values from being used by dependent instructions.
- Prevents wrong-path instructions from updating registers or memory after a taken branch.
- Makes the simulation traceable during evaluation because every cycle is printed.

### Short Evaluation Summary

You can say this in the evaluation:

"My part handles pipeline hazards by forwarding results from EX to ID and flushing younger instructions on a taken branch, while the system logger prints a cycle-by-cycle snapshot of IF, ID, and EX so the execution can be traced and explained."

### Related Test File

- [test_pipeline_hazards.c](test_pipeline_hazards.c)
- [PIPELINE_HAZARD_LOGGER_NOTES.md](PIPELINE_HAZARD_LOGGER_NOTES.md)

**Status: READY FOR PIPELINE INTEGRATION ✅**

---

## File Structure

```
CA-Project/
├── alu.h                    (Public interface)
├── alu.c                    (Implementation + tests)
├── test_alu.c              (Standalone test driver)
├── ALU_DOCUMENTATION.md    (Full documentation)
├── ALU_QUICK_REFERENCE.md  (This file)
├── registers.h/c           (Flag update functions)
├── memory.h/c              (Memory interface)
├── decoder.h/c             (Instruction decoding)
└── test_alu.exe            (Compiled test executable)
```

---

**Last Updated**: Implementation complete
**Member**: Member 4 - ALU & Execution Logic
