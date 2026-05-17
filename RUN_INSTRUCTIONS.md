# How to Run

## Step 1: Open the project folder

Open the `CA-Project` folder in VS Code or Command Prompt.

If you are using Command Prompt, first change into the project folder.

```cmd
cd path\to\CA-Project
```

## Step 2: Make sure the assembly file exists

The simulator reads instructions from a text file. In this project, the sample input file is [sample_program.txt](sample_program.txt).

If you want to use your own program, put the assembly instructions in a `.txt` file in the same folder.

## Step 3: Run the program

If the executable already exists, run:

```cmd
package3.exe sample_program.txt > output.txt
```

This means:

- `package3.exe` starts the simulator
- `sample_program.txt` is the assembly input file
- `>` sends all console output into `output.txt`

If you want to use your own program file, replace `sample_program.txt` with your file name.

## Step 4: Compile first if needed

If `package3.exe` has not been built yet, compile it first with:

```cmd
gcc main.c memory.c registers.c decoder.c alu.c pipeline.c -o package3.exe
package3.exe sample_program.txt > output.txt
```

The first line builds the simulator.
The second line runs it and saves the output into `output.txt`.

If you are compiling on Windows Command Prompt and the command above does not work, make sure `gcc` is installed and available in your PATH.

## Step 5: Open the HTML viewer

Open [simulator_gui.html](simulator_gui.html) in a browser.

In VS Code, you can right-click the file and choose **Open with Default Browser**.

You can also double-click the HTML file from File Explorer.

## Step 6: Load the simulator output

The HTML file does not run the CPU by itself.
It only displays the output written by the simulator.

Upload [output.txt](output.txt) into the HTML page using the file picker or by dragging the file into the page.

## Step 7: View the results

After loading the file, the HTML page shows:

- the clock cycle number
- the IF, ID, and EX pipeline stages
- register updates
- data memory updates
- the final register dump
- the final SREG and PC values
- the instruction memory dump
- the data memory dump

## Step 8: Use a different program if needed

If your program file has a different name, replace `sample_program.txt` in the command with your file name.

## Full flow summary

1. Open the project folder.
2. Compile the simulator if needed.
3. Run the executable on a `.txt` assembly file.
4. Save the console output to `output.txt`.
5. Open `simulator_gui.html`.
6. Load `output.txt` into the HTML page.