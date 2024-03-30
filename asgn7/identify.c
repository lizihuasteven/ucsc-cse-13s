#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "bv.h"
#include "ht.h"
#include "metric.h"
#include "node.h"
#include "parser.h"
#include "pq.h"
#include "salts.h"
#include "speck.h"
#include "text.h"

char const *ARGV0 = "identify";

void Usage() {
  fprintf(stderr,
	  "Usage: %s [ <option>... ]\n"
	  "  where <option> is one or more of:\n"
	  "    -d: Specify path to database of authors and texts. The default is lib.db.\n"
	  "    -n: Specify path to file of noise words to filter out. The default is noise.txt.\n"
	  "    -k: Specify the number of matches. The default is 5.\n"
	  "    -l: Specify the number of noisewords to filter out. The default is 100.\n"
	  "    -e: Set the distance metric to use as the Euclidean distance.\n"
	  "        This should be the default metric if no other distance metric is specified.\n"
	  "    -m: Set the distance metric to use as the Manhattan distance.\n"
	  "    -c: Set the distance metric to use as the cosine distance.\n"
	  "    -h: Display program help and usage.\n",
	  ARGV0
	 );
  exit(1);
}

uint32_t optNumberOfNoiseWordsToFilterOut = 100;

typedef struct AuthorPathText {
  char *author;
  char *path;
  Text *text;
} AuthorPathText;

int main(int argc, char *const argv[]) {
  char *optPathToDatabase = "lib.db";
  char *optPathToNoiseWords = "noise.txt";
  uint32_t optNumberOfMatches = 5;
  Metric optMetric = EUCLIDEAN;
  (void) optMetric;

  // Process the command line.

  ARGV0 = argv[0];

  int c;
  while ((c = getopt(argc, argv, "d:n:k:l:emch")) != -1) {
    switch (c) {
    case 'd':
      optPathToDatabase = optarg;
      break;
    case 'n':
      optPathToNoiseWords = optarg;
      break;
    case 'k':
      if (sscanf(optarg, "%u", &optNumberOfMatches) != 1) {
	fprintf(stderr, "%s: -k <unsigned-number-of-matches>.\n", ARGV0);
	Usage();
      }
      break;
    case 'l':
      if (sscanf(optarg, "%u", &optNumberOfNoiseWordsToFilterOut) != 1) {
	fprintf(stderr, "%s: -l <unsigned-number-of-noise-words-to-filter-out>.\n", ARGV0);
	Usage();
      }
      break;
    case 'e':
      optMetric = EUCLIDEAN;
      break;
    case 'm':
      optMetric = MANHATTAN;
      break;
    case 'c':
      optMetric = COSINE;
      break;
    case 'h':
      Usage();
      break;
    default:
      fprintf(stderr, "%s: Unrecognized option.\n", ARGV0);
      Usage();
    }
  }

  printf("# %s -d %s -n %s -k %u -l %u %s\n",
	 ARGV0,
	 optPathToDatabase,
	 optPathToNoiseWords,
	 optNumberOfMatches,
	 optNumberOfNoiseWordsToFilterOut,
	 (optMetric == EUCLIDEAN) ? "-e" :
	 (optMetric == MANHATTAN) ? "-m" :
	 (optMetric == COSINE) ? "-c" :
	 "metric???"
        );

  // Open the database file.

  FILE *databaseFile = fopen(optPathToDatabase, "r");
  if (!databaseFile) {
    fprintf(stderr, "%s: Unable to open database file \"%s\".\n", ARGV0, optPathToDatabase);
    perror(ARGV0);
    exit(1);
  }

  // Open the noise file.

  FILE *noiseFile = fopen(optPathToNoiseWords, "r");
  if (!noiseFile) {
    fprintf(stderr, "%s: Unable to open noise file \"%s\".\n", ARGV0, optPathToNoiseWords);
    perror(ARGV0);
    exit(1);
  }

  // Create the noise text.

  Text *noise = text_create(noiseFile, NULL);
  printf("# Noise text, words: %u/%u\n", text_total_word_count(noise), text_unique_word_count(noise));
  // text_print(noise);

  // Create the anonymous text.

  Text *anonymous = text_create(stdin, noise);
  printf("# Anonymous text, words: %u/%u\n", text_total_word_count(anonymous), text_unique_word_count(anonymous));
  // text_print(anonymous);

  // Read the database....

  char buffer[4096];
  uint32_t numberOfTexts = 0;

  // Read the number of texts from the database.

  if (!fgets(buffer, sizeof(buffer), databaseFile) || sscanf(buffer, "%u", &numberOfTexts) != 1) {
    fprintf(stderr, "%s: Unable to read number of texts from database.\n", ARGV0);
    perror(ARGV0);
    exit(1);
  }

  // Create a vector to hold the texts' Text.

  AuthorPathText **texts = (AuthorPathText **)calloc(sizeof(AuthorPathText *), numberOfTexts);

  // For each text mentioned in the database,...

  for (uint32_t i = 0; i < numberOfTexts; i++) {

    // ... read an author name,...

    if (!fgets(buffer, sizeof(buffer), databaseFile)) {
      fprintf(stderr, "%s: Unable to read author name from database.\n", ARGV0);
      perror(ARGV0);
      exit(1);
    }
    buffer[strlen(buffer) - 1] = '\0';
    char *author = strdup(buffer);

    // ... and text path.

    if (!fgets(buffer, sizeof(buffer), databaseFile)) {
      fprintf(stderr, "%s: Unable to read text path from database.\n", ARGV0);
      perror(ARGV0);
      exit(1);
    }
    buffer[strlen(buffer) - 1] = '\0';
    char *textPath = strdup(buffer);

    // Open the text path,...

    FILE *textFile = fopen(textPath, "r");

    if (!textFile) {
      fprintf(stderr, "%s: Unable to open text file \"%s\".\n", ARGV0, textPath);
      perror(ARGV0);
      exit(1);
    }

    // ... and create the text's Text.

    AuthorPathText *apt = (AuthorPathText *)malloc(sizeof(AuthorPathText));

    assert(apt);
    apt->author = author;
    apt->path = textPath;
    apt->text = text_create(textFile, noise);
    assert(apt->text);

    texts[i] = apt;

    printf("# %u/%u) Author: %s, Path: %s text, words: %u/%u.\n",
	   i + 1,
	   numberOfTexts,
	   author,
	   textPath,
	   text_total_word_count(apt->text),
	   text_unique_word_count(apt->text)
	  );
    // text_print(app->text);
  }

  PriorityQueue *pq = pq_create(numberOfTexts);
  assert(pq);

  for (uint32_t i = 0; i < numberOfTexts; i++) {
    double distance = text_dist(anonymous, texts[i]->text, optMetric);
    enqueue(pq, texts[i]->author, distance);
  }

  printf("Top %u, metric: %s, noise limit: %u\n",
	 optNumberOfMatches,
	 metric_names[optMetric],
	 optNumberOfNoiseWordsToFilterOut
	);

  for (uint32_t i = 0; i < optNumberOfMatches && i < numberOfTexts; i++) {
    char *author;
    double distance;
    dequeue(pq, &author, &distance);
    printf("%u) %s [%.15f]\n", i + 1, author, distance);
  }

  pq_delete(&pq);

  for (uint32_t i = 0; i < numberOfTexts; i++) {
    text_delete(&texts[i]->text);
    free(texts[i]->author);
    texts[i]->author = NULL;
    free(texts[i]->path);
    texts[i]->path = NULL;
    free(texts[i]);
    texts[i] = NULL;
  }

  text_delete(&anonymous);
  text_delete(&noise);
  return 0;
}
