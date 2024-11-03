#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 100
#define STOP_WORD_COUNT 20
#define MIN_NGRAM_LENGTH 2
#define MAX_NGRAM_LENGTH 4
#define MAX_COMMON_NGRAMS 100
#define MAX_DISTINCT_WORDS 100

// Trie node structure, supports 27 characters (a-z + '|')
struct TrieNode
{
    struct TrieNode *children[27]; // Extra space for '|'
    int isEndOfWord;
};

// Predefined list of stop words
const char *stopWords[STOP_WORD_COUNT] = {"the", "is", "a", "an", "and", "of", "to", "in", "that", "it",
                                          "The", "Is", "A", "An", "And", "Of", "To", "In", "That", "It"};

// Function to create a new Trie node
struct TrieNode *createNode()
{
    struct TrieNode *newNode = (struct TrieNode *)malloc(sizeof(struct TrieNode));
    newNode->isEndOfWord = 0;
    for (int i = 0; i < 27; i++)
    { // 26 letters + 1 for '|'
        newNode->children[i] = NULL;
    }
    return newNode;
}

// Helper function to get the index of a character ('a' to 'z' => 0 to 25, '|' => 26)
int getCharIndex(char c)
{
    if (c == '|')
    {
        return 26; // '|' is assigned to index 26
    }
    else
    {
        return tolower(c) - 'a'; // 'a' -> 0, 'z' -> 25
    }
}

// Function to insert a phrase into the Trie
void insertPhrase(struct TrieNode *root, const char *phrase)
{
    struct TrieNode *currentNode = root;
    while (*phrase)
    {
        int index = getCharIndex(*phrase);
        if (index >= 0 && index <= 26)
        { // Allow 'a-z' and '|'
            if (currentNode->children[index] == NULL)
            {
                currentNode->children[index] = createNode();
            }
            currentNode = currentNode->children[index];
        }
        phrase++;
    }
    currentNode->isEndOfWord = 1;
}

// Function to search for a phrase in the Trie
int searchPhrase(struct TrieNode *root, const char *phrase)
{
    struct TrieNode *currentNode = root;
    while (*phrase)
    {
        int index = getCharIndex(*phrase);
        if (index >= 0 && index <= 26)
        {
            if (currentNode->children[index] == NULL)
            {
                return 0;
            }
            currentNode = currentNode->children[index];
        }
        phrase++;
    }
    return currentNode != NULL && currentNode->isEndOfWord;
}

// Function to check if a word is a stop word
int isStopWord(const char *word)
{
    for (int i = 0; i < STOP_WORD_COUNT; i++)
    {
        if (strcmp(word, stopWords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// Basic stemming function
void stemWord(char *word)
{
    int len = strlen(word);
    if (len > 4 && strcmp(&word[len - 3], "ing") == 0)
    {
        word[len - 3] = '\0';
    }
    else if (len > 3 && (strcmp(&word[len - 2], "ed") == 0 || strcmp(&word[len - 2], "ly") == 0))
    {
        word[len - 2] = '\0';
    }
    else if (len > 2 && word[len - 1] == 's')
    {
        word[len - 1] = '\0';
    }
}

// Function to preprocess text, apply stemming, and insert phrases into Trie
int insertPhrasesFromFile(struct TrieNode *root, const char *filename, int nGramLength)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error: Could not open file %s\n", filename);
        return 0;
    }

    char *phrase = (char *)malloc(MAX_WORD_LENGTH * nGramLength * sizeof(char));
    char **phraseWords = (char **)malloc(nGramLength * sizeof(char *));
    for (int i = 0; i < nGramLength; i++)
    {
        phraseWords[i] = (char *)malloc(MAX_WORD_LENGTH * sizeof(char));
    }

    char word[MAX_WORD_LENGTH];
    int wordCount = 0, phraseCount = 0;

    printf("Generating %d-grams from file: %s\n", nGramLength, filename);

    while (fscanf(file, "%s", word) != EOF)
    {
        int sentenceBoundary = 0;
        int len = strlen(word);
        if (word[len - 1] == '.' || word[len - 1] == '?' || word[len - 1] == '!')
        {
            sentenceBoundary = 1;
            word[len - 1] = '\0';
        }

        char cleanWord[MAX_WORD_LENGTH] = "";
        int j = 0;
        for (int i = 0; word[i] != '\0'; i++)
        {
            if (isalpha(word[i]))
            {
                cleanWord[j++] = tolower(word[i]);
            }
        }
        cleanWord[j] = '\0';

        if (strlen(cleanWord) > 0)
        {
            stemWord(cleanWord);
        }

        if (strlen(cleanWord) > 0 && !isStopWord(cleanWord))
        {
            strcat(cleanWord, "|"); // Add | to the end of each cleaned word.
            strcpy(phraseWords[wordCount % nGramLength], cleanWord);
            wordCount++;

            if (wordCount >= nGramLength)
            {
                phrase[0] = '\0';
                for (int k = 0; k < nGramLength; k++)
                {
                    strcat(phrase, phraseWords[(wordCount - nGramLength + k) % nGramLength]);
                }
                insertPhrase(root, phrase);
                printf("Inserted Phrase: %s\n", phrase);
                phraseCount++;
            }
        }

        if (sentenceBoundary)
        {
            wordCount = 0;
        }
    }
    fclose(file);

    for (int i = 0; i < nGramLength; i++)
    {
        free(phraseWords[i]);
    }
    free(phraseWords);
    free(phrase);

    return phraseCount;
}

// Function to traverse both Trie structures simultaneously and find common n-grams
void findCommonNgrams(struct TrieNode *root1, struct TrieNode *root2, char *currentNgram, int level, int *commonCount, char commonNgrams[MAX_COMMON_NGRAMS][MAX_WORD_LENGTH])
{
    if (root1 == NULL || root2 == NULL)
    {
        return;
    }

    // If both nodes indicate the end of a word, it's a common n-gram
    if (root1->isEndOfWord && root2->isEndOfWord)
    {
        strcpy(commonNgrams[*commonCount], currentNgram);
        (*commonCount)++;
    }

    // Traverse all possible children (a-z and '|')
    for (int i = 0; i < 27; i++)
    { // Loop through all 26 letters and '|'
        if (root1->children[i] != NULL && root2->children[i] != NULL)
        {
            // Add the corresponding character to the n-gram
            char nextChar[2] = {(i == 26) ? '|' : (char)(i + 'a'), '\0'};
            strcat(currentNgram, nextChar);

            // Recursively search further down both Tries
            findCommonNgrams(root1->children[i], root2->children[i], currentNgram, level + 1, commonCount, commonNgrams);

            // Backtrack to previous state of currentNgram
            currentNgram[level] = '\0';
        }
    }
}

// Function to extract distinct words from common n-grams
int extractDistinctWordsFromCommonNgrams(char commonNgrams[MAX_COMMON_NGRAMS][MAX_WORD_LENGTH], int commonCount, char distinctWords[MAX_DISTINCT_WORDS][MAX_WORD_LENGTH])
{
    int distinctWordCount = 0;
    for (int i = 0; i < commonCount; i++)
    {
        char *token = strtok(commonNgrams[i], "|");
        while (token != NULL)
        {
            // Check if the word is already in the distinctWords array
            int isDistinct = 1;
            for (int j = 0; j < distinctWordCount; j++)
            {
                if (strcmp(distinctWords[j], token) == 0)
                {
                    isDistinct = 0;
                    break;
                }
            }

            // If it's distinct, add it to the array
            if (isDistinct)
            {
                strcpy(distinctWords[distinctWordCount], token);
                distinctWordCount++;
            }

            token = strtok(NULL, "|");
        }
    }
    return distinctWordCount;
}

// Function to count occurrences of distinct words in a file
void countWordOccurrencesInFile(char *filename, char distinctWords[MAX_DISTINCT_WORDS][MAX_WORD_LENGTH], int distinctWordCount, int wordCounts[MAX_DISTINCT_WORDS], int *totalWords)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    char word[MAX_WORD_LENGTH];
    while (fscanf(file, "%s", word) != EOF)
    {
        int len = strlen(word);
        if (word[len - 1] == '.' || word[len - 1] == ',' || word[len - 1] == '?' || word[len - 1] == '!')
        {
            word[len - 1] = '\0'; // Remove punctuation
        }

        stemWord(word);

        if (!isStopWord(word))
        {
            (*totalWords)++; // Increment total word count

            for (int i = 0; i < distinctWordCount; i++)
            {
                if (strcmp(word, distinctWords[i]) == 0)
                {
                    wordCounts[i]++;
                }
            }
        }
    }

    fclose(file);
}

// Function to display total occurrences of distinct words
void displayTotalOccurrences(int wordCounts1[], int wordCounts2[], int distinctWordCount)
{
    int totalFile1 = 0, totalFile2 = 0;

    for (int i = 0; i < distinctWordCount; i++)
    {
        totalFile1 += wordCounts1[i];
        totalFile2 += wordCounts2[i];
    }

    printf("\nTotal occurrences of common words:\n");
    printf("File 1: %d occurrences\n", totalFile1);
    printf("File 2: %d occurrences\n", totalFile2);
}

// Function to calculate the similarity score
void calculateSimilarityScores(int wordCounts1[], int wordCounts2[], int distinctWordCount, int totalWordsFile1, int totalWordsFile2)
{
    int totalCommonWordsFile1 = 0, totalCommonWordsFile2 = 0;

    // Sum up the common word counts for each file
    for (int i = 0; i < distinctWordCount; i++)
    {
        totalCommonWordsFile1 += wordCounts1[i];
        totalCommonWordsFile2 += wordCounts2[i];
    }

    // Calculate individual similarity scores
    double similarityFile1 = (double)totalCommonWordsFile1 / totalWordsFile1;
    double similarityFile2 = (double)totalCommonWordsFile2 / totalWordsFile2;

    // Calculate combined similarity score
    double combinedSimilarity = ((similarityFile1 * totalWordsFile1) + (similarityFile2 * totalWordsFile2)) / (totalWordsFile1 + totalWordsFile2);

    // Display the results
    printf("\nSimilarity for File 1: %.2f%%\n", similarityFile1 * 100);
    printf("Similarity for File 2: %.2f%%\n", similarityFile2 * 100);
    printf("Combined Similarity: %.2f%%\n", combinedSimilarity * 100);
}

// Function to compare two text files for common n-grams and word counts
void compareTwoFiles(char *filename1, char *filename2, int minN, int maxN)
{
    struct TrieNode *root1 = createNode();
    struct TrieNode *root2 = createNode();

    char commonNgrams[MAX_COMMON_NGRAMS][MAX_WORD_LENGTH];
    int commonCount = 0;
    int totalWordsFile1 = 0, totalWordsFile2 = 0;

    for (int n = minN; n <= maxN; n++)
    {
        printf("\nGenerating %d-grams:\n", n);
        insertPhrasesFromFile(root1, filename1, n);
        insertPhrasesFromFile(root2, filename2, n);

        // Traverse both Tries and find common n-grams
        char currentNgram[MAX_WORD_LENGTH] = "";
        findCommonNgrams(root1, root2, currentNgram, 0, &commonCount, commonNgrams);
    }

    // Output common n-grams
    printf("\nCommon n-grams found:\n");
    if (commonCount > 0)
    {
        for (int i = 0; i < commonCount; i++)
        {
            printf("%s\n", commonNgrams[i]);
        }
    }
    else
    {
        printf("No common n-grams found.\n");
    }

    // Extract distinct common words
    char distinctWords[MAX_DISTINCT_WORDS][MAX_WORD_LENGTH];
    int distinctWordCount = extractDistinctWordsFromCommonNgrams(commonNgrams, commonCount, distinctWords);

    // Count occurrences of distinct common words in both files
    int wordCounts1[MAX_DISTINCT_WORDS] = {0};
    int wordCounts2[MAX_DISTINCT_WORDS] = {0};

    // Count occurrences of distinct common words in both files and track total words
    countWordOccurrencesInFile(filename1, distinctWords, distinctWordCount, wordCounts1, &totalWordsFile1);
    countWordOccurrencesInFile(filename2, distinctWords, distinctWordCount, wordCounts2, &totalWordsFile2);

    // Display word occurrences
    printf("\nWord Occurrences in File 1:\n");
    for (int i = 0; i < distinctWordCount; i++)
    {
        printf("%s: %d\n", distinctWords[i], wordCounts1[i]);
    }

    printf("\nWord Occurrences in File 2:\n");
    for (int i = 0; i < distinctWordCount; i++)
    {
        printf("%s: %d\n", distinctWords[i], wordCounts2[i]);
    }

    // Call the new function to display total occurrences
    displayTotalOccurrences(wordCounts1, wordCounts2, distinctWordCount);

    // Display total occurrences and calculate similarity scores
    calculateSimilarityScores(wordCounts1, wordCounts2, distinctWordCount, totalWordsFile1, totalWordsFile2);
}

int main()
{
    char file1[MAX_WORD_LENGTH], file2[MAX_WORD_LENGTH];
    printf("Enter the first filename (e.g., file1.txt): ");
    scanf("%s", file1);
    printf("Enter the second filename (e.g., file2.txt): ");
    scanf("%s", file2);

    compareTwoFiles(file1, file2, MIN_NGRAM_LENGTH, MAX_NGRAM_LENGTH);

    return 0;
}
