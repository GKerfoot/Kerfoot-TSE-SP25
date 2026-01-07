# CS50 Lab 4 (TSE)
## Gretchen Kerfoot, Spring 2025

### common (pagedir module)

The pagedir module provides functions to initialize a pageDirectory for
crawler output, and to save webpages into numbered files inside that
directory.

### Usage

The *pagedir* module, defined in pagedir.h and implemented in pagedir.c,
exports the following functions:

```c
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
```

### Implementation

The pagedir_init function creates a .crawler file inside the specified
pageDirectory, marking it as crawler-generated.

The pagedir_save function writes a webpage to a file inside the 
pageDirectory, named by a unique document ID. Each saved file contains the 
URL on the first line, depth on the second line, and the HTML content 
starting on the third line.

The module assumes that the provided pageDirectory is valid and writable.


### common (index module)

The index module provides a data structure for storing and retrieving
an index: a mapping from words to documents that contain them,
along with frequency counts.

### Usage

The *index* module, defined in index.h and implemented in index.c,
exports the following functions:

```c
index_t* index_new(const int slots);
void index_insert(index_t* index, const char* word, const int docID);
counters_t* index_find(index_t* index, const char* word);
void index_save(index_t* index, const char* filename);
index_t* index_load(const char* filename);
void index_delete(index_t* index);
```

### Implementation

The index is implemented as a hashtable where each key is a word
and each value is a counters set, mapping docIDs to the number
of occurrences of that word in each document.

The index_save function writes the contents of the index to a file
using a specified format. index_load reconstructs the structure from
that file. index_find is a helper function used to retrieve the counters
set for a specific word.


### common (word module)

The word module provides utilities for normalizing words before they
are stored in the index.

### Usage

The *word* module, defined in word.h and implemented in word.c,
exports the following function:

```c
void normalizeWord(char* word);
```

### Implementation

The normalizeWord function converts each character in the word
to lowercase and removes punctuation. Words shorter than three
characters may be ignored by the calling program.

### Assumptions

The pageDirectory provided must already exist and be writable.

The pageDirectory path must be a valid directory path.

Each webpage saved must be properly constructed with valid URL, depth, and 
HTML content.

The caller is responsible for providing unique document IDs for saved
pages.

Words passed into normalizeWord are non-null.

All file paths are valid and writable.

### Files
* 'Makefile' - compilation procedure
* '.gitignore' - ignores object files and unnecessary output
* 'pagedir.c', 'pagedir.h' - page directory utility functions
* 'index.c', 'index.h' - index data structure and file input/output
* 'word.c', 'word.h' - word normalization utility
* 'README.md' - documentation file

### Compilation

To compile, run make from the common directory.

### Testing

The pagedir module was tested indirectly through the testing.sh file in the 
crawler program, ensuring pages were correctly saved to files during 
crawler operations.

The index module is tested via indexer and querier programs, and verified 
using indexcmp.

The word module was indirectly tested through the indexer and querier 
programs (proper normalization confirmed).

