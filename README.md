# CS201_Plagiarism_Detection
Plagiarism Detection Project using Tries and Hash Tables

For now the code is just computing all the N-Grams from both the text files after excluding the stop words ("the", "is", "a", "an", etc.) and sentence boundaries like '?', '.', '!' etc., then it finds the N-Grams that are common in both the files, then all distinct words are extracted from those common N-Grams and frequency of each of those words is counted from both the text files.

Now these common words are basically the plagiarised content that we are looking for, and these frequencies along with their total sum can be used to calculate the final similarity score (plagiarism percentage) for both the files individually and for a combined similarity score also.

We now only have to come up with the best possible way to use these frequencies to calculate the similarity score.
