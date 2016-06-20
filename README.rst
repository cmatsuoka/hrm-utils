
HRM Utils
=========

**Warning**: work in progress

This is my take on creating an HRM CPU emulator, capable of running an
arbitrarily defined "HRM machine code" (to be generated from HRM assembly
language source code as exported by HRM by a forthcoming assembler utility).

The machine code uses one byte opcodes (with an additional byte for jump
instructions) and operates with a 256-byte code memory and a 16-word
data memory.

Example HRM assembly code from level 22 (Fibonacci Visitor)::

    hrm code     -- HUMAN RESOURCE MACHINE PROGRAM --
               
      0  89          BUMPUP   9
                 a:
      1  21          INBOX   
      2  C0          COPYTO   0
      3  E9          COPYFROM 9
      4  C1          COPYTO   1
      5  C2          COPYTO   2
                 b:
      6  20          OUTBOX  
      7  E0          COPYFROM 0
      8  62          SUB      2
      9  12 F6       JUMPN    a
     11  E1          COPYFROM 1
     12  42          ADD      2
     13  C2          COPYTO   2
     14  61          SUB      1
     15  C1          COPYTO   1
     16  10 F4       JUMP     b
               
        
