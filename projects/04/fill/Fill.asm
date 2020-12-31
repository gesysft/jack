// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, the
// program clears the screen, i.e. writes "white" in every pixel.

(BEGIN)
    @i
    M=0
(LOOP)
    @i
    D=M	
    @8192
    D=D-A
    @BEGIN
    D;JGE   // if i-8192 >= 0 then goto BEGIN
    @KBD
    D=M
    @IF_FALSE
    D;JNE
    @i
    D=M
    @SCREEN
    A=D+A
    M=0     // writes white
    @IF_END
    0;JMP
(IF_FALSE)
    @i
    D=M
    @SCREEN
    A=D+A
    M=-1    // writes black
(IF_END)
    @i
    M=M+1   // i=i+1
    @LOOP
    0;JMP
