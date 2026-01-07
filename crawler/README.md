# CS50 Lab 4 (TSE)
## Gretchen Kerfoot, Spring 2025

### crawler

The crawler is a standalone program that crawls internal webpages starting
from a given seed URL, up to a specified maximum depth. It saves each
downloaded webpage into a specified directory, using unique document IDs
as filenames.

### Usage

The *crawler* program, defined in crawler.c, exports the following
functions:

```c
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);
```

### Implementation

The crawlers uses a bag to manage pages that have not yet been crawled, and
a hashtable to track URLs already visited.

The seedURL is normalized and validated as internal.

The crawler fetches each webpage, saves to the pageDirectory with a 
sequential document ID, and, if the page depth is less than maxDepth, scans
for additional internal links.

Each found URL is normalized, checked against the hashtable, and, if new, 
added to the bag and hashtable for future crawling.

Memory is carefully managed, and all dynamic allocations are properly freed.
No memory leaks are reported under valgrind testing.

### Assumptions

The seedURL must be internal.

The pageDirectory must already exist and be writeable.

maxDepth must be an integer in the range [0, 10].

The pageDirectory is assumed to not contain files with purely integer names 
before crawling.

The crawler assumes that libcs50 and webpage modules behave as detailed in 
their specifications.

### Files
* 'Makefile' - compilation procedure
* '.gitignore' - ignores object files, executables, and testing output
* 'crawler.c' - the implementation
* 'testing.sh' - test driver
* 'testing.out' - result of make test &> testing.out
* 'README.md' - documentation file

### Compilation

To compile, run make from the lab4-GKerfoot directory.

### Testing

The testing.sh program tests the crawler with invalid and valid arguments 
to verify error handling, successful crawling, and proper allocation of 
memory.

To test, run make test. Output is captured in testing.out.
