/*
 * indextest.c    Gretchen Kerfoot    Spring 2025
 *
 * This program tests the index module by loading an index file,
 * then saving it to a new output file. It ensures that reading and writing
 * the index are consistent and memory-safe.
 */

#include <stdio.h>
#include <stdlib.h>
#include "index.h"


int main(const int argc, char* argv[]) {
  //checks number of arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: %s oldIndexFilename newIndexFilename\n", argv[0]);
    return 1;
  }

  const char* oldIndexFilename = argv[1];
  const char* newIndexFilename = argv[2];

  //loads the old index from file
  index_t* index = index_load(oldIndexFilename);
  if (index == NULL) {
    fprintf(stderr, "Failed to load index from file: %s\n", oldIndexFilename);
    return 2;
  }

  //saves the index to new file
  if (!index_save(index, newIndexFilename)) {
    fprintf(stderr, "Failed to save index to file: %s\n", newIndexFilename);
    index_delete(index);
    return 3;
  }

  //cleans up
  index_delete(index);
  return 0;
}
