# CS50 TSE Querier
## Gretchen Kerfoot, Spring 2025

## Design Spec

According to the Querier Requirements Spec, the TSE Querier is a standalone 
program that reads the page files produced by the TSE crawler and the 
index file produced by the TSE indexer, and responds to search queries 
entered on the command line. In this document we cover only the querier.

## User interface

The querier's only interface with the user is on the command-line; it must 
always have two arguments.

```
querier pageDirectory indexFilename
```
For example, if 'page-letters-depth-3' and 'index-letters-3' are outputs 
from crawler and indexer, respectively:

```
$ ./querier ../../shared/tse/output/crawler/pages-letters-depth-3 ../../shared/tse/output/indexer/index-letters-3
```

The user then inputs a query strings at the prompt "Query?", and the 
program prints results, one per line. Queries are not case-sensitive and 
can include boolean logic using 'and' and 'or'.

To exit, the user types EOF.

## Inputs and outputs

Input:
* The querier reads the index from the file provided as 'indexFilename'.
* It reads the web pages from the provided 'pageDirectory'.
* It reads queries line-by-line from standard input.

Output:
* For valid queries, it prints:
    * The cleaned, normalized query
    * A ranked list of matching document scores, docIDs, and URLs
* For invalid queries, it prints specific error messages

## Functional decomposition into modules

We anticipate the following functions and modules:

* main - parses arguments, loads the index, and enters a loop to process 
user input
* padedir - validates the pageDirectory and loads URL information
* index - loads the in-memory index from file
* prompt - prints "Query?" if stdin is a terminal
* parseWords - tokenizes and normalizes input queries
* validateQuery - validates syntax rules such as operator positions and 
invalid characters
* evaluateQuery - processes boolean logic (AND and OR precedence) using 
intersect and union logic
* rankAndPrint - ranks matching documents and prints results

## Psuedo code for logic/algorithmic flow

The querier runs as follows:

```
parse command-line arguments
validate pageDirectory using pagedir_validate
load index into memory using index_load
loop:
    prompt user
    read a line of input
    if EOF: exit cleanly
    clean and parse the query
    validate query syntax
    if invalid: print error
    else:
        evaluate query using index
        rank documents by score
        print results in descending order
free memory and exit
```

### Boolean logic evaluation (inside evaluateQuery):

```
initialize result = empty counters
loop through words:
    if word is part of AND-sequence:
        intersect counters with running AND result
    if OR encountered:
        union running AND result into final result
        reset running AND result
union final AND result into total result
return total result
```

## Major data structures

The primary data structure is the 'index', which maps words to 'counters' 
sets. Each counters set maps docIDs to a count of that word's 
occurences.

During evaluation:
* Intermediate AND results are stored in one 'counters_t*'
* Final OR aggregation is stored in another 'counters_t*'
* Ranking is performed by iterating over final counters, finding the max 
score repeatedly

## Testing Plan

### Unit testing

Test 'validateQuery' with edge cases: invalid operators, bad characters

Test 'parseWords' with space variation, case conversion, token count

Test 'evaluateQuery' on known index data

### Integration/system testing

Use 'testing.sh' to automate tests:

* Valid queries ('search', 'computational and biology', etc.)
* Invalid syntax (leading 'and', bad characters, empty line)
* OR/AND precedence logic

### Valgrind testing

Confirm no memory leaks and no invalid reads/writes.

### Argument validation testing

* 0, 1, or 3+ arguments
* bad pageDirectory (missing .crawler file)
* nonexistent or unreadable indexFilename
