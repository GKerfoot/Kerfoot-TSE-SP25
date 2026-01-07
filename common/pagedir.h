/*
 * pagedir.h    Gretchen Kerfoot    Spring 2025
 *
 * This is the header file for the pagedir module. 
 * It provides functions for initializing a pageDirectory, saving a
 * webpage to a uniquely numbered file in that directory, validating 
 * crawler directories, and loading webpage files.
 */

//header guards prevent multiple inclusion of same .h file
//placed to prevent errors with more complex completed TSE
#ifndef __PAGEDIR_H
#define __PAGEDIR_H

#include <stdio.h>
#include <stdbool.h>
#include "webpage.h"

/*
 * Creates a .crawler file inside the given directory to mark it
 * as produced by the crawler.
 *
 * Caller provides:
 *   pageDirectory - path to an existing directory
 * Return:
 *   true if .crawler file was successfully created, false otherwise
 */
bool pagedir_init(const char* pageDirectory);

/*
 * Saves a webpage into the given pageDirectory under the given docID.
 *
 * Caller provides:
 *   page - pointer to valid webpage structure
 *   pageDirectory - path to target directory
 *   docID - unique integer ID for page
 * Notes:
 *   The saved file contains the URL (first line), depth (second line),
 *   and the  HTML content (following lines)
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/*
 * Validates that the given directory was created by the crawler.
 *
 * Caller provides:
 *   pageDirectory - path to a directory
 * Returns:
 *   true if the directory contains a .crawler file, false otherwise
 */
bool pagedir_validate(const char* pageDirectory);

/*
 * Loads a webpage from the given pageDirectory with the specified docID.
 *
 * Caller provides:
 *   pageDirectory - path to crawler directory
 *   docID - integer ID of the document to load
 * Returns:
 *   pointer to newly allocated webpage, or NULL if file cannot be read
 */
webpage_t* pagedir_load(const char* pageDirectory, const int docID);

#endif // __PAGEDIR_H

