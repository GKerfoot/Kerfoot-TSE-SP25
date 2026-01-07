/* 
 * hashtable.c    Gretchen Kerfoot    Spring 2025
 *
 * This file implements the crawler component of the Tiny Search Engine.
 * The crawler starts from a given seed URL, crawls up to a specific depth,
 * and saves all fetched webpages into the given page directory.
 *
 * Functions:
 *  main - parses arguments and intiates crawling
 *  parseArgs - parses and validates command-line arguments
 *  crawl - core crawler that manages webpage fetching and parsing
 *  pageScan - scans a fetched page for internal URLs and adds unseen URLs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "webpage.h"
#include "bag.h"
#include "hashtable.h"
#include "pagedir.h"

//local function prototypes
static void parseArgs(const int argc, char *argv[], char **seedURL, char **pageDirectory, int *maxDepth);
static void crawl(char *seedURL, char *pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);


/* 
 * Parses command-line arguments and initiates crawling.
 * 
 * Caller provides:
 *   argc, argv from the command line
 * Return:
 *   exit status 0 if successful, nonzero if error
 */
int main(const int argc, char *argv[]) {
  char *seedURL = NULL;
  char *pageDirectory = NULL;
  int maxDepth = 0;

  //parses command-line arguments
  parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);

  //begins crawling process
  crawl(seedURL, pageDirectory, maxDepth);

  return 0;
}

/*
 * Parses and validates the command-line arguments.
 *
 * Caller provides:
 *   argc and argv from the command line, plus pointers to
 *   seedURL, pageDirectory, and maxDepth variables
 * Notes:
 *   Exits with error if arguments are invalid
 */
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth) {
  //checks for correct number of arguments
  if (argc != 4) {
    fprintf(stderr, "Usage: ./crawler seedURL pageDirectory maxDepth\n");
    exit(1);
  }

  //extracts arguments
  *seedURL = argv[1];
  *pageDirectory = argv[2];
  *maxDepth = atoi(argv[3]);

  //normalizes seed URL
  char* normalizedURL = normalizeURL(*seedURL);
  if (normalizedURL == NULL) {
    fprintf(stderr, "Error: could not normalize seed URL\n");
    exit(2);
  }
  
  //checks if normalized URL is internal
  if (!isInternalURL(normalizedURL)) {
    fprintf(stderr, "Error: seed URL must be internal\n");
    free(normalizedURL);
    exit(3);
  }

  //overwrites seedURL with normalized version
  *seedURL = normalizedURL;
  
  //checks that page directory is writable
  if (!pagedir_init(*pageDirectory)) {
    fprintf(stderr, "Error: could not initialize pageDirectory\n");
    free(normalizedURL);
    exit(4);
  }

  //validates that maxDepth is within range
  if (*maxDepth < 0 || *maxDepth > 10) {
    fprintf(stderr, "Error: maxDepth must be between 0 and 10\n");
    free(normalizedURL);
    exit(5);
  }
}

/*
 * Crawls webpages starting from the seedURL up to maxDepth.
 *
 * Caller provides:
 *   seedURL - a valid, normalized, internal URL
 *   pageDirectory - existing page directory to store pages
 *   maxDepth - maximum depth to crawl (non-negative integer)
 * Notes:
 *   Exits on failure to allocate memory or fetch pages
 */
static void crawl(char *seedURL, char *pageDirectory, const int maxDepth) {
  //initializes bag to manage pages to crawl
  bag_t *pagesToCrawl = bag_new();

  //initializes hashtable to record seen URLs
  hashtable_t *pagesSeen = hashtable_new(200);

  //checks initialization success
  if (pagesToCrawl == NULL || pagesSeen == NULL) {
    fprintf(stderr, "Error: unable to initialize bag or hashtable\n");
    exit(6);
  }

  //creates webpage struct for the seed URL at depth 0
  webpage_t* page = webpage_new(seedURL, 0, NULL);

  //adds seed page to bag and hashtable
  bag_insert(pagesToCrawl, page);
  hashtable_insert(pagesSeen, seedURL, "");

  webpage_t *currPage;
  int docID = 1; //document ID starts from 1

  //main crawling loop: extracts and processes each page
  while ((currPage = bag_extract(pagesToCrawl)) != NULL) {
    if (webpage_fetch(currPage)) {
      //saves fetched page to pageDirectory
      pagedir_save(currPage, pageDirectory, docID);
      docID++;

      //if depth < maxDepth, scans page for more links
      if (webpage_getDepth(currPage) < maxDepth) {
        pageScan(currPage, pagesToCrawl, pagesSeen);
      }
    }
    //done with this page: frees its memory
    webpage_delete(currPage);
  }

  //frees all allocated structures
  bag_delete(pagesToCrawl, webpage_delete);
  hashtable_delete(pagesSeen, NULL);
}

/*
 * Scans a webpage for internal URLs, adding new URLs to bag and hashtable
 *
 * Caller provides:
 *   page - a fetched webpage
 *   pagesToCrawl - bag of pages to be crawled
 *   pagesSeen - hashtable of URLs already seen
 */
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen) {
  int pos = 0;
  char *nextURL;

  //loops through all URLs found in the page
  while ((nextURL = webpage_getNextURL(page, &pos)) != NULL) {
    if (isInternalURL(nextURL)) {
      if (hashtable_insert(pagesSeen, nextURL, "")) {
        //new internal URL: creates webpage and add to bag
        webpage_t *newPage = webpage_new(nextURL, webpage_getDepth(page) + 1, NULL);
        bag_insert(pagesToCrawl, newPage);
      } else {
        //URL already seen: frees it
        free(nextURL);
      }
    } else {
      //external URL: frees it
      free(nextURL);
    }
  }
}
