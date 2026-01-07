/*
 * index.c    Gretchen Kerfoot    Spring 2025
 *
 * This module implements the index type for the TSE.
 * It supports creating a new index, inserting words and docIDs, 
 * saving an index to a file, loading it from a file, deleting it, and 
 * looking up the counters for a given word in the index.
 *
 * Each word maps to a counters_t structure, which maps docIDs to counts.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"
#include "hashtable.h"
#include "counters.h"
#include "file.h"
#include "memory.h"

//helper function prototypes
void index_save_helper(void* fp, const char* word, void* item);
void index_counter_print(void* fp, const int docID, const int count);

//private type for the index
typedef struct index {
  hashtable_t* table;  //maps word (char*) -> counters_t*
} index_t;


/* 
 * Creates a new index with the given number of slots.
 *
 * Caller provides:
 *   numSlots - number of slots in hashtable
 * Returns:
 *   pointer to new index, or NULL if error
 */
index_t* index_new(const int numSlots) {
  index_t* index = malloc(sizeof(index_t));
  if (index == NULL) {
    return NULL;  //out of memory
  }

  index->table = hashtable_new(numSlots);
  if (index->table == NULL) {
    free(index);
    return NULL;
  }

  return index;
}

/*
 * Inserts word for a given docID into the index.
 * If word is new, allocates a new counters set.
 * If docID exists, increments its count.
 *
 * Returns:
 *   true if success, false if error
 */
bool index_insert(index_t* index, const char* word, const int docID) {
  if (index == NULL || word == NULL || docID <= 0) {
    return false;
  }

  counters_t* ctrs = hashtable_find(index->table, word);

  if (ctrs == NULL) {
    //word not in index yet--allocates new counters set
    ctrs = counters_new();
    if (ctrs == NULL) {
      return false; //memory error
    }

    if (!hashtable_insert(index->table, word, ctrs)) {
      counters_delete(ctrs); //cleanup
      return false;
    }
  }

  counters_add(ctrs, docID);
  return true;
}


/* 
 * Saves the index to a file in the required format.
 *
 * Returns:
 *   true if successful, false if error
 */
bool index_save(index_t* index, const char* filename) {
  if (index == NULL || filename == NULL) {
    return false;
  }

  FILE* fp = fopen(filename, "w");
  if (fp == NULL) {
    return false;
  }

  hashtable_iterate(index->table, fp, index_save_helper);
  fclose(fp);
  return true;
}


/* 
 * HELPER FUNCTION
 * Called for each word in the hashtable.
 * Prints the word, then its docID/count pairs from counters.
 */
void index_save_helper(void* fp, const char* word, void* item) {
  if (fp == NULL || word == NULL || item == NULL) {
    return;
  }

  fprintf((FILE*)fp, "%s", word);
  counters_t* ctrs = item;

  counters_iterate(ctrs, fp, index_counter_print);
  fprintf((FILE*)fp, "\n");
}


/* 
 * HELPER FUNCTION
 * Called for each docID/count in a counters set.
 * Prints " docID count".
 */
void index_counter_print(void* fp, const int docID, const int count) {
  fprintf((FILE*)fp, " %d %d", docID, count);
}


/* 
 * Loads an index from a file, making assumption that file format is correct
 *
 * Returns:
 *   pointer to index if successful, or NULL if error
 */
index_t* index_load(const char* filename) {
  if (filename == NULL) {
    return NULL;
  }

  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    return NULL;
  }

  index_t* index = index_new(500); //reasonable default size
  if (index == NULL) {
    fclose(fp);
    return NULL;
  }

  char* word;
  while ((word = file_readWord(fp)) != NULL) {
    counters_t* ctrs = counters_new();
    if (ctrs == NULL) {
      free(word);
      continue;
    }

    int docID, count;
    while (fscanf(fp, "%d %d", &docID, &count) == 2) {
      counters_set(ctrs, docID, count);
    }

    hashtable_insert(index->table, word, ctrs);
    free(word);
  }

  fclose(fp);
  return index;
}


/* 
 * Frees all memory used by the index.
 */
void index_delete(index_t* index) {
  if (index == NULL) return;

  hashtable_delete(index->table, (void (*)(void*)) counters_delete);
  free(index);
}

/*
 * Looks up the counters for a given word in the index
 *
 * Returns:
 *  counters_t associated with word if found, NULL otherwise
 */
counters_t* index_find(index_t* index, const char* word) {
  if (index == NULL || word == NULL) {
    return NULL;
  }
  return hashtable_find(index->table, word);
}
