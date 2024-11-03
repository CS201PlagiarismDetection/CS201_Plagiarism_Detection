# CS201_Plagiarism_Detection

This program calculates the similarity between two text files by analyzing shared word sequences (n-grams) and identifying distinct common words. It outputs similarity scores indicating how much content overlaps between the two files.

## Features
- Supports 2-gram, 3-gram, and 4-gram text sequence comparisons.
- Removes common stop words and applies basic stemming.
- Displays similarity scores for each file individually and a combined similarity score.

## How to Run

### Prerequisites
- **C compiler** (such as GCC) must be installed on your system.

### Compilation
1. Download or clone the code files to your local machine.
2. Compile the code:
   ```bash
   gcc -o main main.c

## Execution
1. Place two plain text files (.txt) you want to compare in the same directory as the compiled program.
2. Run the program and enter the filenames when prompted

## Input Format
- After starting the program, you will be asked to input the filenames of the two text files to compare.
- Example prompt and response:
  ```bash
  Enter the first filename (e.g., file1.txt): file1.txt
  Enter the second filename (e.g., file2.txt): file2.txt



