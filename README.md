# Building a compiler
I want to build a compiler for a long time. After looking for minimal examples, as I don't read documentation, I found some examples.

# The Plan
New plan. I start with the crenshaw course because i will make the asm code there. Then I can reuse this asm in the virtual compiler to make it real. Then i'll add things to this compiler to make it work with th article compiler

I'll take the tiny c by Feely, and first implement the VM to compile to x84-64 ASM. Because the VM is so simple I think I can do this, It is also a good excercise to get my x84 ASM back up to speed. Then I hope to extend with custom variables and pointers (for inspiration for this I hope to use Ron Cain's work).

## VM functions to ASM-ize
 - [ ] IFETCH
 - [ ] ISTORE
 - [ ] IPUSH
 - [ ] IPOP
 - [ ] IADD
 - [ ] ISUB
 - [ ] ILT
 - [ ] JMP
 - [ ] JZ
 - [ ] JNZ

## Functionality to add
 - [ ] More boolean operations
 - [ ] Variables
 - [ ] Pointers
 - [ ] 
 - [ ] 
 - [ ] 
 - [ ] 

# Sources
## Tiny c by Marc Feeley (2001) (virtual folder)
This is an educational example of a small c dialect that compiles to a custom vm that works stack based. I like the lexer and parser that are used. It has a big limitation that it only has 26 int variables [a-z].

## Tiny c by Ron Cain (1980) (article folder)
This is a minimal c implementation for 8080 based systems. It has what I think the minimal syntax, and is also self compiling (which is something I hope to achieve). It compiles to assembly, but the parser, and lexer are way more intertwined, and tbh too complex for me to fully understand. It doesn't have enums, So I hope to add that.