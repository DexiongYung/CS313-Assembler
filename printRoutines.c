
#include <stdio.h>
#include <unistd.h>
#include "printRoutines.h"

// You probably want to create a number of printing routines in this file.
// Put the prototypes in printRoutines.h

 
  /*********************************************************************
   Details on print formatting can be found by reading the man page
   for printf. The formatting rules for the disassembler output are
   given below.  Basically it should be the case that if you take the
   ouput from your disassembler, remove the initial address
   information, and then take the resulting file and load it into the
   simulator the object code produced be the simulator should match
   what your program read.  (You may have to use a .pos directive to
   indicate the offset to where the code starts.) If the simulator
   reports an assembler error then you need to fix the output of your
   dissassembler so that it is acceptable to the simulator.)

   The printing rules are as follows:
   
     1) Each line is to begin with the hex printed value of the
        address followed immediately by a ": ". Leading zeros are to
        be printed for the address which is 8 bytes long.

     2) After the ": " there are to be 22 characters before the start
        of the printing of the instruction mnemonic. In that 22
        characters the program is to print, left justified, the hex
        representation of the memory values corresponding to the
        assembler instruction and operands that were printed.

     3) The instruction field follows, and is 8 characters long. The
        instruction is to be printed left justified in those 8
        characters (%-8s). All instructions are to be printed in lower
        case.

     4) After the instruction field the first operand is to be
        printed. No extra space is needed, since the 8 characters for
        instructions is enough to leave a space between instruction
        and operands.
 
     5) If there is a second operand then there is to be a comma
        immediately after the first operand (no spaces) and then a
        single space followed by the second operand.

     6) The rules for printing operands are as follows: 

         a) Registers: A register is to be printed with the % sign and
            then its name. (.e.g. %rsp, %rax etc) Register names are
            printed in lower case.
 
         b) All numbers are to be printed in hex with the appropriate
            leading 0x. Leading zeros are to be suppressed. A value of
            zero is to be printed as "0x0". The numbers are assumed to
            be unsigned.

         c) A base displacement address is to be printed as D(reg)
            where the printing of D follows the rules in (b), and the
            printing of reg follows the rules in (a). Note there are
            no spaces between the D and "(" or between reg and the "("
            or ")".
        
         d) An address such as that used by a call or jump is to be
            printed as in (b).

         e) A constant, such as used by irmovq is to be printed as a
            number in (b) but with a "$" immediately preceeding the 0x
            without any spaces.
           
     7) The unconditional move instruction is to be printed as rrmovq,
        while the conditional move is to be printed as cmovXX, where
        XX is the condition (e.g., cmovle).

     8) The mnemonics for the instruction are to conform to those
        described in the text and accepted by the simulator.

     9) If there are any differences between what is accepted by the
        simulator and what is described in these comments or the text,
        the differences will be resolved in favour of the simulator.

    10) The arguments for the format string in the example printing
        are strictly for illustrating the spacing. You are free to
        construct the output however you want.
 
  ********************************************************************************/
 
/* This is a function to demonstrate how to do print formatting. It
 * takes the file descriptor the I/O is to be done to. You are not
 * required to use the same type of printf formatting, but you must
 * produce the same result.
 *
 * The funtion returns PRINTSUCCESS if there were no write problems,
 * and PRINTERROR otherwise.
 * 
 */
int samplePrint(FILE *out) {

  int res;

  unsigned long addr = 0x1016;
  unsigned long offset = 3484;
  char * r1 = "%rax";
  char * r2 = "%rdx";
  char * inst1 = "rrmovq";
  char * inst2 = "jne";
  char * inst3 = "irmovq";
  char * inst4 = "mrmovq";
  unsigned long destAddr = 8193;
  

  res = fprintf(out, "%016lx: %-22s%-8s%s, %s\n", 
		addr, "2002", inst1, r1, r2);

  if (res <= 0) return PRINTERROR;

  addr += 2;
  res = fprintf(out, "%016lx: %-22s%-8s%#lx\n", 
		addr, "740120000000000000", inst2, destAddr);

  if (res <= 0) return PRINTERROR;

  addr += 9;
  res = fprintf(out, "%016lx: %-22s%-8s$%#lx, %s\n", 
		addr, "30F21000000000000000", inst3, 16, r2);

  if (res <= 0) return PRINTERROR;

  addr += 10;
  res = fprintf(out, "%016lx: %-22s%-8s%#lx(%s), %s\n", 
		addr, "50020000010000000000", inst4, 65536, r2, r1); 

  if (res <= 0) return PRINTERROR;
  
  addr += 10;
  res = fprintf(out, "%016lx: %-22s%-8s%s, %s\n", 
		addr, "2020", inst1, r2, r1);

  if (res <= 0) return PRINTERROR;


  return PRINTSUCCESS;
}  
  
