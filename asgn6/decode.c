#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "code.h"
#include "io.h"
#include "trie.h"
#include "word.h"

// 5 Program Options
//
// Your decode program must support the following getopt() options:
//
// • -v : Print decompression statistics to stderr.
//
// • -i <input> : Specify input to decompress (stdin by default)
//
// • -o <output> : Specify output of decompressed input (stdout by
//   -default)
//
// The verbose option enables a flag to print out informative
// statistics about the compression or decompression that is
// performed. These statistics include the compressed file size, the
// uncompressed file size, and space saving. The formula for
// calculating space saving is:
//
// 100 * (1 - // compressed size / uncompressed size)
//
// The verbose output of both encode and decode must match the following:
//
// Compressed file size: X bytes
// Uncompressed file size: X bytes
// Space saving: XX.XX%
//
// 7 Decompression
//
// The following steps for decompression will refer to the input file
// to decompress as infile and the uncompressed output file as
// outfile.
//
// 1. Open infile with open(). If an error occurs, print a helpful
// message and exit with a status code indicating that an error
// occurred. infile should be stdin if an input file wasn’t specified.
//
// 2. Read in the file header with read_header(), which also verifies
// the magic number. If the magic number is verified then
// decompression is good to go and you now have a header which
// contains the original protection bit mask.
//
// 3. Open outfile using open(). The permissions for outfile should
// match the protection bits as set in your file header that you just
// read. Any errors with opening outfile should be handled like with
// infile.  outfile should be stdout if an output file wasn’t
// specified.
//
// 4. Create a new word table with wt_create() and make sure each of
// its entries are set to NULL. Initialize the table to have just the
// empty word, a word of length 0, at the index EMPTY_CODE. We will
// refer to this table as table.
//
// 5. You will need two uint16_t to keep track of the current code and
// next code. These will be referred to as curr_code and next_code,
// respectively. next_code should be initialized as START_CODE and
// functions exactly the same as the monotonic counter used during
// compression, which was also called next_code.
//
// 6. Use read_pair() in a loop to read all the pairs from infile. We
// will refer to the code and symbol from each read pair as curr_code
// and curr_sym, respectively. The bit-length of the code to read is
// the bit-length of next_code. The loop breaks when the code read is
// STOP_CODE. For each read pair, perform the following:
//
//   (a) As seen in the decompression example, we will need to append
//   the read symbol with the word denoted by the read code and add
//   the result to table at the index next_code. The word denoted by
//   the read code is stored in table[curr_code]. We will append
//   table[curr_code] and curr_sym using word_append_sym().
//
//   (b) Write the word that we just constructed and added to the
//   table with write_word(). This word should have been stored in
//   table[next_code].
//
//   (c) Increment next_code and check if it equals MAX_CODE. If it
//   has, reset the table using wt_reset() and set next_code to be
//   START_CODE. This mimics the resetting of the trie during
//   compression.
//
// 7. Flush any buffered words using flush_words(). Like with
// write_pair(), write_word() buffers words under the hood, so we have
// to remember to flush the contents of our buffer.
//
// 8. Close infile and outfile with close().

char const *ARGV0 = "decode";
char *inPath = NULL;
char *outPath = NULL;
int optAscii = 0;
int optVerbose = 0;

int bitLength(uint16_t count) {
    if (count == 0) {
        return 1;
    }
    int result = 0;
    while (count > 0) {
        count >>= 1;
        result += 1;
    }
    return result;
}

int main(int argc, char *const argv[]) {
    ARGV0 = argv[0];

    // extern char *optarg;
    // extern int optind, optopt;
    int c;

    // Your decode program must support the following getopt() options:

    while ((c = getopt(argc, argv, "ai:o:vh")) != -1) {
        switch (c) {

            // • -a : Use ASCII, instead of binary, encoding.

        case 'a':
            optAscii++;
            break;

            // • -i <input> : Specify input to decompress (stdin by default)

        case 'i':
            inPath = strdup(optarg);
            break;

            // • -o <output> : Specify output of decompressed input (stdout by
            //   -default)

        case 'o':
            outPath = strdup(optarg);
            break;

            // • -v : Print decompression statistics to stderr.

        case 'v': optVerbose++; break;

        case 'h':
            printf("SYNOPSIS\n   Compresses files using the LZ78 compression algorithm.\n   "
                   "Compressed files are decompressed with the corresponding decoder.\n\nUSAGE\n   "
                   "./encode [-vh] [-i input] [-o output]\n\nOPTIONS\n   -v          Display "
                   "compression statistics\n   -i input    Specify input to compress (stdin by "
                   "default)\n   -o output   Specify output of compressed input (stdout by "
                   "default)\n   -h          Display program help and usage\n");
            return 0;

        default:
            printf("SYNOPSIS\n   Compresses files using the LZ78 compression algorithm.\n   "
                   "Compressed files are decompressed with the corresponding decoder.\n\nUSAGE\n   "
                   "./encode [-vh] [-i input] [-o output]\n\nOPTIONS\n   -v          Display "
                   "compression statistics\n   -i input    Specify input to compress (stdin by "
                   "default)\n   -o output   Specify output of compressed input (stdout by "
                   "default)\n   -h          Display program help and usage\n");
            return 0;
        }
    }

    // 7 Decompression
    //
    // The following steps for decompression will refer to the input file
    // to decompress as infile and the uncompressed output file as
    // outfile.
    //
    // 1. Open infile with open(). If an error occurs, print a helpful
    // message and exit with a status code indicating that an error
    // occurred. infile should be stdin if an input file wasn’t specified.

    int inFile = 0; // stdin is the default
    if (inPath) {
        if ((inFile = open(inPath, O_RDONLY /* | O_BINARY */)) == -1) {
            fprintf(stderr, "%s: Unable to open (-i) %s\n", ARGV0, inPath);
            perror(ARGV0);
            exit(1);
        }
    }

    // 2. Read in the file header with read_header(), which also verifies
    // the magic number. If the magic number is verified then
    // decompression is good to go and you now have a header which
    // contains the original protection bit mask.

    FileHeader inFileHeader;
    read_header(inFile, &inFileHeader);

    // 3. Open outfile using open(). The permissions for outfile should
    // match the protection bits as set in your file header that you just
    // read. Any errors with opening outfile should be handled like with
    // infile.  outfile should be stdout if an output file wasn’t
    // specified.

    int outFile = 1; // stdout is the default
    if (outPath) {
        if ((outFile = open(outPath, O_WRONLY | O_CREAT | O_TRUNC /* | O_BINARY */)) == -1) {
            fprintf(stderr, "%s: Unable to open (-o) %s\n", ARGV0, outPath);
            perror(ARGV0);
            exit(1);
        }
        fchmod(outFile, inFileHeader.protection);
    }

    // 4. Create a new word table with wt_create() and make sure each of
    // its entries are set to NULL. Initialize the table to have just the
    // empty word, a word of length 0, at the index EMPTY_CODE. We will
    // refer to this table as table.

    WordTable *wordTable = wt_create();

    // 5. You will need two uint16_t to keep track of the current code and
    // next code. These will be referred to as curr_code and next_code,
    // respectively. next_code should be initialized as START_CODE and
    // functions exactly the same as the monotonic counter used during
    // compression, which was also called next_code.

    uint16_t curr_code;
    uint16_t next_code = START_CODE;

    // 6. Use read_pair() in a loop to read all the pairs from infile. We
    // will refer to the code and symbol from each read pair as curr_code
    // and curr_sym, respectively. The bit-length of the code to read is
    // the bit-length of next_code. The loop breaks when the code read is
    // STOP_CODE. For each read pair, perform the following:

    uint8_t curr_sym;
    while (read_pair(inFile, &curr_code, &curr_sym, bitLength(next_code))) {

        // (a) As seen in the decompression example, we will need to append
        // the read symbol with the word denoted by the read code and add
        // the result to table at the index next_code. The word denoted by
        // the read code is stored in table[curr_code]. We will append
        // table[curr_code] and curr_sym using word_append_sym().

        wordTable[next_code] = word_append_sym(wordTable[curr_code], curr_sym);

        // (b) Write the word that we just constructed and added to the
        // table with write_word(). This word should have been stored in
        // table[next_code].

        write_word(outFile, wordTable[next_code]);

        // (c) Increment next_code and check if it equals MAX_CODE. If it
        // has, reset the table using wt_reset() and set next_code to be
        // START_CODE. This mimics the resetting of the trie during
        // compression.

        if (++next_code == MAX_CODE) {
            next_code = START_CODE;
            wt_reset(wordTable);
        }
    }

    // 7. Flush any buffered words using flush_words(). Like with
    // write_pair(), write_word() buffers words under the hood, so we have
    // to remember to flush the contents of our buffer.

    flush_words(outFile);

    // 8. Close infile and outfile with close().

    if (optVerbose) {
        extern uint64_t total_bytes_read;
        extern uint64_t total_bytes_written;

        fprintf(stdout, "Compressed file size: %lu bytes\n", total_bytes_read);
        fprintf(stdout, "Uncompressed file size: %lu bytes\n", total_bytes_written);
        fprintf(stdout, "Space saving: %.2f%%\n",
            100.0 * (1.0 - (double) total_bytes_read / (double) total_bytes_written));
    }

    close(inFile);
    close(outFile);
    return 0;
}
