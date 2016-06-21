
HRM Utils
=========

**Warning**: work in progress

This is my take on creating an HRM CPU emulator, capable of running an
arbitrarily defined "HRM machine code" (to be generated from HRM assembly
language source code as exported by HRM by a forthcoming assembler utility).

The machine code uses one word opcodes and operates with a 256-word code
memory and a 256-word data memory.

Example HRM assembly code from level 22 (Fibonacci Visitor)::

    hrm code     -- HUMAN RESOURCE MACHINE PROGRAM --
               
      0  03 09       BUMPUP   9
                 a:
      1  41 00       INBOX   
      2  05 00       COPYTO   0
      3  06 09       COPYFROM 9
      4  05 01       COPYTO   1
      5  05 02       COPYTO   2
                 b:
      6  40 00       OUTBOX  
      7  06 00       COPYFROM 0
      8  02 02       SUB      2
      9  22 F7       JUMPN    a
     11  06 01       COPYFROM 1
     12  01 02       ADD      2
     13  05 02       COPYTO   2
     14  02 01       SUB      1
     15  05 01       COPYTO   1
     16  20 F6       JUMP     b
               
       
