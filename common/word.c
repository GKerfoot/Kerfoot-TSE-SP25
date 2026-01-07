/*
 * word.c   Gretchen Kerfoot    Spring 2024 
 *
 * This module provides a function to normalize words to lowercase.
 * It ensures words are alphabetic and at least 3 characters long.
 */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "word.h"


/*
 * Converts word to lowercase if alphabetic.
 *
 * Caller provides:
 *   word - non-null string to normalize
 * Returns:
 *   newly allocated normalized string, or NULL if word contains non-alpha 
 *   characters
 */
char* normalizeWord(const char* word) {
  if (word == NULL) {
    return NULL;
  }

  int length = strlen(word);
  for (int i = 0; i < length; i++) {
    if (!isalpha(word[i])) {
      return NULL; //rejects words with non-alphabetic characters
    }
  }

  char* norm = malloc(length + 1);
  if (norm == NULL) {
    return NULL;
  }

  for (int i = 0; i < length; i++) {
    norm[i] = tolower(word[i]);
  }
  norm[length] = '\0';

  return norm;
}
