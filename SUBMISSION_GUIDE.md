# Package 3 Submission Guide

Use this guide to prepare the final deliverables for Package 3.

## 1) Files to Submit

- `Project_Team_m_Video.mp4`
- `Project_Team_m_Report.pdf`
- Source code files (`.c`, `.h`, and any required `.txt` input files)

## 2) Video Requirements

Make a short video of about 3 minutes that shows the project running.

### What to show

1. Open the project folder.
2. Show the input assembly file, such as `sample_program.txt`.
3. Run the simulator.
4. Show the console output or the HTML viewer output.
5. Explain the pipeline stages and the result of the program.
6. Point out register updates, memory updates, and the final dumps.

### Simple narration script

- "This is our Package 3 Harvard pipeline simulator."
- "The program is read from a text file, parsed, encoded, and stored in instruction memory."
- "The simulator runs a 3-stage pipeline: IF, ID, and EX."
- "Here you can see the instruction being fetched, decoded, and executed."
- "When a register changes, the simulator prints the new value and the stage name."
- "When memory changes, the simulator prints the address and stored value."
- "At the end, the simulator prints the full register file, SREG, PC, instruction memory, and data memory."

### Cycle-by-cycle explanation for the sample program

Use this if you want to explain the output in detail while the HTML viewer is open on the cycle log.

#### Cycle 1

- EX is a bubble because nothing has reached Execute yet.
- ID is a bubble because nothing has reached Decode yet.
- IF fetches `MOVI R1 5` from PC 0.
- PC becomes 1.

Say: "The pipeline starts empty, so the first cycle only fetches the first instruction."

#### Cycle 2

- EX is still a bubble.
- ID decodes `MOVI R1 5` and gets immediate value 5.
- IF fetches `MOVI R2 3` from PC 1.
- PC becomes 2.

Say: "The first instruction moves to decode while the second instruction is fetched."

#### Cycle 3

- EX executes `MOVI R1 5`.
- R1 becomes 5.
- ID decodes `MOVI R2 3`.
- IF fetches `ADD R1 R2` from PC 2.

Say: "Now the first instruction really executes, so R1 is updated to 5."

#### Cycle 4

- EX executes `MOVI R2 3`.
- R2 becomes 3.
- ID decodes `ADD R1 R2` with values 5 and 3.
- IF fetches `MUL R1 R2` from PC 3.

Say: "The second move-immediate loads 3 into R2, and the add instruction now has both operands ready."

#### Cycle 5

- EX executes `ADD R1 R2`.
- The result is 8.
- R1 becomes 8.
- SREG stays normal because the result is not negative, not zero, and does not overflow.
- ID decodes `MUL R1 R2`.
- IF fetches `ANDI R1 7` from PC 4.

Say: "ADD produces 8, writes it back to R1, and the flags remain clear."

#### Cycle 6

- EX executes `MUL R1 R2`.
- 8 times 3 gives 24.
- R1 becomes 24.
- SREG updates the N and Z flags, but both stay 0.
- ID decodes `ANDI R1 7`.
- IF fetches `EOR R1 R2` from PC 5.

Say: "Multiplication produces 24, which is positive and nonzero, so the flags remain clear."

#### Cycle 7

- EX executes `ANDI R1 7`.
- 24 AND 7 gives 0.
- R1 becomes 0.
- SREG Z becomes 1 because the result is zero.
- ID decodes `EOR R1 R2`.
- IF fetches `SLC R1 1` from PC 6.

Say: "The AND instruction clears all bits, so the result is zero and the zero flag is set."

#### Cycle 8

- EX executes `EOR R1 R2`.
- 0 XOR 3 gives 3.
- R1 becomes 3.
- SREG Z clears again.
- ID decodes `SLC R1 1`.
- IF fetches `SRC R1 1` from PC 7.

Say: "XOR with 3 gives 3, so R1 changes from zero to three."

#### Cycle 9

- EX executes `SLC R1 1`.
- The value 3 shifted left circularly becomes 6.
- R1 becomes 6.
- SREG N and Z stay 0.
- ID decodes `SRC R1 1`.
- IF fetches `STR R1 10` from PC 8.

Say: "Shift left circular rotates the bits of R1, turning 3 into 6."

#### Cycle 10

- EX executes `SRC R1 1`.
- The value 6 shifted right circularly becomes 3.
- R1 becomes 3.
- ID decodes `STR R1 10`.
- IF fetches `LDR R3 10` from PC 9.

Say: "Shift right circular reverses the rotation, so the value returns to 3."

#### Cycle 11

- EX executes `STR R1 10`.
- Memory address 10 is updated to 3.
- ID decodes `LDR R3 10`.
- IF fetches `SUB R3 R2` from PC 10.

Say: "This is the memory write step. The value 3 is stored in data memory address 10."

#### Cycle 12

- EX executes `LDR R3 10`.
- R3 loads the value from memory address 10, so R3 becomes 3.
- ID decodes `SUB R3 R2`.
- IF becomes a bubble because there are no more instructions left to fetch.

Say: "The load instruction reads back the value that was just stored, proving that memory update worked."

#### Cycle 13

- EX executes `SUB R3 R2`.
- 3 minus 3 gives 0.
- R3 becomes 0.
- SREG Z becomes 1.
- ID is a bubble.
- IF is a bubble.
- The simulation ends.

Say: "The final subtraction gives zero, so the zero flag is set and the program finishes."

### Final summary to say at the end

- "The program completed in 13 clock cycles."
- "A total of 11 instructions were loaded and executed."
- "The final register values are shown in the register dump."
- "The final memory contents are shown in the memory dump."
- "The final PC is 11, and the final SREG shows the zero flag set."

### Longer webpage narration

If you want to explain everything in the webpage and make the video longer than 3 minutes, you can speak about every visible change like this:

1. Start with the cycle counter and say which cycle is selected.
2. Read the IF stage and explain which instruction was fetched and what the PC became.
3. Read the ID stage and explain the operands or immediate value that entered decode.
4. Read the EX stage and explain exactly what the instruction did.
5. If a register changed, say the register name and the new value.
6. If SREG changed, say which flag changed and why it changed.
7. If memory changed, say the address and the new stored value.
8. If data memory was loaded, say that the value was read back from the same address.
9. Repeat this for every cycle in the page until the program finishes.

### Detailed lines you can use while pointing at the webpage

- "Cycle 1: the pipeline starts empty, so EX and ID are bubbles, and IF fetches the first instruction."
- "Cycle 2: the first instruction moves to decode, and the second instruction is fetched."
- "Cycle 3: the first instruction executes, R1 changes to 5, and the next instruction is already in decode."
- "Cycle 4: R2 changes to 3, and ADD now has both operands ready."
- "Cycle 5: ADD writes 8 into R1, and the status register stays normal because the result is valid."
- "Cycle 6: MUL writes 24 into R1, and the flags still show a normal positive result."
- "Cycle 7: ANDI clears R1 to 0, so the zero flag becomes 1."
- "Cycle 8: EOR writes 3 into R1 and clears the zero flag again."
- "Cycle 9: SLC rotates 3 into 6 and updates the register file."
- "Cycle 10: SRC rotates 6 back to 3 and updates R1 again."
- "Cycle 11: STR stores 3 into data memory address 10."
- "Cycle 12: LDR reads back the stored value from memory address 10 into R3."
- "Cycle 13: SUB gives 0, so R3 becomes 0 and the zero flag is set."

### Good closing line

- "This output shows that the pipeline, register updates, status flags, and memory updates all work correctly cycle by cycle."

### Video filename

- `Project_Team_m_Video.mp4`

## 3) Report Requirements

Write the report as a PDF and include a cover page with the following information:

- Team number
- Team name
- Package number and package name
- All team members' names
- All team members' IDs
- All team members' tutorials

### Suggested cover page format

```text
CSEN601 Project Report

Team Number: [team number]
Team Name: [team name]
Package Number: 3
Package Name: Harvard CPU Simulator

Team Members:
- [Name] - [ID] - [Tutorial]
- [Name] - [ID] - [Tutorial]
- [Name] - [ID] - [Tutorial]
```

### Report filename

- `Project_Team_m_Report.pdf`

## 4) Comment Style for `.c` and `.h` Files

Keep comments short and humanized.

### Good comment style

- "Store the instruction at address 0."
- "Fetch the next instruction from instruction memory."
- "Update SREG after ADD."
- "Print the full memory dump at the end."

### Avoid

- Very long textbook-style comments
- Repeating the code line by line
- Overly formal or robotic explanations

## 5) Final Submission Checklist

- [ ] The simulator runs without errors.
- [ ] The console output shows the pipeline stages.
- [ ] Register updates are visible.
- [ ] Memory updates are visible.
- [ ] The final dumps are printed.
- [ ] The video is named correctly.
- [ ] The report is named correctly.
- [ ] The report cover page contains all required team information.
- [ ] The comments in `.c` and `.h` files are short and clear.

## 6) Recommended Final Flow

1. Run the simulator on your `.txt` program.
2. Save the output.
3. Open the HTML viewer and confirm the result.
4. Record the video while showing the full run.
5. Prepare the PDF report.
6. Check the file names before submission.