
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>
#include "printInternalReg.h"

#define ERROR_RETURN -1
#define SUCCESS 0


int main(int argc, char **argv) {
  
  int machineCodeFD = -1;       // File descriptor of file with object code
  uint64_t PC = 0;                // The program counder

  // Verify that the command line has an appropriate number
  // of arguments

  if (argc < 2 || argc > 3) {
    printf("Usage: %s InputFilename [startingOffset]\n", argv[0]);
    return ERROR_RETURN;
  }

  // First argument is the file to open, attempt to open it 
  // for reading and verify that the open did occur.
  machineCodeFD = open(argv[1], O_RDONLY);

  if (machineCodeFD < 0) {
    printf("Failed to open: %s\n", argv[1]);
    return ERROR_RETURN;
  }

  // If there is a 2nd argument present it is an offset so
  // convert it to a value. This offset is the initial value the 
  // program counter is to have. The program will seek to that location
  // in the object file and begin fetching instructions from there.  
  if (3 == argc) {
    // See man page for strtol() as to why
    // we check for errors by examining errno
    errno = 0;
    PC = strtol(argv[2], NULL, 0);
    if (errno != 0) {
      perror("Invalid offset on command line");
      return ERROR_RETURN;
    }
  }

  printf("Opened %s, starting offset 0x%016llX\n", argv[1], PC);

  // Start adding your code here and comment out the line the #define EXAMPLESON line

#define EXAMPLESON 1
#ifdef  EXAMPLESON



  // The next few lines are examples of various types of output. In the comments is 
  // an instruction, the address it is at and the associated binary code that would
  // be found in the object code file at that address (offset). Your program 
  // will read that binary data and then pull it appart just like the fetch stage. 
  // Once everything has been pulled apart then a call to printReg is made to 
  // have the output printed. Read the comments in printInternalReg.c for what the 
  // various arguments are and how they are to be used. 
  
  /************************************************* 
     irmovq $1, %rsi   0x008: 30f60100000000000000
  ***************************************************/

  printReg(8, 3, 0, 1, 15, 6, 1,  1, 01, 0, 0, 0, 0, 0, 0, 0, 8+10, "irmovq");

    /************************************************* 
     je target   x034: 733f00000000000000     Note target is a label

     ***************************************************/
  printReg(0x34, 7, 3, 0, 15, 0xf, 1, 0x3f, 0x3f, 0, 0, 0, 0, 0, 0, 0, 0x34 + 9, "je");


    /************************************************* 
     nop  x03d: 10    

     ***************************************************/
  printReg(0x3d, 1, 0, 0, 15, 0xf, 0, 0x3f, 0x3f, 0, 0, 0, 0, 0, 0, 0, 0x3e + 1, "nop");



    /************************************************* 
     addq %rsi,%rdx  0x03f: 6062 

     ***************************************************/
  printReg(0x3f, 0, 0, 0, 6, 2, 0, 0x3f, 0x3f, 0, 0, 0, 0, 0, 0, 0, 0x3f + 1, "halt");


#endif


  return SUCCESS;

}

