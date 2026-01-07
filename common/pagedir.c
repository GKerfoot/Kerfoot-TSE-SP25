/*
 * pagedir.c    Gretchen Kerfoot    Spring 2025
 *
 * Implementation of the pagedir module.
 * Supports marking a page directory, saving webpages, validating crawler 
 * directories, and loading webpage files.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "webpage.h"
#include "pagedir.h"
#include "webpage.h"
#include "file.h"


/* Marks a directory as produced by the crawler
 *
 * Caller provides:
 *   pageDirectory - path to an existing directory
 * Return:
 *   true if '.crawler' file was successfully created,
 *   false otherwise
 * Notes:
 *   It's the caller's responsibility to ensure pageDirectory exists
 *   before calling this function
 */
bool pagedir_init(const char* pageDirectory) {
  if (pageDirectory == NULL) {
    return false;
  }

  //builds the path to the .crawler file
  char filepath[200];
  snprintf(filepath, sizeof(filepath), "%s/.crawler", pageDirectory);

  //tries to open the .crawler file for writing
  FILE* fp = fopen(filepath, "w");
  if (fp == NULL) {
    return false;
  }

  fclose(fp);
  return true;
}


/* Saves a webpage into a uniquely numbered file in a given directory
 *
 * Caller provides:
 *   page - a pointer to a valid webpage_t struct
 *   pageDirectory - path to the directory in which to save
 *   docID - unique integer ID for the page
 * Notes:
 *   Creates a file named pageDirectory/docID containing: 
 *   the URL (first line), the depth (second line),
 *   the page HTML (following lines)
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID) {
  if (page == NULL || pageDirectory == NULL || docID <= 0) {
    return;
  }

  //builds the path to the new page file (pageDirectory/docID)
  char filepath[200];
  snprintf(filepath, sizeof(filepath), "%s/%d", pageDirectory, docID);

  //opens the page file for writing
  FILE* fp = fopen(filepath, "w");
  if (fp == NULL) {
    fprintf(stderr, "Error: Unable to write page to file %s\n", filepath);
    return;
  }

  //writes URL, depth, and HTML contents
  fprintf(fp, "%s\n", webpage_getURL(page));
  fprintf(fp, "%d\n", webpage_getDepth(page));
  fprintf(fp, "%s", webpage_getHTML(page));

  //closes file
  fclose(fp);
}


/* Checks whether a given directory contains a valid .crawler marker file.
 *
 * Caller provides:
 *   pageDirectory - path to a directory
 * Returns:
 *   true if .crawler file exists in the directory, false otherwise
 */
bool pagedir_validate(const char* pageDirectory) {
  if (pageDirectory == NULL) {
    return false;
  }

  //constructs path to .crawler marker file
  char* path = malloc(strlen(pageDirectory) + strlen("/.crawler") + 1);
  if (path == NULL) {
    return false;
  }

  sprintf(path, "%s/.crawler", pageDirectory);

  FILE* fp = fopen(path, "r");
  free(path);

  if (fp == NULL) {
    return false;
  } else {
    fclose(fp);
    return true;
  }
}


/* Loads a webpage from a file named pageDirectory/docID.
 *
 * Caller provides:
 *   pageDirectory - crawler-produced directory
 *   docID - positive document ID
 * Returns:
 *   pointer to a new webpage_t containing URL, depth, and HTML content
 *   NULL if any part of the file cannot be read
 */
webpage_t* pagedir_load(const char* pageDirectory, const int docID) {
  if (pageDirectory == NULL || docID <= 0) {
    return NULL;
  }

  //constructs filename: pageDirectory/docID
  char* filename = malloc(strlen(pageDirectory) + 20); //enough for large integers
  if (filename == NULL) {
    return NULL;
  }

  sprintf(filename, "%s/%d", pageDirectory, docID);

  FILE* fp = fopen(filename, "r");
  free(filename);
  if (fp == NULL) {
    return NULL;
  }

  //reads the URL (first line)
  char* url = file_readLine(fp);
  if (url == NULL) {
    fclose(fp);
    return NULL;
  }

  //reads the depth (second line)
  int depth;
  if (fscanf(fp, "%d\n", &depth) != 1) {
    free(url);
    fclose(fp);
    return NULL;
  }

  //reads the HTML content (rest of file)
  char* html = file_readFile(fp);
  fclose(fp);

  if (html == NULL) {
    free(url);
    return NULL;
  }

  //creates and returns the webpage object
  webpage_t* page = webpage_new(url, depth, html);
  return page;
}
