# CS50 TSE Querier
## Gretchen Kerfoot, Spring 2025

## Implementation Spec

In this document we reference the Requirements Specification and Design 
Specification and focus on the implementation-specific decisions. Here 
we focus on the core subset of implementation topics:

* Data structures
* Control flow: pseudo code for overall flow, and for each of the functions
* Detailed function prototypes and their parameters
* Error handling and recovery
* Testing plan

## Data structures

The core data structure use is an 'index', which maps normalized words to 
counters sets. Each counters set maps docIDs to a count of that word's 
occurrences in that document.

Internally, 'index' is implemented using a 'hashtable' where:

* keys are 'char*' words
* values are 'counters_t*'

During query evaluation, counters are intersected or unioned to compute 
document scores. These results are stored in temporary 'counters_t*'
objects, which are iterated and ranked.

## Control flow

The querier system is implemented in a single program: 'querier.c'

### main (querier)

Validates arguments, loads the index, and repeatedly prompts the user for 
input. Each input line is parsed, validated, and either evaluated or 
rejected.

### prompt

Displays "Query?" if the input is from an interactive terminal.

### parseWords

Cleans and tokenizes input. Converts all words to lowercase, checks for 
bad characters, and allocates a 'char*' array of tokens.

### validateQuery

Checks tokenized query for syntax correctness:

* no leading/trailing operators
* no adjacent operators
* only alphabetic words and spaces allowed

### evaluateQuery

Iterates over tokenized query, computing intermediate AND and OR groups 
using the following logic:

```
initialize result = NULL
while (more tokens):
    compute andResult = intersection of counters for words in AND group
    result = union(result, andResult)
```

### intersectCounters/unionCounters

Used to combine sets of counters during query evaluation. For AND, takes 
minimum values; for OR, takes the sum.

### rankAndPrint

Ranks documents in the final result countres and prints in descending 
order of score. Reads the URL from the page directory file.

## Function prototypes

All function prototypes are documented in querier.c. Key functions include: 

```
int main(const int argc, char* argv[]);
static void prompt(void);
static char** parseWords(char* line, int* wordCount);
static bool validateQuery(char** words, const int wordCount);
static counters_t* evaluateQuery(char** words, int wordCount, index_t* index);
static void rankAndPrint(counters_t* result, const char* pageDir);
```

## Error handling and recovery

* All command-line parameters are validated before continuing
* Invalid queries are detected and reported with appropriate error 
messages
* Word parsing fails gracefully on memory or format issues
* Memory allocation is checked and freed properly
* The index and pageDirectory are assumed to match and be correctly 
formatted as per spec

## Testing plan

### Unit Testing

Functions such as 'validateQuery', 'parseWords', 'evaluateQuery', and 
'rankAndPrint' were tested individually via hand-written queries and test 
cases.

### Regression testing

'testing.sh' script performs repeated runs with example queries:
* Valid queries like 'search', 'computational and biology', 'playground'
* Invalid input (non-alphabetic characters, bad syntax)

Test data used: 'page-letters-depth-3' and 'index-letters-3' from the 
shared TSE output directory.

### Integration/system testing

'make test' runs using 'testing.sh':

```
bash -v testing.sh &> testing.out
```

### Memory testing

All runs confirmed clean via valgrind:
```
HEAP SUMMARY:
in use at exit: 0 bytes in 0 blocks
total heap usage: 682 allocs, 682 frees, 120,727 bytes allocated

All heap blocks were freed -- no leaks are possible
```
This confirms all memory is properly managed and no invalid reads/writes 
occur.


