
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "printRoutines.h"

#define ERROR_RETURN -1
#define SUCCESS 0

// =========================================================
// INTERFACE
int bytesToY86Assembly(char *storage, int length, int start);
int conditionalMove(char upper, char lower, char rs);
int immediateToRegister(char upper, char lower, char rs, char v1, char v2, char v3, char v4);
int registerToMemory(char upper, char lower, char rs, char d1, char d2, char d3, char d4);
int memoryToRegister(char upper, char lower, char rs, char d1, char d2, char d3, char d4);
int arithmeticOperation(char upper, char lower, char rs);
int callInstruction(char upper, char lower, char d1, char d2, char d3, char d4);
char** registerRepresentation(int rA, int rB, int firstD, int secondD);

int jumpInstruction(char upper, char lower, char d1, char d2, char d3, char d4);
int returnInstruction(char upper, char lower);
int pushq(char upper, char lower, char rs);
int popq(char upper, char lwoer, char rs);

void procedurePrinterArithmeticOp(char* procName, int r1, int r2, int upBound, int lowerBound);
void procedurePrinterCondtionalMove(char* procName, int r1, int r2, int upBound, int lowerBound);


// ============================
// Additional global variables:
int start;
int res;
int writtenBytes;
long currAddr = 0;
FILE *inputFilePointer, *outputFile;

// ============================
int main(int argc, char **argv) {
    FILE *machineCode;
    // =====================
    // Additional variables:

    long fileSize;
    char *storage;
    // =====================

  // Verify that the command line has an appropriate number
  // of arguments

  if (argc < 3 || argc > 4) {
    printf("Usage: %s InputFilename OutputFilename [startingOffset]\n", argv[0]);
    return ERROR_RETURN;
  }

  // First argument is the file to read, attempt to open it 
  // for reading and verify that the open did occur.
  machineCode = fopen(argv[1], "rb");

  if (machineCode == NULL) {
    printf("Failed to open %s: %s\n", argv[1], strerror(errno));
    return ERROR_RETURN;
  }

  // Second argument is the file to write, attempt to open it 
  // for writing and verify that the open did occur.
  outputFile = fopen(argv[2], "w");

  if (outputFile == NULL) {
    printf("Failed to open %s: %s\n", argv[2], strerror(errno));
    fclose(machineCode);
    return ERROR_RETURN;
  }

  // If there is a 3rd argument present it is an offset so
  // convert it to a value. 
  if (4 == argc) {
    // See man page for strtol() as to why
    // we check for errors by examining errno
    errno = 0;
    currAddr = strtol(argv[3], NULL, 0);
    if (errno != 0) {
      perror("Invalid offset on command line");
      fclose(machineCode);
      fclose(outputFile);
      return ERROR_RETURN;
    }
      // ===========================
      // ADDED: Start initiliazation:
      start = strtol(argv[3], NULL, 10);
  } else {
      start = 0;
  }
    // ===========================

  printf("Opened %s, starting offset 0x%lX\n", argv[1], currAddr);
  printf("Saving output to %s\n", argv[2]);

  /* Comment or delete the following line and this comment before
   * handing in your final version.
   */
    
    // open output file to write in:
    outputFile = fopen(argv[2], "w");
    
    // open input (binary file) to read starting at provided offset "rb"
    inputFilePointer = fopen(argv[1], "rb");
    
    // go to end of file
    fseek(inputFilePointer, 0, SEEK_END);
    
    // find file length
    fileSize = ftell(inputFilePointer);
    
    // allocate fileSize amount of bytes of space for storage
    storage = (char*) malloc((fileSize + 1) * sizeof(char));
    
    // Began at start of file:
    rewind(inputFilePointer);
    
    // put entire file into storage
    fread(storage, fileSize, 1, inputFilePointer);
    
    // close inputFile Pointer
    fclose(inputFilePointer);
    
    // At this point, our storage contains all of the bytes from the file
    // Call byteConverter helper function
    // Converts bytes to Assembly
    bytesToY86Assembly(storage, fileSize+1, start);
    
    // free unused storage
    free(storage);
    
    printf("Got here!!!");
  
  fclose(machineCode);
  fclose(outputFile);
  return SUCCESS;
}


// TODO: Double check that everything is 64bit (not the 32 bit Y86-32 version!)
int bytesToY86Assembly(char *storage, int length, int start) {
    int i;
    for(i=start; i<length; i++) {
        char b = storage[i];
        char upperNib = b >> 4;
        char lowerNib = b & 15;
        // helperCaller(char *storage, char upperNib, char lowerNib);
        switch (upperNib) {
            case 0x0:
                currAddr += 1;
                continue;
            case 0x1:
                currAddr += 1;
                continue;
            case 0x2:
                conditionalMove(upperNib, lowerNib, storage[i+1]);
                i++;
                currAddr += 2;
                continue;
            case 0x3:
                // <<BUG HERE!>> [i+1] contains negative values
                immediateToRegister(upperNib, lowerNib, storage[i+1], storage[i+2], storage[i+3], storage[i+4], storage[i+5]);
                i+=5;
                currAddr += 6;
                continue;
            case 0x4:
                registerToMemory(upperNib, lowerNib, storage[i+1], storage[i+2], storage[i+3], storage[i+4], storage[i+5]);
                i+=5;
                currAddr += 6;
                continue;
            case 0x5:
                memoryToRegister(upperNib, lowerNib, storage[i+1], storage[i+2], storage[i+3], storage[i+4], storage[i+5]);
                i+=5;
                currAddr += 6;
            case 0x6:
                if ((arithmeticOperation(upperNib, lowerNib, storage[i+1])) > 0) {
                    i++;
                    currAddr += 2;
                    continue;
                } else {
                    printf("# Invalid instruction encountered.");
                }
            case 0x7:
                jumpInstruction(upperNib, lowerNib, storage[i+1], storage[i+2], storage[i+3],     storage[i+4]);
                i+=4;
                currAddr += 5;
                continue;
            case 0x8:
                callInstruction(upperNib, lowerNib, storage[i+1], storage[i+2], storage[i+3], storage[i+4]);
                i+=4;
                currAddr += 5;
                continue;
            case 0x9:
                returnInstruction(upperNib, lowerNib);
                currAddr+=1;
                continue;
            case 0xA:
                pushq(upperNib, lowerNib, storage[i+1]);
                i++;
                currAddr += 2;
                continue;
            case 0xB:
                popq(upperNib, lowerNib, storage[i+1]);
                i++;
                currAddr += 2;
                continue;
            default: {
                printf("# Invalid instruction encountered.");
                break;
            }
        }
    }
    return 1;
}

int conditionalMove(char upper, char lower, char rs) {
    char rA = rs >> 4;
    char rB = rs & 15;
    // Integer representation:
    int r1 = rA;
    int r2 = rB;
    int upBound = (int) upper;
    int lowerBound = (int) lower;
    
    switch (lower) {
        case 0x0:{
            // char* procedureName = "rrmovl";
            procedurePrinterCondtionalMove("rrmovl", r1, r2, upBound, lowerBound);
            break;
        }
        case 0x1: {
            // char* procedureName = "cmovle";
            procedurePrinterCondtionalMove("cmovle", r1, r2, upBound, lowerBound);
            break;
        }
        case 0x2: {
            // char* procedureName = "cmovl";
            procedurePrinterCondtionalMove("cmovl", r1, r2, upBound, lowerBound);
            break;
        }
        case 0x3: {
            // char* procedureName = "cmove";
            procedurePrinterCondtionalMove("cmove", r1, r2, upBound, lowerBound);
            break;
        }
        case 0x4: {
            // char* procedureName = "cmovs";
            procedurePrinterCondtionalMove("cmovs", r1, r2, upBound, lowerBound);
            break;
        }
        case 0x5: {
            // char* procedureName = "cmovge";
            procedurePrinterCondtionalMove("cmovge", r1, r2, upBound, lowerBound);
            break;
        }
        case 0x6: {
            // char* procedureName = "cmovg";
            procedurePrinterCondtionalMove("cmovg", r1, r2, upBound, lowerBound);
            break;
        }
        default: {
            printf("# Invalid instruction encountered.");
            break;
        }
    }
    return 1;
}

int immediateToRegister(char upper, char lower, char rs, char v1, char v2, char v3, char v4) {
    char rA = rs >> 4;
    char rB = rs & 15;
    char v1start = v1 >> 4;
    char v1end = v1 & 15;
    char v2start = v2 >> 4;
    char v2end = v2 & 15;
    char v3start = v3 >> 4;
    char v3end = v3 & 15;
    char v4start = v4 >> 4;
    char v4end = v4 & 15;
    
    // convert bytes to integer:
    int r1 = rA;
    int r2 = rB;
    int upBound = (int) upper;
    int lowerBound = (int) lower;
    
    char** regs = registerRepresentation(8, r2, 2, 4);
    char* regA = regs[0];
    char* regB = regs[1];
    
    res = fprintf(outputFile, "%016lx: %d%d%c%d%c%c%c%c%c%c%c%c%s%c%c%c%c%c%c%c%c%s\n", currAddr, upBound, lowerBound, 'F', r2, v1start, v1end, v2start, v2end, v3start, v3end, v4start, v4end, "irmovq", v4start, v4end, v3start, v3end, v2start, v2end, v1start, v1end, regB);
        if(res <= 0) {
            printf("# Invalid instruction encountered.");
            return -1;
        }
    return 1;
}

int registerToMemory(char upper, char lower, char rs, char d1, char d2, char d3, char d4) {
    char rA = rs >> 4;
    char rB = rs & 15;
    char d1start = d1 >> 4;
    char d1end = d1 & 15;
    char d2start = d2 >> 4;
    char d2end = d2 & 15;
    char d3start = d3 >> 4;
    char d3end = d3 & 15;
    char d4start = d4 >> 4;
    char d4end = d4 & 15;
    
    // convert bytes to integer:
    int r1 = rA;
    int r2 = rB;
    int upBound = (int) upper;
    int lowerBound = (int) lower;
    
    char** regs = registerRepresentation(8, r2, 2, 4);
    char* regA = regs[0];
    char* regB = regs[1];

    res = fprintf(outputFile, "%016lx: %d%d%d%d%c%c%c%c%c%c%c%c%s%s%s \n", currAddr, upBound, lowerBound, r1, r2, d1start, d1end, d2start, d2end, d3start, d3end, d4start, d4end, "rrmovq", regA, regB);
    if(res <= 0) {
        printf("# Invalid instruction encountered.");
        return -1;
    }
    return 1;
}

int memoryToRegister(char upper, char lower, char rs, char d1, char d2, char d3, char d4) {
    char rA = rs >> 4;
    char rB = rs & 15;
    char d1start = d1 >> 4;
    char d1end = d1 & 15;
    char d2start = d2 >> 4;
    char d2end = d2 & 15;
    char d3start = d3 >> 4;
    char d3end = d3 & 15;
    char d4start = d4 >> 4;
    char d4end = d4 & 15;
    
    // convert bytes to integer:
    int r1 = rA;
    int r2 = rB;
    int upBound = (int) upper;
    int lowerBound = (int) lower;
    
    // multiple buffers for this?:
    char buffer[255];
    
    char** regs = registerRepresentation(8, r2, 2, 4);
    char* regA = regs[0];
    char* regB = regs[1];
    
    
    // don't have to use sprintf could also directly put this into fprintf
    // todo: update this print message
    res = fprintf(outputFile, "%016lx: %d%d%d%d%c%c%c%c%c%c%c%c%s%s%s\n", currAddr, upBound, lowerBound, r1, r2, d1start, d1end, d2start, d2end, d3start, d3end, d4start, d4end, "mrmovq", regB, regA);
    if(res <= 0) {
            printf("# Invalid instruction encountered.");
            return -1;
    }
    return 1;
}

int arithmeticOperation(char upper, char lower, char rs) {
    char rA = rs >> 4;
    char rB = rs & 15;
    
    int r1 = rA;
    int r2 = rB;
    int upBound = (int) upper;
    int lowerBound = (int) lower;
   
    switch (lower) {
        case 0x0:
            // char* procedureName = "addq";
            procedurePrinterArithmeticOp("addq", r1, r2, upBound, lowerBound);
            break;
        case 0x1:
            //char* procedureName = "subq";
            procedurePrinterArithmeticOp("subq", r1, r2, upBound, lowerBound);
            break;
        case 0x2:
            // char* procedureName = "andq";
            procedurePrinterArithmeticOp("andq", r1, r2, upBound, lowerBound);
            break;
        case 0x3:
            // char* procedureName = "xorq";
            procedurePrinterArithmeticOp("xorq", r1, r2, upBound, lowerBound);
            break;
        case 0x4:
            // char* procedureName = "null";
            procedurePrinterArithmeticOp("null", r1, r2, upBound, lowerBound);
            break;
        case 0x5:
            // char* procedureName = "divq";
            procedurePrinterArithmeticOp("divq", r1, r2, upBound, lowerBound);
            break;
        case 0x6:
            // char* procedureName = "modq";
            procedurePrinterArithmeticOp("modq", r1, r2, upBound, lowerBound);
            break;
        default: {
            // TODO:
            printf("# Invalid instruction encountered.");
            break;
        }
    }
    return 1;
}

// CALL
int callInstruction(char upper, char lower, char d1, char d2, char d3, char d4) {
    char d1start = d1 >> 4;
    char d1end = d1 & 15;
    char d2start = d2 >> 4;
    char d2end = d2 & 15;
    char d3start = d3 >> 4;
    char d3end = d3 & 15;
    char d4start = d4 >> 4;
    char d4end = d4 & 15;
    
    // convert bytes to integer:
    int upBound = (int) upper;
    int lowerBound = (int) lower;
    
    res = fprintf(outputFile, "%016lx: %d%d%c%c%c%c%c%c%c%c%s\n", currAddr, upBound, lowerBound, d1start, d1end, d2start, d2end, d3start, d3end, d4start, d4end, "call");
    if(res <= 0) {
        printf("# Invalid instruction encountered.");
        return -1;
    }
    return 1;
}


int jumpInstruction(char upper, char lower, char d1, char d2, char d3, char d4) {
    char d1start = d1 >> 4;
    char d1end = d1 & 15;
    char d2start = d2 >> 4;
    char d2end = d2 & 15;
    char d3start = d3 >> 4;
    char d3end = d3 & 15;
    char d4start = d4 >> 4;
    char d4end = d4 & 15;
    
    // convert bytes to integer:
    int upBound = (int) upper;
    int lowerBound = (int) lower;
    
    switch(lower){
        case 0x0: {
            res = fprintf(outputFile, "%016lx: %d%d%c%c%c%c%c%c%c%c%s\n", currAddr, upBound, lowerBound, d1start, d1end, d2start, d2end, d3start, d3end, d4start, d4end, "jmp");
            if(res <= 0) {
                printf("# Invalid instruction encountered.");
                return -1;
            }
            break;
        }
        case 0x1: {
            res = fprintf(outputFile, "%016lx: %d%d%c%c%c%c%c%c%c%c%s\n", currAddr, upBound, lowerBound, d1start, d1end, d2start, d2end, d3start, d3end, d4start, d4end, "jle");
            if(res <= 0) {
                printf("# Invalid instruction encountered.");
                return -1;
            }
            break;
        }
        case 0x2: {
            res = fprintf(outputFile, "%016lx: %d%d%c%c%c%c%c%c%c%c%s\n", currAddr, upBound, lowerBound, d1start, d1end, d2start, d2end, d3start, d3end, d4start, d4end, "jl");
            if(res <= 0) {
                printf("# Invalid instruction encountered.");
                return -1;
            }
            break;
        }
        case 0x3: {
            res = fprintf(outputFile, "%016lx: %d%d%c%c%c%c%c%c%c%c%s\n", currAddr, upBound, lowerBound, d1start, d1end, d2start, d2end, d3start, d3end, d4start, d4end, "je");
            if(res <= 0) {
                printf("# Invalid instruction encountered.");
                return -1;
            }
            break;
        }
        case 0x4: {
            res = fprintf(outputFile, "%016lx: %d%d%c%c%c%c%c%c%c%c%s\n", currAddr, upBound, lowerBound, d1start, d1end, d2start, d2end, d3start, d3end, d4start, d4end, "jn");
            if(res <= 0) {
                printf("# Invalid instruction encountered.");
                return -1;
            }
            break;
        }
        case 0x5: {
            res = fprintf(outputFile, "%016lx: %d%d%c%c%c%c%c%c%c%c%s\n", currAddr, upBound, lowerBound, d1start, d1end, d2start, d2end, d3start, d3end, d4start, d4end, "jge");
            if(res <= 0) {
                printf("# Invalid instruction encountered.");
                return -1;
            }
            break;
        }
        case 0x6: {
            res = fprintf(outputFile, "%016lx: %d%d%c%c%c%c%c%c%c%c%s\n", currAddr, upBound, lowerBound, d1start, d1end, d2start, d2end, d3start, d3end, d4start, d4end, "jg");
            if(res <= 0) {
                printf("# Invalid instruction encountered.");
                return -1;
            }
            break;
        }
        default: {
            printf("# Invalid instruction encountered.");
            break;
        }
    }
    return 1;
}

    
    int returnInstruction(char upper, char lower) {
        res = fprintf(outputFile, "%016lx: %d%d%s\n", currAddr, (int)upper, (int)lower, "ret");
        if(res <= 0) {
            printf("# Invalid instruction encountered.");
            return -1;
        }
        return 1;
    }

    int pushq(char upper, char lower, char rs) {
        char rA = rs >> 4;
        char rB = rs & 15;
        
        // convert bytes to integer:
        int r1 = rA;
        int r2 = rB;
        int upBound = (int) upper;
        int lowerBound = (int) lower;
        
        char** regs = registerRepresentation(r1, 8, 2, 4);
        char* regA = regs[0];
        char* regB = regs[1];
        
        res = fprintf(outputFile, "%016lx: %d%d%s%s\n", currAddr, (int)upper, (int)lower, "pushq", regA);
        if(res <= 0) {
            printf("# Invalid instruction encountered.");
            return -1;
        }
        return 1;
    }

    int popq(char upper, char lower, char rs){
        char rA = rs >> 4;
        char rB = rs & 15;
        
        // convert bytes to integer:
        int r1 = rA;
        int r2 = rB;
        int upBound = (int) upper;
        int lowerBound = (int) lower;
        
        char** regs = registerRepresentation(r1, 8, 2, 4);
        char* regA = regs[0];
        char* regB = regs[1];
        
        res = fprintf(outputFile, "%016lx: %d%d%d%c%s%s\n", currAddr, (int)upper, (int)lower, r1, 'F', "popq", regA);
        if(res <= 0) {
            printf("# Invalid instruction encountered.");
            return -1;
        }
        return 1;
    }

void procedurePrinterArithmeticOp(char* procName, int r1, int r2, int upBound, int lowerBound) {
    char** regs = registerRepresentation(r1, r2, 2, 4);
    char* regA = regs[0];
    char* regB = regs[1];
    /*
     
     res = fprintf(outputFile, "%016lx: %d%d%c%d%c%c%c%c%c%c%c%c%s%s%s\n", currAddr, upBound, lowerBound, 'F', r2, v1start, v1end, v2start, v2end, v3start, v3end, v4start, v4end, "irmovq", regA, regB);
     if(res <= 0) {
     printf("# Invalid instruction encountered.");
     return -1;
     }
     return 1;

     */
    res = fprintf(outputFile,  "%016lx: %d%d%d%d%s%s%s\n", currAddr, upBound, lowerBound, r1, r2, procName, regA, regB);
    if(res <= 0) {
        printf("# Invalid instruction encountered.");
        return;
    }
    return;
}

void procedurePrinterCondtionalMove(char* procName, int r1, int r2, int upBound, int lowerBound) {
    char** regs = registerRepresentation(r1, r2, 2, 4);
    char* regA = regs[0];
    char* regB = regs[1];
    
    res = fprintf(outputFile, "%016lx: %d%d%d%d%s%s%s\n", currAddr, upBound, lowerBound, r1, r2, procName, regA, regB);
    if(res <= 0) {
        printf("# Invalid instruction encountered.");
        return;
    }
    return;
}



char** registerRepresentation(int rA, int rB, int firstD, int secondD) {
    int i;
    char** str = malloc(firstD*sizeof(char*));
    for (i = 0; i < 2; i++) {
        str[i] = malloc(secondD*sizeof(char*));
    }
    switch (rA) {
        case 0x0:
            strcpy(str[0],"%rax");
            break;
        case 0x1:
            strcpy(str[0], "%rcx");
            break;
        case 0x2:
            strcpy(str[0], "%rdx");
            break;
        case 0x3:
            strcpy(str[0], "%rbx");
            break;
        case 0x4:
            strcpy(str[0], "%rsp");
            break;
        case 0x5:
            strcpy(str[0], "%rbp");
            break;
        case 0x6:
            strcpy(str[0], "%rsi");
            break;
        case 0x7:
            strcpy(str[0], "%rdi");
            break;
        case 0x8:
            strcpy(str[0], "%r8");
            break;
        case 0x9:
            strcpy(str[0], "%r9");
            break;
        case 0xA:
            strcpy(str[0], "r10");
            break;
        case 0xB:
            strcpy(str[0], "r11");
            break;
        case 0xC:
            strcpy(str[0], "r12");
            break;
        case 0xD:
            strcpy(str[0], "r13");
            break;
        case 0xE:
            strcpy(str[0], "r14");
            break;
        default:
            strcpy(str[0], "Error - no register");
            break;
    }
    
    switch (rB) {
        case 0x0:
            strcpy(str[1],"%rax");
            break;
        case 0x1:
            strcpy(str[1], "%rcx");
            break;
        case 0x2:
            strcpy(str[1], "%rdx");
            break;
        case 0x3:
            strcpy(str[1], "%rbx");
            break;
        case 0x4:
            strcpy(str[1], "%rsp");
            break;
        case 0x5:
            strcpy(str[1], "%rbp");
            break;
        case 0x6:
            strcpy(str[1], "%rsi");
            break;
        case 0x7:
            strcpy(str[1], "%rdi");
            break;
        case 0x8:
            strcpy(str[1], "%r8");
            break;
        case 0x9:
            strcpy(str[1], "%r9");
            break;
        case 0xA:
            strcpy(str[1], "r10");
            break;
        case 0xB:
            strcpy(str[1], "r11");
            break;
        case 0xC:
            strcpy(str[1], "r12");
            break;
        case 0xD:
            strcpy(str[1], "r13");
            break;
        case 0xE:
            strcpy(str[1], "r14");
            break;
        default:
            strcpy(str[1], "Error - no register");
            break;
    }
    
    return str;
}


    
    
    
    
    
    
    
    
    

