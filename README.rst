
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

    -- HUMAN RESOURCE MACHINE PROGRAM --
    
        BUMPUP   9
    a:
        INBOX   
        COPYTO   0
        COPYFROM 9
        COPYTO   1
        COPYTO   2
    b:
        OUTBOX  
        COPYFROM 0
        SUB      2
        JUMPN    a
        COPYFROM 1
        ADD      2
        COPYTO   2
        SUB      1
        COPYTO   1
        JUMP     b
    
    
