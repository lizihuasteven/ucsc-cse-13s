#include <assert.h>
#include <ctype.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

// Returns the next word that matches the specified regular
// expression.  Words are buffered and returned as they are read from
// the input file.
//
// infile:      The input file to read from.
// word_regex:  The compiled regular expression for a word.
// returns:     The next word if it exists, a null pointer otherwise.

typedef struct Word {
  struct Word *next;
  char *text;
} Word;

char *next_word(FILE *infile, regex_t *word_regex) {
  assert(infile);
  assert(word_regex);

  static Word *words = NULL;

  if (words) {
    Word *word = words;
    words = word->next;
    char *text = word->text;
    free(word);
    return text;
  }

  char buffer[4096];

  while (fgets(buffer, sizeof(buffer), infile)) {
    char *ptr = buffer;
    regmatch_t match;
    while (!regexec(word_regex, ptr, 1, &match, 0)) {
      char *text = ptr + match.rm_so;
      text[match.rm_eo - match.rm_so] = '\0';
      ptr += match.rm_eo + 1;
      Word *word = (Word *)malloc(sizeof(Word));
      word->next = words;
      word->text = strdup(text);
      for (char *c = word->text; *c; c++) {
	*c = tolower(*c);
      }
      words = word;
    }

    if (words) {
      Word *word = words;
      words = word->next;
      char *text = word->text;
      free(word);
      return text;
    }
  }

  assert(!words);
  return NULL;
}
