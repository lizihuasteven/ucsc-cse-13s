#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "io.h"
#include "code.h"
#include "endian.h"
#include "word.h"

extern char const *ARGV0;
extern int optAscii;

uint64_t total_syms; // To count the symbols processed.
uint64_t total_bits; // To count the bits processed.

uint64_t total_bytes_read = 0;
uint64_t total_bytes_written = 0;

// 4.4 I/O

// It is also a requirement that your programs, encode and decode,
// perform efficient I/O. Reads and writes will be done 4KB, or a
// block, at a time, which implicitly requires that you buffer
// I/O. Buffering is the act of storing data into a buffer, which you
// can think of as an array of bytes. You will be implementing an I/O
// module for this assignment, and its API is explained in the
// following sections.

// 4.4.1 FileHeader

// 1 struct FileHeader {
// 2   uint32_t magic;
// 3   uint16_t protection;
// 4 };

// This is the struct definition for the file header, which contains
// the magic number for your program and the protection bit mask for
// the original file. The file header is the first thing that appears
// in a compressed file. The magic number field, magic, serves as a
// unique identifier for files compressed by encode. decode should
// only be able to decompress files which have the correct magic
// number. This magic number is 0xBAADBAAC.

// Before writing the file header to the compressed file using
// write_header(), you must swap the endianness of the fields if
// necessary since interoperability is required. If your program is
// run on a system using big endian, the fields must be swapped to
// little endian, since little endian is canonical. A module
// specifically for handling endianness will be provided.

// Note that the output file in which you write to must have the same
// protection bits as the original file. Like in assignment 4, you
// will make use of fstat() and fchmod().

// All reads and writes in this program must be done using the system
// calls read() and write(), which means that you must use the system
// calls open() and close() to get your file descriptors. As stated
// earlier, all reads and writes must be performed in efficient blocks
// of 4KB. You will want to use two static 4KB uint8_t arrays to serve
// as buffers: one to store binary pairs and the other to store
// characters. Each of these buffers should have an index, or a
// variable, to keep track of the current byte or bit that has been
// processed.

// 4.4.2 int read_bytes(int infile, uint8_t *buf, int to_read)

// This will be a useful helper function to perform reads. As you may
// know, the read() syscall does not always guarantee that it will
// read all the bytes specified. For example, a call could be issued
// to read a block of bytes, but it might only read half a block. So,
// we write a wrapper function to loop calls to read() until we have
// either read all the bytes that were specified (to_read), or there
// are no more bytes to read. The number of bytes that were read are
// returned. You should use this function whenever you need to perform
// a read.

static uint8_t readBuffer[BLOCK];
static uint8_t const *cRdPtr = &readBuffer[BLOCK];
static uint8_t const *eRdPtr = &readBuffer[BLOCK];

int read_bytes(int infile, uint8_t *buf, int to_read) {
    // fprintf(stderr,
    //         "%s: read_bytes(): read(infile=%d, buf=%p, to_read(%d))",
    //         ARGV0,
    //         infile,
    //         (void *) buf,
    //         to_read
    //        );

    // Keep track of whether we saw a 0-byte return from infile, to
    // avoid requiring multiple ctrl-d's from stdin.

    static int saw_end_of_file = 0;
    if (saw_end_of_file) {
        return 0;
    }

    // Keep track of how many bytes we've (already) read, because we
    // want to have read all to_read bytes.

    int already_read = 0;

    // While there's still more to be read...

    while (already_read < to_read) {

        // Attempt to read in any remaining requested bytes.

        int read_this_time = 0;
        if (infile != 0) {

            // For any non-stdin file, attempt to read all remaining bytes
            // that have not yet been read.

            read_this_time = read(infile, buf, to_read - already_read);
        } else {

            // When the file is stdin, read a character at a time, so that
            // we can recognize a ctrl-d without a preceeding new-line.

            read_this_time = read(infile, buf, 1);
        }

        // Errors are bad.

        if (read_this_time == -1) {
            fprintf(stderr, "%s: read_bytes(): error during read().\n", ARGV0);
            perror(ARGV0);
            exit(1);
        }

        // If read() == 0, there're no more to read, and we're done.

        if (read_this_time == 0) {
            saw_end_of_file = 1;
            break;
        }

        // We added more bytes.

        already_read += read_this_time;
        buf += read_this_time;
    }

    // Remember the total of how many bytes we've read, for statistics.

    total_bytes_read += already_read;

    // fprintf(stderr, ", total_bytes_read=%llu -> %d\n", total_bytes_read, already_read);

    // Return the count of what we successfully read.

    return already_read;
}

// 4.4.3 int write_bytes(int outfile, uint8_t *buf, int to_write)

// This function is very much the same as read_bytes(), except that it
// is for looping calls to write(). As you may imagine, write() isn’t
// guaranteed to write out all the specified bytes (to_write), and so
// we loop until we have either written out all the bytes specified,
// or no bytes were written. The number of bytes written out is
// returned.  You should use this function whenever you need to
// perform a write.

static uint8_t writeBuffer[BLOCK];
static uint8_t *cWrPtr = writeBuffer;

int write_bytes(int outfile, uint8_t *buf, int to_write) {
    int already_written = 0;
    while (already_written < to_write) {
        int written_this_time = write(outfile, buf, to_write - already_written);
        if (written_this_time == -1) {
            fprintf(stderr, "%s: write_bytes(): error during write().\n", ARGV0);
            perror(ARGV0);
            exit(1);
        }
        if (written_this_time == 0) {
            break;
        }
        already_written += written_this_time;
    }
    total_bytes_written += already_written;
    return already_written;
}

// 4.4.4 void read_header(int infile, FileHeader *header)

// This reads in sizeof(FileHeader) bytes from the input file. These
// bytes are read into the supplied header. Endianness is swapped if
// byte order isn’t little endian. Along with reading the header, it
// must verify the magic number.

void read_header(int infile, FileHeader *header) {
    int bytesRead = read_bytes(infile, (uint8_t *) header, sizeof(FileHeader));
    if (bytesRead != sizeof(FileHeader)) {
        fprintf(stderr, "%s: read_header(): Unable to read full FileHeader.\n", ARGV0);
        exit(1);
    }
    if (big_endian()) {
        header->magic = swap32(header->magic);
        header->protection = swap16(header->protection);
    }
    if (header->magic != 0xBAADBAAC) {
        fprintf(stderr, "%s: read_header(): FileHeader magic number is incorrect.\n", ARGV0);
        exit(1);
    }
}

// 4.4.5 void write_header(int outfile, FileHeader *header)

// Writes sizeof(FileHeader) bytes to the output file. These bytes are
// from the supplied header. Endianness is swapped if byte order isn’t
// little endian.

void write_header(int outfile, FileHeader *header) {
    if (big_endian()) {
        header->magic = swap32(header->magic);
        header->protection = swap16(header->protection);
    }
    if (header->magic != 0xBAADBAAC) {
        fprintf(stderr, "%s: write_header(): FileHeader magic number is incorrect.\n", ARGV0);
        exit(1);
    }

    uint8_t *fileHeader = (uint8_t *) header;
    for (size_t i = 0; i < sizeof(FileHeader); i++) {
        *cWrPtr++ = *fileHeader++;
        if (cWrPtr == &writeBuffer[BLOCK]) {
            write_bytes(outfile, writeBuffer, BLOCK);
            cWrPtr = writeBuffer;
        }
    }

    /* int bytesWritten = write_bytes(outfile, (uint8_t *) header, sizeof(FileHeader)); */
    /* if (bytesWritten != sizeof(FileHeader)) { */
    /*   fprintf(stderr, "%s: write_header(): Unable to write full FileHeader.\n", ARGV0); */
    /*   exit(1); */
    /* } */
}

// 4.4.6 bool read_sym(int infile, uint8_t *sym)

// An index keeps track of the currently read symbol in the
// buffer. Once all symbols are processed , another block is read. If
// less than a block is read , the end of the buffer is
// updated. Returns true if there are symbols to be read , false
// otherwise.

bool read_sym(int infile, uint8_t *sym) {
    if (cRdPtr == eRdPtr) {
        int nBytesRead = read_bytes(infile, readBuffer, sizeof(readBuffer));
        cRdPtr = readBuffer;
        eRdPtr = cRdPtr + nBytesRead;
        if (cRdPtr == eRdPtr) {
            return false;
        }
    }

    *sym = *cRdPtr++;
    return true;
}

// 4.4.7 void write_pair(int outfile, uint16_t code, uint8_t sym, int bitlen)

// “Writes” a pair to outfile. In reality, the pair is buffered. A
// pair is comprised of a code and a symbol. The bits of the code are
// buffered first, starting from the LSB. The bits of the symbol are
// buffered next, also starting from the LSB. The code buffered has a
// bit-length of bitlen. The buffer is written out whenever it is
// filled.

static uint8_t wrByteMsk = 1;
static uint8_t wrByteVal = 0;

void write_bits(int outfile, uint16_t value, int bitlen) {
    if (optAscii) {
        *cWrPtr++ = ' ';
        if (cWrPtr == &writeBuffer[BLOCK]) {
            write_bytes(outfile, writeBuffer, BLOCK);
            cWrPtr = writeBuffer;
        }
        while (bitlen--) {
            *cWrPtr++ = value & 1 ? '1' : '0';
            value >>= 1;
            if (cWrPtr == &writeBuffer[BLOCK]) {
                write_bytes(outfile, writeBuffer, BLOCK);
                cWrPtr = writeBuffer;
            }
        }
        return;
    }

    // Write [bitlen] bits from value, one bit at a time.

    uint16_t wrValueMask = 1;

    while (bitlen--) {

        // If the byte mask is (now) 0, then we've completed writing a
        // byte's worth of bits.  Write out that byte, flushing the buffer
        // if now full, and get ready for the next bit from value.

        if (!wrByteMsk) {
            *cWrPtr++ = wrByteVal;
            if (cWrPtr == &writeBuffer[BLOCK]) {
                write_bytes(outfile, writeBuffer, BLOCK);
                cWrPtr = writeBuffer;
            }
            wrByteMsk = 1;
            wrByteVal = 0;
        }

        // If the next value bit is a 1, OR a 1 bit into the byte to be
        // output.

        if (value & wrValueMask) {
            wrByteVal |= wrByteMsk;
        }
        // fprintf(stderr, value & wrValueMask ? "1" : "0");
        fflush(stderr);

        // Shift the masks (left) for the next position.

        wrValueMask <<= 1;
        wrByteMsk <<= 1;
        // fprintf(stderr, wrByteMsk ? "" : "\n");
        fflush(stderr);
    }
}

void write_pair(int outfile, uint16_t code, uint8_t sym, int bitlen) {
    // fprintf(stderr, "pair(code=%04x[%2d], sym=%02x)\n", code, bitlen, sym);
    // fprintf(stderr, "write_pair(outfile=%d, code=%d, sym=%c, bitlen=%d)\n", outfile, code, sym, bitlen);
    write_bits(outfile, code, bitlen);
    write_bits(outfile, sym, 8);
}

// 4.4.8 void flush_pairs(int outfile)

// Writes out any remaining pairs of symbols and codes to the output
// file.

void flush_pairs(int outfile) {
    if (!optAscii) {
        if (wrByteMsk) {
            *cWrPtr++ = wrByteVal;
            if (cWrPtr == &writeBuffer[BLOCK]) {
                write_bytes(outfile, writeBuffer, BLOCK);
                cWrPtr = writeBuffer;
            }
        }
    }
    write_bytes(outfile, writeBuffer, cWrPtr - writeBuffer);
    cWrPtr = writeBuffer;
}

// 4.4.9 bool read_pair(int infile, uint16_t *code, uint8_t *sym, int bitlen)

// “Reads” a pair (code and symbol) from the input file. The “read”
// code is placed in the pointer to code (e.g. *code = val) The “read”
// symbol is placed in the pointer to sym (e.g. *sym = val). In
// reality, a block of pairs is read into a buffer. An index keeps
// track of the current bit in the buffer. Once all bits have been
// processed, another block is read. The first bitlen bits are the
// code, starting from the LSB. The last 8 bits of the pair are the
// symbol, starting from the LSB. Returns true if there are pairs left
// to read in the buffer, else false. There are pairs left to read if
// the read code is not STOP_CODE.

static uint8_t rdByteMsk = 0;
static uint8_t rdByteVal = 0;

void read_bits(int infile, uint16_t *value, int bitlen) {

    // We're reading a value starting with the least significant bit
    // first, and starting with a value of 0 (as we'll be turning bits
    // on, not off).

    uint16_t rdValueMask = 1;
    uint16_t rdValue = 0;

    if (optAscii) {
        if (cRdPtr == &readBuffer[BLOCK]) {
            read_bytes(infile, readBuffer, BLOCK);
            cRdPtr = readBuffer;
        }
        if (*cRdPtr != ' ') {
            fprintf(stderr, "%s: read_bits(): Expected a ' ', got '%c'.\n", ARGV0, *cRdPtr);
        } else {
            cRdPtr++;
        }

        for (int b = 0; b < bitlen; b++) {
            if (cRdPtr == &readBuffer[BLOCK]) {
                read_bytes(infile, readBuffer, BLOCK);
                cRdPtr = readBuffer;
            }

            uint16_t sym = *cRdPtr++;
            switch (sym) {
            case '0': break;

            case '1': rdValue |= rdValueMask; break;

            default:
                fprintf(stderr, "%s: read_bits(): Expected a '0' or '1', got '%c'.\n", ARGV0, sym);
            }
            rdValueMask <<= 1;
        }

        *value = rdValue;
        return;
    }

    // Read [bitlen] bits from value, one bit at a time.

    while (bitlen--) {

        // If the byte mask is (now) 0, then we've completed reading a
        // byte's worth of bits.  Read the next byte, filling the buffer
        // if now empty, and get ready for the next bit for value.

        if (!rdByteMsk) {
            if (cRdPtr == &readBuffer[BLOCK]) {
                read_bytes(infile, readBuffer, BLOCK);
                cRdPtr = readBuffer;
            }
            rdByteMsk = 1;
            rdByteVal = *cRdPtr++;
        }

        // If the next value bit is a 1, OR a 1 bit into the byte to be
        // output.

        if (rdByteVal & rdByteMsk) {
            rdValue |= rdValueMask;
        }
        // fprintf(stderr, rdByteVal & rdByteMsk ? "1" : "0");
        fflush(stderr);

        // Shift the masks (left) for the next position.

        rdValueMask <<= 1;
        rdByteMsk <<= 1;
        // fprintf(stderr, rdByteMsk ? "" : "\n");
        fflush(stderr);
    }

    *value = rdValue;
}

bool read_pair(int infile, uint16_t *code, uint8_t *sym, int bitlen) {
    uint16_t value;
    read_bits(infile, &value, bitlen);
    *code = value;
    read_bits(infile, &value, 8);
    *sym = (uint8_t) value;

    // fprintf(stderr, "pair(code=%04x[%2d], sym=%02x)\n", *code, bitlen, *sym);
    // fprintf(stderr, "read_pair(outfile=%d, *code=%d, *sym=%c, bitlen=%d)\n", infile, *code, *sym, bitlen);
    return *code != STOP_CODE;
}

// 4.4.10 void write_word(int outfile, Word *w)

// “Writes” a pair to the output file. Each symbol of the Word is
// placed into a buffer. The buffer is written out when it is filled.

void write_word(int outfile, Word *w) {
    // fprintf(stderr, "write_word(outfile=%d, w->{ syms=\"%s\", len=%d })\n", outfile, w->syms, w->len);
    write_bytes(outfile, w->syms, w->len);
}

// 4.4.11 void flush_words(int outfile)

// Writes out any remaining symbols in the buffer to the outfile.

void flush_words(int outfile) {
    write_bytes(outfile, writeBuffer, cWrPtr - writeBuffer);
    cWrPtr = writeBuffer;
}
