# CS50 Lab 5 (TSE)
## Gretchen Kerfoot, Spring 2025

### indexer

The indexer is a program that reads the files saved by the crawler, 
builds an index from the words found in those webpages, and writes the 
index to a file. The indextest program reads that index file back into 
memory and writes it to a second file for testing and verification.

### Usage

The *indexer* program, defined in 'indexer.h' and implemented in 
'indexer.c', exports the following functions:

```c
int main(const int argc, char* argv[]);
static bool validateArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
static index_t* indexBuild(const char* pageDirectory);
static void indexPage(index_t* index, webpage_t* page, const int docID);
```

### Implementation

The indexer uses a hashtable to map words (keys) to counters objects, 
which track document IDs and occurrence counts. Each page is read from 
the crawler-generated directory, and its HTML content is scanned word by 
word. Each word is normalized, then stored in the index.

The index is written to a file in the format (one line per word):
word docID count [docID count]...

The indextest program (indextest.c) loads this file into a new index and 
writes it out to another file for comparison. indexcmp is used to verify
that both index files are identical.

No memory leaks are reported under valgrind testing.

### Assumptions

The pageDirectory must have been created by the crawler and contain 
.crawler as its marker.

The indexFilename must be a valid, writeable file path. If the file exists, 
it will be overwritten.

Words shorter than 3 characters are ignored. All words are normalized 
using the normalizeWord function.

Assumes presence of CS50 data structures (counters, hashtable). Due to 
missing files in the libcs50 library, I used my own working versions of 
counters.c, hashtable.c, and set.c from Lab 3.

### Files
    * 'Makefile' - compilation procedure
    * '.gitignore' - ignores object files and executables
    * 'indexer.c' - implementation of indexer
    * 'indextest.c' - test program
    * 'testing.sh' - script for automated testing
    * 'testing.out' - output from make test
    * 'README.md' - this documentation file
    * 'IMPLEMENTATION.md' - implementation spec for indexer

### Compilation

To compile, run make from the indexer directory.

### Testing

The testing.sh program tests the indexer by checking for correct file 
creation, structural equivalence of index files, and proper memory 
management.

To test, run make test. Output is captured in testing.out.



