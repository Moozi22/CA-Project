# ALU & EXECUTION LOGIC IMPLEMENTATION
## Member 4: Arithmetic Logic Unit (EX Stage)

---

## Overview
The ALU (Arithmetic Logic Unit) is the mathematical heart of the processor, responsible for executing all computational operations during the EX (Execution) stage of the pipeline. This implementation provides a complete ALU that supports:
- **Arithmetic operations**: ADD, SUB, MUL
- **Logical operations**: ANDI, EOR
- **Shift operations**: SLC (Shift Left Circular), SRC (Shift Right Circular)
- **Branch address calculation**: PC = PC + 1 + IMM

---

## Files Implemented

### 1. **alu.h** - Header File
Public interface defining:
- `int8_t execute(int opcode, int8_t val1, int8_t val2)` - Main ALU execution function
- `uint16_t calculateBranch(uint16_t currentPC, int8_t immediate)` - Branch address calculation
- `int testALU(void)` - Comprehensive test suite

### 2. **alu.c** - Implementation File
Contains:
- Full implementation of all ALU operations
- Proper flag updates using the registers.h interface
- 33 comprehensive unit tests covering all operations
- Edge case and overflow handling

### 3. **test_alu.c** - Standalone Test Driver
Executable test file that can be compiled and run independently.

---

## Detailed Implementation

### execute(int opcode, int8_t val1, int8_t val2)

#### Arithmetic Operations

**ADD (Opcode 0)**
- **Operation**: `val1 + val2`
- **Formula**: Result = (int)val1 + (int)val2, then truncate to 8-bit
- **Flags Updated**: C (Carry), V (Overflow), N (Negative), S (Sign), Z (Zero)
- **Example**: 
  ```
  ADD 5 + 3 = 8        (normal)
  ADD 127 + 1 = -128   (overflow → V=1, N=1)
  ADD 200 + 100 = 44   (carry → C=1)
  ```

**SUB (Opcode 1)**
- **Operation**: `val1 - val2`
- **Formula**: Result = (int)val1 - (int)val2, then truncate to 8-bit
- **Flags Updated**: V (Overflow), N (Negative), S (Sign), Z (Zero)
- **Example**: 
  ```
  SUB 10 - 3 = 7       (normal)
  SUB 3 - 10 = -7      (negative result)
  ```

**MUL (Opcode 2)**
- **Operation**: `val1 * val2`
- **Formula**: Product = (int)val1 * (int)val2, then truncate to 8-bit
- **Flags Updated**: N (Negative), Z (Zero)
- **Example**: 
  ```
  MUL 5 * 3 = 15
  MUL 20 * 20 = 144    (400 truncated to 8-bit)
  MUL -5 * 3 = -15
  ```

#### Logical Operations

**ANDI (Opcode 5)**
- **Operation**: `val1 & val2` (bitwise AND)
- **Flags Updated**: N (Negative), Z (Zero)
- **Example**: 
  ```
  ANDI 0xF0 & 0xAA = 0xA0
  ANDI 0xF0 & 0x0F = 0x00 (sets Z flag)
  ```

**EOR (Opcode 6)**
- **Operation**: `val1 ^ val2` (bitwise XOR)
- **Flags Updated**: N (Negative), Z (Zero)
- **Example**: 
  ```
  EOR 0xF0 ^ 0xAA = 0x5A
  EOR value ^ value = 0x00 (sets Z flag)
  ```

#### Shift Operations (Circular)

**SLC (Opcode 8) - Shift Left Circular**
- **Operation**: Rotate left by 1 bit
- **Formula**: `(val1 << 1) | (MSB wrap-around)`
- **Flags Updated**: N (Negative), Z (Zero)
- **Example**: 
  ```
  SLC 0b01010101 (0x55) → 0b10101010 (0xAA)
  SLC 0b10000000 (0x80) → 0b00000001 (0x01)
  ```

**SRC (Opcode 9) - Shift Right Circular**
- **Operation**: Rotate right by 1 bit
- **Formula**: `(val1 >> 1) | (LSB wrap-around << 7)`
- **Flags Updated**: N (Negative), Z (Zero)
- **Example**: 
  ```
  SRC 0b10101010 (0xAA) → 0b01010101 (0x55)
  SRC 0b00000001 (0x01) → 0b10000000 (0x80)
  ```

---

### calculateBranch(uint16_t currentPC, int8_t immediate)

Calculates the target address for branch instructions.

**Formula**: 
```
PC_new = currentPC + 1 + immediate
```

- **PC + 1**: Accounts for normal sequential instruction fetch (moves to next instruction)
- **+ immediate**: Applies the signed branch offset

**Used by**:
- BEQZ (Opcode 4): Branch if equal to zero
- BR (Opcode 7): Unconditional branch

**Examples**:
```
calculateBranch(10, 5) = 16     (forward branch: 10+1+5)
calculateBranch(20, -5) = 16    (backward branch: 20+1-5)
calculateBranch(100, 0) = 101   (no offset: 100+1+0)
calculateBranch(500, 127) = 628 (max forward: 500+1+127)
```

---

## Flag Updates

The ALU correctly updates the Status Register (SREG) flags according to specification:

| Flag | Operation | Condition |
|------|-----------|-----------|
| **C (Carry)** | ADD only | Unsigned result > 255 (added operands as unsigned) |
| **V (Overflow)** | ADD, SUB | Signed overflow occurred |
| **N (Negative)** | ADD, SUB, MUL, ANDI, EOR, SLC, SRC | Result MSB = 1 |
| **Z (Zero)** | ADD, SUB, MUL, ANDI, EOR, SLC, SRC | Result = 0 |
| **S (Sign)** | ADD, SUB | S = N XOR V (calculated after N and V) |

---

## Test Results

### Test Suite: 33 Comprehensive Tests

#### Test Group 1: Arithmetic Operations (10 tests)
- ✅ ADD basic addition
- ✅ ADD with negative operands
- ✅ ADD positive overflow (127 + 1)
- ✅ ADD negative underflow (-128 - 1)
- ✅ SUB basic subtraction
- ✅ SUB negative result
- ✅ MUL basic multiplication
- ✅ MUL with zero operand
- ✅ MUL negative operands
- ✅ MUL overflow truncation

#### Test Group 2: Logical Operations (4 tests)
- ✅ ANDI basic AND
- ✅ ANDI producing zero result
- ✅ EOR basic XOR
- ✅ EOR with identical operands (self-cancellation)

#### Test Group 3: Shift Operations (4 tests)
- ✅ SLC with 0x55 → 0xAA
- ✅ SLC with MSB set (0x80 → 0x01)
- ✅ SRC with 0xAA → 0x55
- ✅ SRC with LSB set (0x01 → 0x80)

#### Test Group 4: Branch Calculation (6 tests)
- ✅ Forward branch
- ✅ Backward branch
- ✅ Branch from start address
- ✅ Branch with zero offset
- ✅ Branch with maximum positive offset
- ✅ Branch with maximum negative offset

#### Test Group 5: Flag Updates (4 tests)
- ✅ ADD sets Carry flag on unsigned overflow
- ✅ ADD clears Z and C on normal operation
- ✅ ANDI sets Zero flag when result is 0
- ✅ SLC sets Negative flag when MSB = 1

#### Test Group 6: Edge Cases (5 tests)
- ✅ Non-ALU opcodes return 0
- ✅ ADD with zero operand
- ✅ MUL with negative operands (result positive)
- ✅ ANDI with all-ones operand
- ✅ EOR with self (commutative property)

**Result: ALL 33 TESTS PASSED ✅**

---

## Compilation & Testing

### Build Instructions

```bash
# Compile test executable
gcc -Wall -o test_alu test_alu.c alu.c registers.c -lm

# Run tests
./test_alu
```

### Expected Output
```
========================================================
  ALU & EXECUTION LOGIC TEST SUITE
  Member 4: Arithmetic Logic Unit Implementation
========================================================

Running comprehensive ALU tests...

[... 33 tests execute ...]

========================================================
  TEST SUMMARY
========================================================
  Total tests: 33
  Passed: 33
  Failed: 0

  ALL TESTS PASSED!
========================================================
```

---

## Integration Notes

The ALU is designed to integrate seamlessly into the pipeline:

1. **EX Stage Integration**:
   - Call `execute(opcode, val1, val2)` to compute results
   - Results are automatically written to destination register
   - SREG flags are automatically updated via `registers.h` functions

2. **Branch Handling**:
   - Call `calculateBranch(PC, immediate)` during BR/BEQZ decode
   - Returns new PC value for branch target

3. **Dependencies**:
   - Requires `registers.h` for flag update functions
   - Uses `int8_t` and `uint16_t` from `stdint.h`
   - Logs to console via `registers.c` logging interface

4. **Error Handling**:
   - Non-ALU opcodes safely return 0
   - Overflow/underflow handled by C's built-in signed integer arithmetic
   - All flag updates delegated to tested `registers.c` functions

---

## Implementation Quality

✅ **Correctness**: All operations verified against specification  
✅ **Robustness**: Handles overflow, underflow, edge cases  
✅ **Testing**: 33 comprehensive unit tests with 100% pass rate  
✅ **Integration**: Seamless integration with existing pipeline modules  
✅ **Documentation**: Detailed comments and specifications  
✅ **Code Quality**: Follows project conventions and style  

---

## References

- **Specification**: Package 3 - CA Project (Computer Architecture)
- **Related Modules**: 
  - Memory (memory.c/h)
  - Registers (registers.c/h)
  - Decoder (decoder.c/h)
- **Testing Framework**: Custom unit test suite (33 tests)

---

**Status**: ✅ COMPLETE AND TESTED
**Member**: Member 4 - ALU & Execution Logic
**Date**: Implementation Date
