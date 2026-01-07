/*
 * querier.c    Gretchen Kerfoot    Spring 2025
 *
 * Implements the querier component of the Tiny Search Engine.
 * This program loads the inverted index built by the indexer,
 * reads search queries from stdin, and prints ranked matching documents.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "../common/index.h"
#include "../common/word.h"
#include "../common/pagedir.h"
#include "../libcs50/file.h"
#include "../libcs50/mem.h"
#include "../libcs50/counters.h"
#include "../libcs50/hashtable.h"

//function prototypes
static void prompt(void);
static void processQuery(char* line, index_t* index, const char* pageDir);
static bool validateQuery(char** words, const int wordCount);
static char** parseWords(char* line, int* wordCount);
static void normalizeWords(char** words, int wordCount);
static void freeWords(char** words, int wordCount);
static counters_t* evaluateQuery(char** words, int wordCount, index_t* index);
static counters_t* intersectCounters(counters_t* a, counters_t* b);
static counters_t* unionCounters(counters_t* a, counters_t* b);
static void rankAndPrint(counters_t* result, const char* pageDir);
static counters_t* counters_copy(counters_t* source);
static int counters_nonzero(counters_t* ctrs);

/*
 * Validates command-line arguments, loads index from file, enters a loop 
 * to process user queries from stdin
 *
 * Caller provides:
 *   argc - number of command-line arguments
 *   argv - array of command-line argument strings
 * Return:
 *   0 on normal completion, non-zero exit code on error
 */
int main(const int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: ./querier pageDirectory indexFilename\n");
    exit(1);
  }

  const char* pageDirectory = argv[1];
  const char* indexFilename = argv[2];

  if (!pagedir_validate(pageDirectory)) {
    fprintf(stderr, "Error: '%s' is not a valid crawler directory.\n", pageDirectory);
    exit(2);
  }

  index_t* index = index_load(indexFilename);
  if (index == NULL) {
    fprintf(stderr, "Error: could not load index file '%s'\n", indexFilename);
    exit(3);
  }

  char* line = NULL;
  size_t len = 0;

  while (true) {
    prompt();

    ssize_t nread = getline(&line, &len, stdin);
    if (nread == -1) {
      break;
    }

    if (line[nread - 1] == '\n') {
      line[nread - 1] = '\0';
    }

    processQuery(line, index, pageDirectory);
  }

  free(line);
  index_delete(index);
  return 0;
}


/* 
 * Prints a prompt only if stdin is coming from a terminal
 */
static void prompt(void) {
  if (isatty(fileno(stdin))) {
    printf("Query? ");
    fflush(stdout);
  }
}

/* 
 * Creates a copy of a counters object
 * 
 * Caller provides:
 *   source - the counters object to copy
 * Return:
 *   new counters object
 */
static counters_t* counters_copy(counters_t* source) {
  counters_t* copy = counters_new();
  if (source == NULL) {
    return copy;
  }
  for (int i = 1; i <= 1000; i++) {
    int val = counters_get(source, i);
    if (val > 0) {
      counters_set(copy, i, val);
    }
  }
  return copy;
}

/* 
 * Computes the intersection (AND) of two counters objects
 * For each docID in both, the score is the minimum count
 *
 * Caller provides:
 *   a - first counters object
 *   b - second counters object
 * Return:
 *   new counters object representing intersection
 */
static counters_t* intersectCounters(counters_t* a, counters_t* b) {
  counters_t* result = counters_new();
  if (a == NULL || b == NULL) {
    return result;
  }
  for (int i = 1; i <= 1000; i++) {
    int countA = counters_get(a, i);
    int countB = counters_get(b, i);
    if (countA > 0 && countB > 0) {
      counters_set(result, i, (countA < countB ? countA : countB));
    }
  }
  return result;
}

/* 
 * Computes the union (OR) of two counters objects
 * For each docID in either, the score is the sum of the counts
 *
 * Caller provides:
 *   a - first counters object
 *   b - second counters object
 * Return:
 *   new counters object representing union
 */
static counters_t* unionCounters(counters_t* a, counters_t* b) {
  counters_t* result = counters_new();
  if (a == NULL && b == NULL) {
    return result;
  }
  for (int i = 1; i <= 1000; i++) {
    int count = (a ? counters_get(a, i) : 0) + (b ? counters_get(b, i) : 0);
    if (count > 0) {
      counters_set(result, i, count);
    }
  }
  return result;
}

/*
 * Interprets AND/OR logic on the input words and combines results using
 * index
 *
 * Caller provides:
 *   words - query words (normalized and validated)
 *   wordCount - number of words
 *   index - in-memory index structure
 * Return:
 *   counters object mapping docIDs to total relevance score
 */
static counters_t* evaluateQuery(char** words, int wordCount, index_t* index) {
  counters_t* result = NULL;
  int i = 0;

  while (i < wordCount) {
    counters_t* subResult = NULL;

    while (i < wordCount && strcmp(words[i], "or") != 0) {
      if (strcmp(words[i], "and") == 0) {
        i++;
        continue;
      }

      counters_t* wordCounts = index_find(index, words[i]);
      counters_t* wordCopy = (wordCounts != NULL) ? counters_copy(wordCounts) : counters_new();

      if (subResult == NULL) {
        subResult = wordCopy;
      } else {
        counters_t* temp = intersectCounters(subResult, wordCopy);
        counters_delete(subResult);
        counters_delete(wordCopy);
        subResult = temp;
      }
      i++;
    }

    if (subResult == NULL) {
      subResult = counters_new();
    }

    if (result == NULL) {
      result = subResult;
    } else {
      counters_t* temp = unionCounters(result, subResult);
      counters_delete(result);
      counters_delete(subResult);
      result = temp;
    }

    if (i < wordCount && strcmp(words[i], "or") == 0) {
      i++;
    }
  }

  if (result == NULL) {
    result = counters_new();
  }

  return result;
}

/*
 * Checks is query syntax is valid based on position and adjacency of 
 * operators
 *
 * Caller provides:
 *   words - array of strings representing query tokens
 *   wordCount - number of tokens
 * Return:
 *   true if query is valid, false otherwise
 */
static bool validateQuery(char** words, const int wordCount) {
  for (int i = 0; i < wordCount; i++) {
    if ((strcmp(words[i], "and") == 0 || strcmp(words[i], "or") == 0)) {
      if (i == 0) {
        fprintf(stderr, "Error: '%s' cannot be first\n", words[i]);
        return false;
      }
      if (i == wordCount - 1) {
        fprintf(stderr, "Error: '%s' cannot be last\n", words[i]);
        return false;
      }
      if (strcmp(words[i + 1], "and") == 0 || strcmp(words[i + 1], "or") == 0) {
        fprintf(stderr, "Error: '%s' and '%s' cannot be adjacent\n", words[i], words[i + 1]);
        return false;
      }
    }
  }
  return true;
}

/*
 * Tokenizes the input line into lowercase alphabetic words
 * Checks for errors
 *
 * Caller provides:
 *   line - the input query string
 *   wordCount - pointer to store number of valid words
 * Return:
 *   array of allocated strings (words), or NULL if fails
 */
static char** parseWords(char* line, int* wordCount) {
  if (line == NULL || wordCount == NULL) return NULL;

  char* saveptr;
  int count = 0;
  int capacity = 10;
  char** words = malloc(capacity * sizeof(char*));

  for (char* token = strtok_r(line, " ", &saveptr); token != NULL;
       token = strtok_r(NULL, " ", &saveptr)) {

    for (char* c = token; *c != '\0'; c++) {
      if (!isalpha(*c)) {
        fprintf(stderr, "Error: bad character '%c' in query.\n", *c);
        freeWords(words, count);
        return NULL;
      }
    }

    if (count >= capacity) {
      capacity *= 2;
      words = realloc(words, capacity * sizeof(char*));
    }

    words[count++] = strdup(token);
  }

  *wordCount = count;
  return words;
}

/*
 * Converts all words to lowercase 
 *
 * Caller provides:
 *   words - array of string tokens
 *   wordCount - number of words
 */
static void normalizeWords(char** words, int wordCount) {
  for (int i = 0; i < wordCount; i++) {
    for (char* c = words[i]; *c != '\0'; c++) {
      *c = tolower(*c);
    }
  }
}

/*
 * Frees memory allocated for a list of strings
 *
 * Caller provides:
 *   words - array of strings to free
 *   wordCount - number of words
 */
static void freeWords(char** words, int wordCount) {
  for (int i = 0; i < wordCount; i++) {
    free(words[i]);
  }
  free(words);
}

/*
 * Checks if counters object contains any non-zero entries
 *
 * Caller provides:
 *   ctrs - counters object
 * Return:
 *   1 if an entry non-zero, 0 otherwise
 */
static int counters_nonzero(counters_t* ctrs) {
  for (int i = 1; i <= 1000; i++) {
    if (counters_get(ctrs, i) > 0) {
      return 1;
    }
  }
  return 0;
}

/*
 * Prints documents sorted by source, retrieves URLs from page files
 *
 * Caller provides:
 *   result - counters object mapping docIDs to relevance scores
 *   pageDir - directory of crawler page files
 */
static void rankAndPrint(counters_t* result, const char* pageDir) {
  while (1) {
    int maxDoc = 0;
    int maxScore = 0;

    for (int i = 1; i <= 1000; i++) {
      int score = counters_get(result, i);
      if (score > maxScore) {
        maxScore = score;
        maxDoc = i;
      }
    }

    if (maxDoc == 0) break;

    char* path;
    if (asprintf(&path, "%s/%d", pageDir, maxDoc) != -1) {
      FILE* fp = fopen(path, "r");
      if (fp != NULL) {
        char* url = file_readLine(fp);
        printf("score %4d doc %3d: %s\n", maxScore, maxDoc, url);
        free(url);
        fclose(fp);
      }
      free(path);
    }
    counters_set(result, maxDoc, 0);
  }
}

/*
 * Parses, validates, and evaluates a query line, then prints ranked 
 * results
 *
 * Caller provides:
 *   line - input string containing query
 *   index - loaded index from indexFilename
 *   pageDir - directory containing crawler-generated pages
 */
static void processQuery(char* line, index_t* index, const char* pageDir) {
  if (line == NULL || index == NULL || pageDir == NULL) return;
  if (line[0] == '\0') {
    printf("No documents match.\n");
    return;
  }

  int wordCount = 0;
  char** words = parseWords(line, &wordCount);
  if (words == NULL || wordCount == 0) {
    freeWords(words, wordCount);
    printf("No documents match.\n");
    return;
  }

  normalizeWords(words, wordCount);

  if (!validateQuery(words, wordCount)) {
    freeWords(words, wordCount);
    return;
  }

  printf("Query:");
  for (int i = 0; i < wordCount; i++) {
    printf(" %s", words[i]);
  }
  printf("\n");

  counters_t* result = evaluateQuery(words, wordCount, index);

  if (result == NULL || counters_nonzero(result) == 0) {
    printf("No documents match.\n");
  } else {
    rankAndPrint(result, pageDir);
  }

  counters_delete(result);
  freeWords(words, wordCount);
}
