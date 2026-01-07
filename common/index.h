/*
 * index.h    Gretchen Kerfoot    Spring 2025
 *
 * This is the header file for the index module.
 * It provides functions for creating, updating, saving, loading, and 
 * deleting an index structure used by the TSE.
 * The index is a hashtable mapping from words (strings) to counters sets, 
 * where each counters set stores (docID, count) pairs.
 */

//header guard prevents multiple inclusion of same header file
#ifndef __INDEX_H
#define __INDEX_H

#include <stdio.h>
#include <stdbool.h>
#include "counters.h"

//global types
typedef struct index index_t;

/*
 * Creates a new index with the given number of slots.
 *
 * Caller provides:
 *   numSlots - number of slots for underlying hashtable
 * Returns:
 *   pointer to a new index_t struct, or NULL if error
 */
index_t* index_new(const int numSlots);


/*
 * Inserts the given word/docID pair into the index
 *
 * Caller provides:
 *   index - pointer to an index structure
 *   word - non-NULL word string
 *   docID - positive document ID
 * Returns:
 *   true if insertion was successful, false otherwise
 * Notes:
 *   Allocates a new counters set for a word if not already present.
 *   Increments count for docID if already present
 */
bool index_insert(index_t* index, const char* word, const int docID);


/*
 * Saves the index to a file in the required format.
 *
 * Caller provides:
 *   index - pointer to a valid index
 *   filename - path to a writable output file
 * Returns:
 *   true if file written successfully, false otherwise
 */
bool index_save(index_t* index, const char* filename);

/*
 * Loads an index from an existing file.
 *
 * Caller provides:
 *   filename - path to an index file formatted as specified
 * Returns:
 *   pointer to an index_t structure if successful, or NULL on failure
 * Notes:
 *   Assumes the input file format is already correct
 */
index_t* index_load(const char* filename);

/*
 * Deletes an index and frees all allocated memory.
 *
 * Caller provides:
 *   index - pointer to a valid index or NULL
 */
void index_delete(index_t* index);

/*
 * Looks up the counters for a given word in the index
 *
 * Caller provides:
 *  index - pointer to a valid index or NULL
 *  word - target word
 * Returns:
 *  pointer to the counters_t structure for the word
 *  NULL if not found
 */
counters_t* index_find(index_t* index, const char* word);

#endif // __INDEX_H
