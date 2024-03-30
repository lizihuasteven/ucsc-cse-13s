# CruzID: zli487

# Zihua Li

# Assignment 6

## make executables by running 'make all'

### executable 'decode'

SYNOPSIS

   Decompresses files with the LZ78 decompression algorithm.
   
   Used with files compressed with the corresponding encoder.


USAGE

   ./decode [-vh] [-i input] [-o output]


OPTIONS

   -v          Display decompression statistics
   
   -i input    Specify input to decompress (stdin by default)
   
   -o output   Specify output of decompressed input (stdout by default)
   
   -h          Display program usage


### executable 'encode'

SYNOPSIS

   Compresses files using the LZ78 compression algorithm.
   
   Compressed files are decompressed with the corresponding decoder.
   

USAGE

   ./encode [-vh] [-i input] [-o output]


OPTIONS

   -v          Display compression statistics
   
   -i input    Specify input to compress (stdin by default)
   
   -o output   Specify output of compressed input (stdout by default)
   
   -h          Display program help and usage

