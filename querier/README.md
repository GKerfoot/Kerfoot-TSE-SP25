# CS50 TSE Querier
## Gretchen Kerfoot, Spring 2025

## querier

The querier is a program that reads the files produced by the crawler and 
indexer, parses queries entered by the user, and prints ranked search 
results. It supports boolean logic with and/or operators and proper 
precedence. Queries are entered interactively and evaluated using the 
in-memory index.

All subsets of functionality are implemented.

### Usage

The *indexer* program, implemented in 'querier.c', exports the following 
functions:

```
int main(const int argc, char* argv[]);
static void processQuery(char* line, index_t* index, const char* pageDir);
static char** parseWords(char* line, int* wordCount);
static bool validateQuery(char** words, const int wordCount);
static counters_t* evaluateQuery(char** words, int wordCount, index_t* index);
static void rankAndPrint(counters_t* ctrs, const char* pageDir);
```

### Implementation

The querier uses an 'index_t*' to map words to countres, where each 
counter maps docIDs to occurrence counts. Each query line is parsed and 
normalized; invalid syntax (i.e. operators at the start or end) is 
rejected. Valid queries are evaluated in two phases:

1. 'and' terms are intersected (minimum of counts)
2. those results are unioned across 'or' boundaries (sum of counts)

The result is a counters set scored by relevance. Document scores are 
ranked and printed with their corresponding URLs, fetched from the 
crawled pageDirectory.

Queries are read interactively until EOF. The program handles spaces, 
normalization, and invalid input gracefully.

Valgrind confirms no memory leaks or access violations.

### Assumptions

The pageDirectory must have been produced by the crawler and contain a 
'.crawler' marker.

The index file must be correctly formatted as produced by the indexer.

Words shorter than 3 characters are ignored.

Queries must only include alphabetic words and operators 'and' or 'or'.

### Files

* 'Makefile' - compilation procedure
* '.gitignore' - ignores object files and executables
* 'querier.c' - implementation of querier
* 'testing.sh' - script for automated testing
* 'testing.out' - output from make test
* 'README.md' - this documentation file
* 'IMPLEMENTATION.md' - implementation spec for querier
* 'DESIGN.md' - design spec for querier

### Compilation

To compile, run make from the querier directory.

### Testing

The testing.sh program tests the querier by checking results from valid 
and  invalid queries, testing proper operator precedence, and ensuring 
proper memory management.

To test, run make test. Output is captured in testing.out.



