# Gladiator
Gladiator is an arena centered and data oriented systems programming language currently in development. For more information, please visit here: 
https://laturas.github.io/gladiator_writeup/

## Info
The compiler compiles the code down to x86 assembly. You'll need to use something like gcc to take the asm to a working executable.

## Current Progress
Currently the program is capable of compiling the following operations on 32bit integers:
- Returning from the main function
- Addition
- Subtraction
- Logical not (!)
- Binary not (~)
- Negation / negatives

It does this while allocating no stack space. Although the code is not optimal and often requires extra mov instructions.

Additional things the compiler supports:
- One line comments with //
- Parser error reporting with line and column numbers (though this is only currently supported with function headers)
