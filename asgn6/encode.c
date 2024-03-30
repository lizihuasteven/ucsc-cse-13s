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
// Your encode program must support the following getopt() options:
//
// • -v : Print compression statistics to stderr.
//
// • -i <input> : Specify input to compress (stdin by default)
//
// • -o <output> : Specify output of compressed input (stdout by
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
// 6 Compression
//
// The following steps for compression will refer to the input file
// descriptor to compress as infile and the compressed output file
// descriptor as outfile.
//
// 1. Open infile with open(). If an error occurs, print a helpful
// message and exit with a status code indicating that an error
// occurred. infile should be stdin if an input file wasn’t specified.
//
// 2. The first thing in outfile must be the file header, as defined
// in the file io.h. The magic number in the header must be
// 0xBAADBAAC. The file size and the protection bit mask you will
// obtain using fstat(). See the man page on it for details.
//
// 3. Open outfile using open(). The permissions for outfile should
// match the protection bits as set in your file header. Any errors
// with opening outfile should be handled like with infile. outfile
// should be stdout if an output file wasn’t specified.
//
// 4. Write the filled out file header to outfile using
// write_header(). This means writing out the struct itself to the
// file, as described in the comment block of the function.
//
// 5. Create a trie. The trie initially has no children and consists
// solely of the root. The code stored by this root trie node should
// be EMPTY_CODE to denote the empty word. You will need to make a
// copy of the root node and use the copy to step through the trie to
// check for existing prefixes. This root node copy will be referred
// to as curr_node. The reason a copy is needed is that you will
// eventually need to reset whatever trie node you’ve stepped to back
// to the top of the trie, so using a copy lets you use the root node
// as a base to return to.
//
// 6. You will need a monotonic counter to keep track of the next
// available code. This counter should start at START_CODE, as defined
// in the supplied code.h file. The counter should be a uint16_t since
// the codes used are unsigned 16-bit integers. This will be referred
// to as next_code.
//
// 7. You will also need two variables to keep track of the previous
// trie node and previously read symbol. We will refer to these as
// prev_node and prev_sym, respectively.
//
// 8. Use read_sym() in a loop to read in all the symbols from
// infile. Your loop should break when read_sym() returns false. For
// each symbol read in, call it curr_sym, perform the following:
//
//   (a) Set next_node to be trie_step(curr_node, curr_sym), stepping
//   down from the current node to the currently read symbol.
//
//   (b) If next_node is not NULL, that means we have seen the current
//   prefix. Set prev_node to be curr_node and then curr_node to be
//   next_node.
//
//   (c) Else, since next_node is NULL, we know we have not
//   encountered the current prefix. We write the pair
//   (curr_node->code, curr_sym), where the bit-length of the written
//   code is the bit-length of next_code.  We now add the current
//   prefix to the trie. Let curr_node->children[curr_sym] be a new
//   trie node whose code is next_code. Reset curr_node to point at
//   the root of the trie and increment the value of next_code.
//
//   (d) Check if next_code is equal to MAX_CODE. If it is, use
//   trie_reset() to reset the trie to just having the root node. This
//   reset is necessary since we have a finite number of codes.
//
//   (e) Update prev_sym to be curr_sym.
//
// 9. After processing all the characters in infile, check if
// curr_node points to the root trie node. If it does not, it means we
// were still matching a prefix. Write the pair (prev_node->code,
// prev_sym). The bit-length of the code written should be the
// bit-length of next_code. Make sure to increment next_code and that
// it stays within the limit of MAX_CODE. Hint: use the modulo
// operator.
//
// 10. Write the pair (STOP_CODE, 0) to signal the end of compressed
// output. Again, the bit-length of code written should be the
// bit-length of next_code.
//
// 11. Make sure to use flush_pairs() to flush any unwritten, buffered
// pairs. Remember, calls to write_pair() end up buffering them under
// the hood. So, we have to remember to flush the contents of our
// buffer.
//
// 12. Use close() to close infile and outfile.

char const *ARGV0 = "encode";
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

    // Your encode program must support the following getopt() options:

    while ((c = getopt(argc, argv, "ai:o:vh")) != -1) {
        switch (c) {

            // • -a : Use ASCII, instead of binary, encoding.

        case 'a':
            optAscii++;
            break;

            // • -i <input> : Specify input to compress (stdin by default)

        case 'i':
            inPath = strdup(optarg);
            break;

            // • -o <output> : Specify output of compressed input (stdout by
            //   -default)

        case 'o':
            outPath = strdup(optarg);
            break;

            // • -v : Print compression statistics to stderr.

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

    // The following steps for compression will refer to the input file
    // descriptor to compress as infile and the compressed output file
    // descriptor as outfile.
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

    // 2. The first thing in outfile must be the file header, as defined
    // in the file io.h. The magic number in the header must be
    // 0xBAADBAAC. The file size and the protection bit mask you will
    // obtain using fstat(). See the man page on it for details.

    struct stat inFileStats;
    if (fstat(inFile, &inFileStats) == -1) {
        fprintf(stderr, "%s: Unable to fstat() %s.\n", ARGV0, inPath ? inPath : "<stdin>");
        perror(ARGV0);
        exit(1);
    }
    FileHeader outFileHeader = { 0xBAADBAAC, inFileStats.st_mode };

    // 3. Open outfile using open(). The permissions for outfile should
    // match the protection bits as set in your file header. Any errors
    // with opening outfile should be handled like with infile. outfile
    // should be stdout if an output file wasn’t specified.

    int outFile = 1; // stdout is the default
    if (outPath) {
        if ((outFile = open(outPath, O_WRONLY | O_CREAT | O_TRUNC /* | O_BINARY */)) == -1) {
            fprintf(stderr, "%s: Unable to open (-o) %s\n", ARGV0, outPath);
            perror(ARGV0);
            exit(1);
        }
        fchmod(outFile, inFileStats.st_mode);
    }

    // 4. Write the filled out file header to outfile using
    // write_header(). This means writing out the struct itself to the
    // file, as described in the comment block of the function.

    write_header(outFile, &outFileHeader);

    // 5. Create a trie. The trie initially has no children and consists
    // solely of the root. The code stored by this root trie node should
    // be EMPTY_CODE to denote the empty word. You will need to make a
    // copy of the root node and use the copy to step through the trie to
    // check for existing prefixes. This root node copy will be referred
    // to as curr_node. The reason a copy is needed is that you will
    // eventually need to reset whatever trie node you’ve stepped to back
    // to the top of the trie, so using a copy lets you use the root node
    // as a base to return to.

    TrieNode *trieRoot = trie_create();
    TrieNode *curr_node = trieRoot;

    // 6. You will need a monotonic counter to keep track of the next
    // available code. This counter should start at START_CODE, as defined
    // in the supplied code.h file. The counter should be a uint16_t since
    // the codes used are unsigned 16-bit integers. This will be referred
    // to as next_code.

    uint16_t next_code = START_CODE;

    // 7. You will also need two variables to keep track of the previous
    // trie node and previously read symbol. We will refer to these as
    // prev_node and prev_sym, respectively.

    TrieNode *prev_node = NULL;
    uint8_t prev_sym;

    // 8. Use read_sym() in a loop to read in all the symbols from
    // infile. Your loop should break when read_sym() returns false. For
    // each symbol read in, call it curr_sym, perform the following:

    uint8_t curr_sym;
    while (read_sym(inFile, &curr_sym)) {

        //   (a) Set next_node to be trie_step(curr_node, curr_sym), stepping
        //   down from the current node to the currently read symbol.

        TrieNode *next_node = trie_step(curr_node, curr_sym);
        if (next_node) {

            //   (b) If next_node is not NULL, that means we have seen the current
            //   prefix. Set prev_node to be curr_node and then curr_node to be
            //   next_node.

            prev_node = curr_node;
            curr_node = next_node;
        } else {

            //   (c) Else, since next_node is NULL, we know we have not
            //   encountered the current prefix. We write the pair
            //   (curr_node->code, curr_sym), where the bit-length of the written
            //   code is the bit-length of next_code.  We now add the current
            //   prefix to the trie. Let curr_node->children[curr_sym] be a new
            //   trie node whose code is next_code. Reset curr_node to point at
            //   the root of the trie and increment the value of next_code.

            write_pair(outFile, curr_node->code, curr_sym, bitLength(next_code));
            curr_node->children[curr_sym] = trie_node_create(next_code);
            curr_node = trieRoot;
            next_code = (next_code + 1) % (MAX_CODE + 1);
        }

        //   (d) Check if next_code is equal to MAX_CODE. If it is, use
        //   trie_reset() to reset the trie to just having the root node. This
        //   reset is necessary since we have a finite number of codes.

        if (next_code == MAX_CODE) {
            next_code = START_CODE;
            trie_reset(trieRoot);
        }

        //   (e) Update prev_sym to be curr_sym.

        prev_sym = curr_sym;
    }

    // 9. After processing all the characters in infile, check if
    // curr_node points to the root trie node. If it does not, it means we
    // were still matching a prefix. Write the pair (prev_node->code,
    // prev_sym). The bit-length of the code written should be the
    // bit-length of next_code. Make sure to increment next_code and that
    // it stays within the limit of MAX_CODE. Hint: use the modulo
    // operator.

    if (curr_node != trieRoot) {
        write_pair(outFile, prev_node->code, prev_sym, bitLength(next_code));
        next_code = (next_code + 1) % (MAX_CODE + 1);
    }

    // 10. Write the pair (STOP_CODE, 0) to signal the end of compressed
    // output. Again, the bit-length of code written should be the
    // bit-length of next_code.

    write_pair(outFile, STOP_CODE, 0, bitLength(next_code));

    // 11. Make sure to use flush_pairs() to flush any unwritten, buffered
    // pairs. Remember, calls to write_pair() end up buffering them under
    // the hood. So, we have to remember to flush the contents of our
    // buffer.

    flush_pairs(outFile);

    // 12. Use close() to close infile and outfile.

    if (optVerbose) {
        extern uint64_t total_bytes_read;
        extern uint64_t total_bytes_written;

        fprintf(stdout, "Compressed file size: %lu bytes\n", total_bytes_written);
        fprintf(stdout, "Uncompressed file size: %lu bytes\n", total_bytes_read);
        fprintf(stdout, "Space saving: %.2f%%\n",
            100.0 * (1.0 - (double) total_bytes_written / (double) total_bytes_read));
    }

    close(inFile);
    close(outFile);
    return 0;
}
