/*
    5   Computer Architecture

    5.1 Background
    5.1.1 The Stored Program Concept
        the program's code is stored and manipulated in the computer memory, just like data,
    becoming what is known as software

    5.1.2 The von Neumann Architecture
    
         -------------         ----------        -------
        |    Memory   |       |   CPU    |      |       |
        |             |       |          |<-----| Input |
        |             |       |   ALU    |       -------               
        |             |<----->|          |       
        |    data     |       | Registers|       -------
        |      +      |       |          |----->| Output|
        | instructions|       | Control  |      |       |
         -------------         ----------        -------                              

    5.1.3 Memory
    Data Memory
        High-level programs manipulate abstract artifacts like variables, arrays, and objects.
    When translated into machine laguage, these data abstractions become series of binary numbers,
    stored in the computer's data memory.

    Instruction Memory
        When translated into machine laguage, each high-level comand becomes a series of binary words,
    representing machine laguage instructions.
    
    5.1.4 Central Processing Unit
    Arithemetic Logic Unit
        low-level arithmetic and logical operations featured by the computer.

    Registers
        every CPU is equipped with a small set of high-spped registers
    
    Control Unit     
        This instruction decoding is done by some control unit

    5.1.5 Registers
    Data registers:
        For example, when calculating the value of (a-b)*c, we must first computer and remember
    the value of (a-b), store result locally inside the CPU - in a data register.

    Addressing registers:
        memory address
    
    Program counter register:
        When executing a program, the CPU must always keep track of the address
    of the next instruction that must be fetched from the instruction memory.
    This address is kept in a special register called program coutner, or PC

    5.1.6 Input and Output
        These include screens, keyboards, printrs, scanners, network interface cards
    USB devices, and so on.


    5.2 The Hack Hardware Platform Specification

    5.2.1 Overview

    5.2.2 Central Processing Unit(CPU)
        Chip Name:  CPU                 // Central Processing Unit
        Inputs:     inM[16],            // M value input (M = contents of RAM[A])
                    instruction[16],    // instructin for execution
                    reset               // Signals whether to restart the current program(reset=1)
                                        // or continue executing the current program(reset=0)
        Outputs:    outM[16],           // M value output
                    writeM,             // write to M?
                    addressM[15],       // Address of M in data memory
                    pc[15]              // Address of next instruction
        Function:   Executes the instructino according to the Hack machine laguage specification.
                    The D and A in the language specification refer to CPU-resident registers,
                    while M refers to the memory location addressed by A(inM holds the value of this location)

                    If the insruction needs to write a value to M, the value is palaced in outM, the address is
                    placed in addressM, and the writeM bit is asserted.(When writeM=0, and value may appear in outM.)

                    If reset=1, then the CPU jumps to address 0(i.e., sets ps=0 in the next time unit) rather than 
                    to the address resulting from executing the current instruction.
    
    5.2.3 Instruction Memory
        Chip Name:  ROM32K              // 16-bit read-only 32K memory
        Input:      address[15]         // Address in the ROM
        Output:     out[16]             // Value of ROM[address]
        Function:   out=ROM[address]    // 16-bit assignment
        Comment:    The ROM is preloaded with a machine langauge program.
                    Hardware implementtations can treat the ROM as a built-in chip.
                    Software simulators must supply a mechanism for loading a program into the ROM.

    5.2.4 Data Memory
    Memory Maps
        screen memory map
        keyboard memory map

    Screen
        Chip Name:  Screen      // Memory map of the physical screen
        Inputs:     in[16],     // What to write
                    load,       // Write-enable bit
                    address[13] // Where to write
        Output:     out[16]     // Screen value at the given address
        Function:   Functions exactly like a 16-bit 8K RAM:
                    1. out(t)=Screen[address[t]](t)
                    2. If load(t-1) then Screen[address[t-1]](t) = in(t-1)

    Keyboard
        Chip Name:  Keyboard    // Memory map of the physical keyboard.
                                // Outpus the code of the currently pressed key.
        Output:     out[16]     // The ASCII code of the pressed key, or one of the special codes
        Function:   Outputs the code of the key presently pressed on the physical keyboard.
        
    Overall Memory
        Chip Name:  Memory
        Inputs:     in[16],
                    load,
                    address[15]
        Output:     out[16]
        Function:   1. out(t)=Memory[address(t)](t)
                    2. if load(t-1) then Memory[address(t-1)](t)=in(t-1)
        
    5.2.5 Computer
        Chip Name: Computer // Topmost chip in the Hack platform
        Inupt:      reset
        Function:   When reset is 0, the program stored in the computer's
                    ROM executes. When reset is 1, the execution of the program restarts. Thus,
                    to start a program's execution, reset must be pushed "up" (1) and then "down" (0)
                    
                    From this point onward the user is at the mercy of the software. In particular,
                    depending on the program's code, the screen may show some output and the user may
                    be able to interact with the computer via the keyboard.


    5.3 Implementation

    5.3.1 The Central Processing Unit
        see CPU.jpg
        Figure 5.9 Proposed CPU implementation. 
   
    Instruction Decoding
        16-bit word instuction 

        i xx a cccccc ddd jjj

        i-bit codes the instruction type, 0 for A-instruction and 1 for C-instruction
        a-bit and c-bits code the comp part
        d-bits code the dest part
        j-bits code the jump part

    Instruction Execution
        a-bit determines whether the ALU will operate on the A register input or on the Memory input
        c-bits determine which function the ALU will compute
        d-bits enable various registers to accept the ALU result
        
    Next Instruction Feching
        if jump(t) PC(t) = A(t-1)
        else       PC(t) = PC(t-1) + 1
    
    5.3.2 Memory
        RAM16, Screen, and Keyboard

    5.3.3 Computer 

                                      --------------------------------------------------
                                     |                                                  |
                                     |                            ------------          |
                                     |                           |            |         |
                                     |                           |            v         |
                 -----------         |       -------             |       -----------    |
                |           |        | inM  |       |            |      |           |   |
                |           |         ----->|       |   writeM   |      |           |   |
                |           |               |       |------------       |           |   |
                |Instruction|  instructoin  |       |   outM            |  Data     |   |
           ---->|  Memory   |-------------->|  CPU  |------------------>|  Memory   |--- 
          |     |           |               |       |   addressM        |           |   
          |     | (ROM32K)  |          ---->|       |------------------>| (Memory)  |
          |     |           |         |     |       |   pc              |           |
          |     |           |         |     |       |------------       |           |  
          |      -----------          |      -------             |       -----------            
          |                           |                          |                   
           -------------------------- | -------------------------                        
                                      |                              
                                      |
                                      |                
                                    reset

                  Figure 5.10 Proposed implementation of the topmost Computer chip.   

    5.4 Perspective
        Complexs Instruction Set Computing(CISC),
            for achieving better performance. 
        Reduced Instruction Set Computing(RISC),
            inorder to promote as fast a hardware implementation as possible

    5.5 Project
    Steps
    * Memory: RAM16K, Screen, and Keyboard
    * CPU: ALU, ARegister and DRegister
    * Instruction Memory: ROM32K
    * Computer: Memory, CPU, Instruction Memory
*/
