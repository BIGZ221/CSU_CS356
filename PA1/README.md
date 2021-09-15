# PA1 Block and Stream Ciphers

## Zachary Fuchs

## Makefile

- `make build` - compiles the main.cpp file into `main`
- `make package` - packages main.cpp, README.md, and Makefile into `Zachary-Fuchs-PA1.tar`
- `make clean` - removes the `main` executable

## Usage

- Program accepts 5 arguments

1. Cipher type: Either `B` or `S` (`B` is for block, `S` is for stream)
1. Input file name: Data to be encrypted/decrypted will be read from this file, including any newlines
1. Output file name: (if the file does not exist, one is created with this name)
1. Key file name: Key will be read from this file, ignoring the last newline if there is one
1. Mode of operation: Either `E` or `D` (in stream mode either can be used to encrypt or decrypt)

### Example `./main B infile outfile keyfile E`
