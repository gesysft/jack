/*
    6   Assembler

    6.1 Background
    Symbols
        Variables
        Labels

    Symbol Resolution
        symbol table

    The Assembler
        * Parse the symbolic command into its underlying fields.
        * For each field, generate the corresponding bits int the machine language.
        * Replace all symbolic references(if any) with numeric addresses of memory locations.
        * Assemble the binary codes into a complete machine instruction.

    6.2 Hack Assembly-to-Binary Translation Specification
    6.2.1 Syntax Conventions and File Formats
        see 4.2.6

    6.2.2 Instruction
        A-instruction: @value   
            value is number or symbol 

        Binary: 0 v v v  v v v v v v v v v v v v

        C-instruction: dest=comp;jump
            if dest is empty, the "=" is ommitted, comp;jump
            if jump is empty, the ";" is ommitted, dest=comp

        Binary: 1 1 1 a  c1 c2 c3 c4  c5 c6 d1 d2  d3 j1 j2 j3

        comp|   c1  c2  c3  c4  c5  c6  | comp
        a=0 |                           | a=1
            |                           |
          0 |   1   0   1   0   1   0   | 
          1 |   1   1   1   1   1   1   |
         -1 |   1   1   1   0   1   0   |
          D |   0   0   1   1   0   0   |
          A |   1   1   0   0   0   0   | M
         !D |   0   0   1   1   0   1   |
         !A |   1   1   0   0   0   1   | !M
         -D |   0   0   1   1   1   1   |
         -A |   1   1   0   0   1   1   | -M
        D+1 |   0   1   1   1   1   1   |
        A+1 |   1   1   0   1   1   1   | M+1
        D-1 |   0   0   1   1   1   0   |
        A-1 |   1   1   0   0   1   0   | M-1
        D+A |   0   0   0   0   1   0   | D+M
        D-A |   0   1   0   0   1   1   | D-M
        A-D |   0   0   0   1   1   1   | M-D
        D&A |   0   0   0   0   0   0   | D&M
        D|A |   0   1   0   1   0   1   | D|M

        dest    d1  d2  d3
        null    0   0   0
        M       0   0   1
        D       0   1   0
        MD      0   1   1
        A       1   0   0
        AM      1   0   1
        AD      1   1   0
        AMD     1   1   1

        jump    j1  j2  j3
        null    0   0   0 
        JGT     0   0   1
        JEQ     0   1   0
        JGE     0   1   1
        JLT     1   0   0
        JNE     1   0   1
        JLE     1   1   0
        JMP     1   1   1

    6.2.3 Symbols
    Predefined Symbols
        Label   RAM address     (hexa)
        SP      0               0x0000
        LCL     1               0x0001
        ARG     2               0x0002
        THIS    3               0x0003 
        THAT    4               0x0004
        R0-R15  0-15            0x0000-f
        SCREEN  16384           0x4000
        KBD     24576           0x6000

    Label Symbols
        (Xxx) defines the symbol Xxx to refer to the instruction memory location 
    holding the next command in the program

    Variable Symbols
        starting at RAM address 16

    6.2.4 Example
        Asesembly code(Prog.asm)                Binary code(Prog.hack)              
        // Adds 1 + ... + 100                   (this line should be erased)
            @i                                  0000000000010000
            M=1     // i=1                      1110111111001000
            @sum                                0000000000010001
            M=0     // sum=0                    1110101010001000
        (LOOP)                                  (this line should be erased)
            @i                                  0000000000010000
            D=M     // D=i                      1111110000010000
            @100                                0000000001100100
            D=D-A   // D=i-100                  1110010011010000
            @END                                0000000000010010
            D;jGT   // if (i-100)>0 goto END    1110001100000000
            @i                                  0000000000010000
            D=M     // D=i                      1111110000010000
            @sum                                0000000000010001
            M=D+M   // sum=sum+i                1111000010001000
            @i                                  0000000000010000
            M=M+1   // i=i+1                    1111110111001000
            @LOOP                               0000000000000100
            0;JMP   // goto LOOP                1110101010000111
        (END)                                   (this line should be erased)
            @END                                0000000000010010
            0;JUMP  // infinite loop            1110101010000000

    6.3 Implementation
    6.3.1 The Parser Module
    6.3.2 The Code Module
    6.3.3 Assembler for Programs with No Symbols
    6.3.4 The SymbolTable Module
    6.3.5 Assembler for Programs with Symbols
        Initialization
            predefined symbols
        First Pass
            (Xxx)
        Second Pass
            @Xxx

    6.4 Perspective

    6.5 Project
    Test Programs
        Add, Max, Rect, Pong

    Steps
        two stages

    The Supplied Assembler
        Assembler.sh
*/
