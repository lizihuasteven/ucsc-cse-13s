#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define OPTIONS "hn:r:"

/*
 * Function: in_circle
 * -------------------
 *  x: x coordinate of the point
 *  y: y coordinate of the point
 *  distance: distance of point from the origin (0,0)
 *  returns: 1 if the point is in the circle, 0 otherwise
 */
uint64_t in_circle(double x, double y) {
  double distance = x * x + y * y;
  if (distance <= 1) {
    return 1;
  }
  return 0;
}

void usage(char *exec) {
  fprintf(stderr,
          "SYNOPSIS\n"
          "   Prints the Monte Carlo value of Pi.\n"
          "\n"
          "USAGE\n"
          "   %s [-hn:r:] [-n points] [-r seed]\n"
          "\n"
          "OPTIONS\n"
          "   -h         display program help and usage.\n"
          "   -n points  number of points for estimation.\n"
          "   -r seed    deterministic random stating point.\n",
          exec);
}

int main(int argc, char **argv) {

  uint32_t seed = time(0); /* Default (no options) is random based on time. */
  srandom(seed);
  uint32_t points = 20; /* Default number of points is 20 */

  /*
   * Checks commandline arguments, sets the seed and the number of points if
   * provided.
   */
  int opt = 0;
  while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
    switch (opt) {
    case 'n':
      points =
          (uint32_t)strtoul(optarg, NULL, 10); /* Explicit number of points */
      break;
    case 'r':
      seed = (uint32_t)strtoul(optarg, NULL, 10); /* Deterministic seed */
      srandom(seed);
      break;
    default:
      usage(argv[0]); /* Invalid options, show usage */
      return EXIT_FAILURE;
    }
  }

  double x, y, pi;
  uint64_t count = 0; /* To maintain the count of points in the circle */
  printf("%10s %7s %16s %16s %7s\n", "Iteration", "Pi", "x", "y", "circle");
  for (uint64_t n = 1; n <= points; n += 1) {
    x = (double)random() / RAND_MAX; /* Get random x between 0 and 1 */
    y = (double)random() / RAND_MAX; /* Get random y between 0 and 1 */
    count += in_circle(x, y);   /* Increment count if in_circle returns 1 */
    pi = (double)count / n * 4; /* Calculate pi */
    printf("%10" PRIu64 "%8g %16g %16g %7" PRIu64 "\n", n - 1, pi, x, y,
           in_circle(x, y));
  }

  return 0;
}
