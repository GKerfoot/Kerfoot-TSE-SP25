/*
 * indexer.c    Gretchen Kerfoot    Spring 2025
 *
 * This program reads files from a crawler-produced pageDirectory,
 * builds an inverted index mapping words to (docID, count) pairs,
 * and writes that index to a file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "webpage.h"
#include "pagedir.h"
#include "index.h"
#include "word.h"
#include "file.h"

//function prototypes
static index_t* indexBuild(const char* pageDirectory);
static void indexPage(webpage_t* page, const int docID, index_t* index);

int main(const int argc, char* argv[]) {
  //checks number of arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: %s pageDirectory indexFilename\n", argv[0]);
    return 1;
  }

  const char* pageDirectory = argv[1];
  const char* indexFilename = argv[2];

  //validates the pageDirectory
  if (!pagedir_validate(pageDirectory)) {
    fprintf(stderr, "Invalid pageDirectory: %s\n", pageDirectory);
    return 2;
  }

  //checks indexFilename is writable
  FILE* fp = fopen(indexFilename, "w");
  if (fp == NULL) {
    fprintf(stderr, "Cannot write to file: %s\n", indexFilename);
    return 3;
  }
  fclose(fp);  //just testing access

  //builds the index
  index_t* index = indexBuild(pageDirectory);
  if (index == NULL) {
    fprintf(stderr, "Failed to build index\n");
    return 4;
  }

  //saves the index to the given file
  if (!index_save(index, indexFilename)) {
    fprintf(stderr, "Failed to save index to file: %s\n", indexFilename);
    index_delete(index);
    return 5;
  }

  //clean up
  index_delete(index);
  return 0;
}

/* Builds an index from all pages in the given pageDirectory
 *
 * Caller provides:
 *   pageDirectory - path to a valid crawler directory
 * Returns:
 *   pointer to a fully populated index, or NULL on error
 * Notes:
 *   Starts from document ID 1 and reads until a page cannot be loaded
 */
static index_t* indexBuild(const char* pageDirectory) {
  index_t* index = index_new(500);  //reasonable starting size
  if (index == NULL) {
    return NULL;
  }

  int docID = 1;
  webpage_t* page;

  //loops through pages until one fails to load
  while ((page = pagedir_load(pageDirectory, docID)) != NULL) {
    indexPage(page, docID, index);
    webpage_delete(page);
    docID++;
  }

  return index;
}


/* Scans a webpage and adds its words to the index.
 *
 * Caller provides:
 *   page - pointer to webpage
 *   docID - integer ID for the page
 *   index - pointer to index being built
 * Notes:
 *   Ignores words shorter than 3 characters, normalizes all words
 */
static void indexPage(webpage_t* page, const int docID, index_t* index) {
  int pos = 0;
  char* word;

  //extracts each word and add to index
  while ((word = webpage_getNextWord(page, &pos)) != NULL) {
    if (strlen(word) >= 3) {
      char* norm = normalizeWord(word);
      if (norm != NULL) {
        index_insert(index, norm, docID);
        free(norm);
      }
    }
    free(word);
  }
}
