/*
    4   Machine Language

    4.1 Background
    4.1.1 Machines
    Memory
        memory(address), RAM[address], or M[address]

    Processor
        CPU
    
    Registers
        high-speed local memory

    4.1.2 Languates
        binary codes and symbolic mnemonics
    
    4.1.3 Commands
    Arithmetic and Logic Operation

        ADD R2,R1,R3    // R2 <- R1+R3 where R1,R2,R3 are registers
        ADD R2,R1,foo   // R2 <- R1+foo
        AND R1,R1,R2    // R1 <- And(R1, R2)


    Memory Access
    * Direct addressing
        LOAD R1,67  // R1 <- Memory[67]
        LOAD R1,bar // R1 <- Memory[67]

    * Immediate addressing
        LOADI R1, 67 // R1 <- 67

    * Indirect addressing
        // Translation of x=foo[j] or x = *(foo + j)
        ADD R1,foo,j    // R1 <- foo + j
        LOAD* R2,R1     // R2 <- Memory[R1]
        STR R2,x        // x <- R2
        

    Flow of Control
        jump

    4.2 Hack Machine Language Specification
    4.2.1 Overview
        von Neumann platform, 16-bit machine, CPU, two memory modules, a screen and a keybord

    Memory Address Spaces
        ROM: instruction memory
        RAM: data memory
        
    Registers
        D: data register
        A: data register and address  register
        M: not a register, an implicit memory location label

        D=Memory[256]-1
        @256
        D=M-1

    Example
        C language

        // Adds 1 + ... + 100
        int i = 1;
        int sum = 0;
        while (i <= 100) {
            sum += i;
            i++;
        }

        Hack machine Language
            @i      // i refers to some memory location.
            M=1     // i=1
            @sum    // sum refers to some memory location
            M=0     // sum=0
        (LOOP)
            @i
            D=M     // D=i
            @100
            D=D-A   // D=i-100
            @END
            D;jGT   // if (i - 100) > 0 goto END
            @i
            D=M     // D=i
            @sum
            M=D+M   // sum=sum+i
            @i
            M=M+1   // i=i+1
            @LOOP
            0;JMP   // Goto LOOP
        (END)
            @END
            0;JUMP  // Infinite loop

    4.2.2 The A-instruction

        A-instruction: @value
        Binary: 0 v v v  v v v v  v v v v  v v v v

        three diffrent purpose.
        First, only way to enter a constant
        Second, data memory location
        Third, jump location

    4.2.3 The C-instruction

        C-instruction: dest=comp;jump
            either the  dest of jump fields may be empty
            if dest is empty, the "=" is ommitted, comp;jump
            if jump is empty, the ";" is ommitted, dest=comp

        Binary: 1 1 1 a  c1 c2 c3 c4  c5 c6 d1 d2  d3 j1 j2 j3

    The Computation Specification

        comp |   c1  c2  c3  c4  c5  c6 | comp
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

        1110 0011 1000 0000 // D-1
        1111 0101 0100 0000 // D|M
        1110 1110 1000 0000 // -1
        

    The Destination Specification
        d1  d2  d3  Mnemonic    Destination(where to store the computed value)
        0   0   0   null        The value is not stored anywhere
        0   0   1   M           Memory[A](memory register addressed by A)
        0   1   0   D           D register
        0   1   1   MD          Memory[A] and D register
        1   0   0   A           A regiser
        1   0   1   AM          A register and Memory[A]
        1   1   0   AD          A register and D register
        1   1   1   AMD         A register, Memory[A], and D register

        0000 0000 0000 0111 // @7
        1111 1101 1101 1000 // MD=M+1

    Conflicting Uses of the A Register
        in well-written programs a C-instruction that may cause a jump should not
        contain a reference to M

    The jump Specification
        j1      j2      j3      Mnemonic    Effect
        (out<0) (out=0) (out>0) 
        ------------------------------------------
        0       0       0       null        No jump
        0       0       1       JGT         if out > 0 jump
        0       1       0       JEQ         if out = 0 jump
        0       1       1       JGE         if out >= 0 jump
        1       0       0       JLT         if out < 0 jump
        1       0       1       JNE         if out != 0 jump
        1       1       0       JLE         if out <= 0 jump
        1       1       1       JMP         Jump

        Logic
            if Memory[3] = 5 then goto 100
            else goto 200

        Implementation
            @3
            D=M     // D=Memory[3]
            @5
            D=D-A   // D=D-5
            @100
            D;JEQ   // If D=0 goto 100
            @200
            0;JMP   // Goto 200

    4.2.4 Symbols
        Assembly commands can refer to memory locations(address) using either constants or symbols.
    * Predefined symbols
        Virtual symbols: the R0 to R15 are predefined to refer to RAM addresses 0 to 15
        Predefined pointers: SP, LCL, ARG, THIS, THAT are predefined to refer to RAM addresses 0 to 4
        I/O pointers: SCREEN and KBD to RAM address 16384(0x4000) and 24576(0x6000)
    * Label symbols
        (Xxx)
    * Variable symbols:
        @i

    4.2.5 Input/Output Handling
    Screen
        256 * 512
        Thus the pixel at row w and column c mapped on the c % 16 bit (counting from LSB to MSB)
    of the word located at RAM[16384 + r * 32 + c / 16]. 1 = black, 0 = white
        // Blacken a line of 16 pixels at the screen's top left corner
        @SCREEN
        M=-1    // -1

    Keyboard
        Whenever a key is pressed on the physical keyboard, its 16-bit ASCII code appers
    in RAM[24576]. When no key is pressed, the code 0 appears in this location.
        
        Key pressed     Code
        newline         128
        backspace       129
        left arrow      130
        up arrow        131
        right arrow     132
        down arrow      133
        home            134
        end             135
        page up         136
        page down       137
        insert          138
        delete          139
        esc             140
        f1-f12          141-152
        Special keyboard codes in the Hack platform

    4.2.6 Syntax Conventions and File Format

    Binary Code Files
        Prog.hack

    Assembly Language Files
        Prog.asm

    Constants and Symbols
        Constants must be non-negative and are always written in decimal notation.
        A user-defined symbol can be any sequence of letters, digits, underscore(_) , dot(.)  ,
        dollar sign($), and colon(:) that not begin whith a digit.

    Comments
        //

    White Space
        Space characters are ignored

    Case Conventioins
        ALL the assembly mnemonics must be written in uppercase. 
        The convention is to use uppercase for labels and lowercase for variable names.

    4.3 Perspective

    4.4 Projects
    Steps
        0. assembler and CPU emulator
        1. projects/04/mult/Mult.asm
        2. Use assembler to translate
        3. Use CPU emulator to test Mult.hack
        4. Repeat stages 1-3 for projects/04/mult/Mult.asm
*/
    

