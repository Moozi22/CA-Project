# Package 3 Checklist

Checklist based on the two PDF specs in this repo and the current codebase.

## Implemented

- [x] Harvard architecture with separate instruction and data memories.
- [x] Instruction memory size matches the spec: 1024 x 16-bit.
- [x] Data memory size matches the spec: 2048 x 8-bit.
- [x] 64 general-purpose registers are implemented.
- [x] PC is implemented as a 16-bit register.
- [x] SREG is implemented with the required flags: C, V, N, S, Z.
- [x] SREG reserved bits [7:5] are kept cleared.
- [x] ADD, SUB, MUL, MOVI, BEQZ, ANDI, EOR, BR, SLC, SRC, LDR, and STR are all supported.
- [x] Assembly text is parsed, encoded, and stored in instruction memory.
- [x] Instructions are decoded again during the pipeline decode stage.
- [x] The pipeline runs as a 3-stage IF / ID / EX flow.
- [x] PC increments during fetch.
- [x] BEQZ updates PC using PC + 1 + immediate when the branch is taken.
- [x] BR updates PC from the concatenation of the two register bytes.
- [x] Taken branches flush the younger instructions in IF and ID.
- [x] Data hazards are handled through forwarding from EX to ID.
- [x] Register updates are logged with the stage name.
- [x] Data memory updates are logged with the stage name.
- [x] Final register dump is printed at the end of execution.
- [x] Final SREG dump is printed at the end of execution.
- [x] Final PC value is printed at the end of execution.

## Partially Implemented / Needs Review

- [x] Full instruction and data memory dumps now print every slot, including zeros.
- [x] LDR / STR immediates are handled as unsigned 6-bit fields instead of being sign-extended.

## Not Found

- [ ] No additional package-3 requirement was clearly missing from the code path after checking the PDF specs.