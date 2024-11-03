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
   gcc -o text_similarity text_similarity.c


