# CS50 TSE Indexer
## Gretchen Kerfoot, Spring 2025

## Implementation Spec

In this document we reference the Requirements Specification and Design 
Specification and focus on the implementation-specific decisions. Here we 
focus on the core subset of implementation topics:

* Data structures
* Control flow: pseudo code for overall flow, and for each of the functions
* Detailed function prototypes and their parameters
* Error handling and recovery
* Testing plan

## Data structures

The core data structure used is an `index`, which maps normalized words to
counters sets. Each counters set maps docIDs to a count of that word's 
occurrences in that document.

Internally, `index` is implemented as a `hashtable` where:

* keys are `char*` words
* values are `counters_t*`

The hashtable size is set to 200, anticipating a moderate vocabulary size 
and following previous guidance.

## Control flow

The indexer system is implemented in two programs: `indexer.c` and 
`indextest.c`.

### main (indexer)

Validates arguments, then calls `index_build()` to build the index and 
`index_save()` to write it to a file.

### validateArgs

Ensures that the pageDirectory contains a .crawler marker file and that the indexFilename can be written to.

### index_build

Creates a new `index_t`, and loops over integer docIDs starting from 1, loading a `webpage_t` from each corresponding file in the pageDirectory. For 
each successfully loaded page, it calls `index_page()`. It then returns 
the constructed `index_t`.

Pseudocode:

```
create new index
for docID = 1; load page from pageDirectory/docID
    if successful
        call index_page(index, page, docID)
        delete page
    else
        break
return index
```

### index_page

Given a webpage and its correspongind docID, steps through all words in 
the page:

* Ignores words shorter than 3 characters
* Normalizes each word
* Inserts or updates the count for that word in the index for the given
docID

Pseudocode:

```
pos = 0
while (word = webpage_getNextWord(page, &pos))
    if length(word) >= 3
        normalize word
        index_insert(index, word, docID)
    free(word)
```

### main (indextest)

Validates arguments, loads index from first file using `index_load()`, 
saves it to second file using `index_save()`.

## Other modules

### index

Implements the abstract data type `index_t`, and provides:

* `index_new(int slots)`
* `index_insert(index, word, docID)`
* `index_save(index, filepath)`
* `index_load(filepath)`
* `index_delete(index)`

### pagedir

Adds two functions:

* `pagedir_validate()` - verifies .crawler file exists in pageDirectory
* `pagedir_load()` - loads a `webpage_t` from a file numbered by docID

### word

Provides function:

* `normalizeWord(char* word)` - converts to lowercase and rejects short 
strings (shorter than 3 characters)

## Function prototypes

All function prototypes are fully documented in the respective `.h` files.
Key functions include:

```c
// indexer.c
int main(const int argc, char* argv[]);
static bool validateArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
static index_t* index_build(const char* pageDirectory);
static void index_page(index_t* index, webpage_t* page, const int docID);

// indextest.c
int main(const int argc, char* argv[]);

// index.h
index_t* index_new(const int slots);
void index_insert(index_t* index, const char* word, const int docID);
void index_save(index_t* index, const char* filename);
index_t* index_load(const char* filename);
void index_delete(index_t* index);
```

## Error handling and recovery

All command-line parameters are validated before index construction begins.
Errors result in messages to stderr and a non-zero exit status.

All pointer parameters are checked for `NULL`, and defensive coding 
practices are followed.

Memory allocation failures are handled by `mem_assert()` or similar 
fail-fast mechanisms. These produce error messages and clean program exits.

The index file format is assumed to be correct during loading; as per the 
spec, minimal validation is required.

## Testing plan

### Unit testing

* `indextest` serves as a unit test for the index module by loading and 
re-saving an index file.
* Manual inspection or the provided `indexcmp` tool can also be used to 
verify equivalence.

### Regression testing

* The `letters-0` test directory from the CS50 shared output is used as a 
baseline.
* Output from `indexer` and `indextest` are compared using provided 
`indexcmp`.

### Integration/system testing

The `testing.sh` script does the following:

1. Runs `indexer` on `../crawler/output/letters-0` and creates `index1`
2. Runs `indextest` to copy `index1` into `index2`
3. Compares the files using `indexcmp`

The entire script is executed using `make test`, which runs:

```bash
bash -v testing.sh &> testing.out
```

### Memory testing

All programs were tested with valgrind.

Output:

```
All heap blocks were freed -- no leaks are possible
ERROR SUMMARY: 0 errors from 0 contexts
```

This confirms all memory is properly freed.

