# Package 3 Checklist

Checklist based on the Package 3 PDF requirements and the current codebase.

## Done Right

- [x] Harvard architecture is implemented with separate instruction and data memories.
- [x] Instruction memory size matches the spec: 1024 x 16-bit.
- [x] Data memory size matches the spec: 2048 x 8-bit.
- [x] 64 general-purpose registers are implemented.
- [x] PC is a 16-bit special register.
- [x] SREG is implemented with the required flags: C, V, N, S, Z.
- [x] SREG reserved bits [7:5] are kept cleared.
- [x] The 12 package 3 instructions are supported: ADD, SUB, MUL, MOVI, BEQZ, ANDI, EOR, BR, SLC, SRC, LDR, STR.
- [x] Assembly text is read from a `.txt` file, parsed, encoded, and stored in instruction memory.
- [x] Instructions are decoded again in the decode stage instead of reusing parser fields.
- [x] The pipeline runs as a 3-stage IF / ID / EX flow.
- [x] Execution starts by fetching the first instruction at clock cycle 1.
- [x] The PC increments during fetch.
- [x] The pipeline runs until there are no more instructions to fetch.
- [x] Immediate values are handled as signed 2's complement values where required.
- [x] Shift immediates are treated as positive / unsigned values.
- [x] BEQZ updates PC using `PC + 1 + immediate` when the branch is taken.
- [x] BR updates PC by concatenating the two register bytes.
- [x] Taken branches flush the younger instructions in IF and ID.
- [x] Data hazards are handled with forwarding from EX to ID.
- [x] Register updates are printed when a register value changes, including R0.
- [x] Data memory updates are printed when a value is stored or updated.
- [x] Each clock cycle prints the pipeline stage activity and the values entering the stages.
- [x] Final register contents are printed at the end of execution.
- [x] Final SREG contents are printed at the end of execution.
- [x] Final PC value is printed at the end of execution.
- [x] Full instruction memory contents are printed at the end of execution.
- [x] Full data memory contents are printed at the end of execution.

## Watchouts / Not Done

- [ ] No critical package 3 functional requirement was found missing during review.
- [ ] If your instructor checks exact console wording, the messages may still need a final string-by-string comparison against the PDF examples.

## Notes

- The current codebase looks functionally complete for Package 3.
- The main remaining risk is output formatting, not core behavior.