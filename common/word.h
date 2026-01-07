/*
 * word.h    Gretchen Kerfoot    Spring 2025
 *
 * This header declares a function for normalizing words, a normalized word
 *is all lowercase and alphabetic only.
 */

#ifndef __WORD_H
#define __WORD_H

#include <stdbool.h>

/*
 * Converts word to lowercase if alphabetic.
 *
 * Caller provides:
 *   word - non-null string to normalize
 * Returns:
 *   newly allocated normalized string, or NULL if word contains non-alpha 
 *   characters
 */
char* normalizeWord(const char* word);

#endif // __WORD_H
