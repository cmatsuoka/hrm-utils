
HRM Utils
=========

**Warning**: work in progress

This is my take on creating an HRM CPU emulator, capable of running an
arbitrarily defined "HRM machine code" generated from HRM assembly by
a separate assembler utility.

The machine code uses one word opcodes and operates with a 256-word code
memory and a 256-word data memory.

Example HRM assembly code from level 22 (Fibonacci Visitor)::

    hasm code    -- HUMAN RESOURCE MACHINE PROGRAM --
               
      0  60 09       BUMPUP   9
                 a:
      1  11 00       INBOX   
      2  A0 00       COPYTO   0
      3  C0 09       COPYFROM 9
      4  A0 01       COPYTO   1
      5  A0 02       COPYTO   2
                 b:
      6  10 00       OUTBOX  
      7  C0 00       COPYFROM 0
      8  40 02       SUB      2
      9  02 F7       JUMPN    a
     11  C0 01       COPYFROM 1
     12  20 02       ADD      2
     13  A0 02       COPYTO   2
     14  40 01       SUB      1
     15  A0 01       COPYTO   1
     16  00 F6       JUMP     b
               
       
