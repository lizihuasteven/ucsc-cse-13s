#include <assert.h>
#include <math.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

#include "bf.h"
#include "ht.h"
#include "metric.h"
#include "node.h"
#include "parser.h"
#include "text.h"

uint32_t noiselimit;	       // Number of noise words to filter out.

struct Text {
  HashTable *ht;
  BloomFilter *bf;
  uint32_t word_count;
};

// The constructor for a text. Using the regex-parsing module, get
// each word of infile and convert it to lowercase. The noise
// parameter is a Text that contains noise words to filter out. That
// is, each parsed, lowercase word is only added to the created Text
// if and only if the word doesn’t appear in the noise Text.

// Why are we ignoring certain words? As you can imagine, certain
// words of the English language occur quite frequently in writing,
// words like “a”, “the”, and “of”. These words aren’t great
// indicators of an author’s unique diction and thus add additional
// noise to the computed distance. Hence, they should be ignored. If
// noise is NULL, then the Text that is being created is the noise
// text itself.

// If sufficient memory cannot be allocated, the function must return
// NULL. Else, it must return a Text *, or a pointer to an allocated
// Text. The hash table should be created with a size of 2^19 and the
// Bloom filter should be created with a size of 2^21.

extern uint32_t optNumberOfNoiseWordsToFilterOut;

Text *text_create(FILE *infile, Text *noise) {
  assert(infile);

  regex_t wordPattern;

  if (regcomp(&wordPattern, "[A-Za-z]+([-'][A-Za-z]+)*", REG_EXTENDED)) {
    fprintf(stderr, "Failed to compile regex.\n");
    return NULL;
  }

  Text *text = (Text *)malloc(sizeof(Text));
  text->ht = ht_create(1 << 19);
  assert(text->ht);
  text->bf = bf_create(1 << 21);
  assert(text->bf);
  text->word_count = 0;

  char *word = NULL;
  while ((noise || text->word_count < optNumberOfNoiseWordsToFilterOut) &&
	 ((word = next_word(infile, &wordPattern)) != NULL)
	)
  {
    if (!noise || !text_contains(noise, word)) {
      // Node *node = ht_insert(text->ht, word);
      ht_insert(text->ht, word);
      bf_insert(text->bf, word);
      text->word_count++;
      // printf("# Word: \"%s\" count=%d, total count=%d\n", word, node->count, text->word_count);
    } else {
      // Node *node = ht_lookup(noise->ht, word);
      // printf("# Word: \"%s\" count=%d, noise\n", word, node->count);
    }
    free(word);
  }

  fclose(infile);
  return text;
}

// Deletes a text. Remember to free both the hash table and the Bloom
// filter in the text before freeing the text itself. Remember to set
// the pointer to NULL after the memory associated with the text is
// freed.

void text_delete(Text **text) {
  assert(text);
  assert(*text);
  ht_delete(&(*text)->ht);
  bf_delete(&(*text)->bf);
  free(*text);
  *text = NULL;
}

// This function returns the distance between the two texts depending
// on the metric being used. This can be either the Euclidean
// distance, the Manhattan distance, or the cosine distance. The
// Metric enumeration is provided to you in metric.h and will be
// mentioned as well in §12.

// Remember that the nodes contain the counts for their respective
// words and still need to be normalized with the total word count
// from the text.

double text_dist(Text *text1, Text *text2, Metric metric) {
  assert(text1);
  assert(text2);

  HashTable *allWords = ht_create(1 << 19);
  uint32_t word_count = 0;

  HashTableIterator *iText1 = hti_create(text1->ht);
  // uint32_t text1InsertionCount = ht_insertion_count(text1->ht);
  Node *node;
  while ((node = ht_iter(iText1))) {
    ht_insert(allWords, node->word);
    word_count++;
    // if (word_count % 1000 == 0) {
    //   printf("# text_dist(): %6u/%u/%u iText1\r", word_count, word_count, text1InsertionCount);
    // }
  }
  printf("\n");


  HashTableIterator *iText2 = hti_create(text2->ht);
  // uint32_t text2InsertionCount = ht_insertion_count(text2->ht);
  while ((node = ht_iter(iText2))) {
    ht_insert(allWords, node->word);
    word_count++;
    // if (word_count % 1000 == 0) {
    //   printf("# text_dist(): %6u/%u/%u iText1\r", word_count, word_count - text1InsertionCount, text2InsertionCount);
    // }
  }

  double *vText1 = (double *)calloc(sizeof(double), word_count);
  double *vText2 = (double *)calloc(sizeof(double), word_count);
  HashTableIterator *iterAllWords = hti_create(allWords);
  uint32_t i = 0;
  while ((node = ht_iter(iterAllWords))) {
    vText1[i] = text_frequency(text1, node->word);
    vText2[i] = text_frequency(text2, node->word);
    // if (vText1[i] != 0.0 && vText2[i] != 0.0) {
    //   printf("# text_dist(): [%u] %.6f %.6f %s\n", i, vText1[i], vText2[i], node->word);
    // }
    i += 1;
  }

  double distance = 0;
  switch(metric) {
  case EUCLIDEAN:
    for (uint32_t i = 0; i < word_count; i++) {
      double difference = vText1[i] - vText2[i];
      distance += difference * difference;
    }
    return sqrt(distance);

  case MANHATTAN:
    for (uint32_t i = 0; i < word_count; i++) {
      double difference = vText1[i] - vText2[i];

      distance += difference > 0.0 ? difference : -difference;
    }
    return distance;

  case COSINE:
    for (uint32_t i = 0; i < word_count; i++) {
      distance += vText1[i] * vText2[i];
    }
    // return 1.0 - distance;
    return distance;

  default:
    assert((size_t)metric < (sizeof(metric_names) / sizeof(metric_names[0])));
  }
  return 0.0;
}

// Returns the frequency of the word in the text. If the word is not
// in the text, then this must return 0. Otherwise, this must return
// the normalized frequency of the word.

double text_frequency(Text *text, char *word) {
  assert(text);
  assert(text->ht);
  assert(word);

  Node *node = ht_lookup(text->ht, word);
  if (node) {
    return (double)node->count / (double) text->word_count;
  }
  return 0.0;
}

// Returns whether or not a word is in the text. This should return
// true if word is in the text and false otherwise.

bool text_contains(Text *text, char *word) {
  assert(text);
  assert(text->ht);
  assert(word);

  if (bf_probe(text->bf, word)) {
    return ht_lookup(text->ht, word) != NULL;
  }
  return NULL;
}

// A debug function to print the contents of a text. You may want to
// just call the respective functions of the component parts of the
// text.

void text_print(Text *text) {
  assert(text);
  assert(text->ht);
  assert(text->bf);
  printf("(Text *)%p->{ ht=", (void *)text);
  ht_print(text->ht);
  printf(", bf=");
  bf_print(text->bf);
  printf(", word_count=%d }\n", text->word_count);
}


uint32_t text_total_word_count(Text *text) {
  assert(text);
  return text->word_count;
}

uint32_t text_unique_word_count(Text *text) {
  assert(text);
  assert(text->ht);
  return ht_insertion_count(text->ht);
}

