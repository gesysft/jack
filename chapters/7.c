/*
    7   Virtual Machine 1: Stack Arithmetic
    7.1 Background
    7.1.1 The Virtual Machine Paradigm
        The first program, still termed compiler, translates the high-level code into
    intermediate VM instructions, while the second  program translates this VM code
    into the machine langauge of the target platform.
        Java compilers are also two-tiered, generating commmands in a bytecode langauge that
    runs on the JVM virtual machine(alse called the Java Runtime Enviroment). 
        .NET required compilers to generate code written in an intermediate language(IL) that
    runs on a virutal machine called CLR (Common Language Runtime).

    7.1.2 The Stack Machine Model
        Further, any program, written in any programming language, can be translated into
    an equivalent stack machine program.

    Elements Stack Operations

        ----------------------          -----------------------   
            Stack       Memory              Stack       Memory      
            -----       ------              -----       ------     
            121         ...                 121         ...       
            5         a 6       push b      5         a 6        
            17          ...                 17          ...     
        sp->          b 108                 108       b 108   
                        ...             sp->            ...                             
        ----------------------          ----------------------

        ----------------------          ----------------------
            Stack       Memory              Stack       Memory
            -----       ------              -----       ------
            121         ...                 121         ...
            5         a 6       pop a       5         a 17
            17          ...             sp->            ...
        sp->          b 108                           b 108
                        ...                             ...
        ----------------------          ----------------------


    Stack  Arithmetic
            // d=(2-x)*(y+5)
            
            Memory      Stack
            ------      -----               -----               -----
            ...     sp->        push 2      2       push x      2       sub
          x 5                           sp->                    5
          y 9                                               sp->
            ...

                        -----               -----               -----
                        -3      push y      -3      push 5      -3      add
                    sp->                    9                   9
                                        sp->                    5
                                                            sp->

                                                                Stack       Memory
                        -----               ----                -----       ------
                        -3      mult        -42     pop d   sp->            ...
                        14              sp->                              x 5
                    sp->                                                  y 9
                                                                            ...
                                                                          d -42
                                                                            ...
        
     
    7.2 VM Specification, Part 1
    7.2.1 General
        The virtual machine is stack-based. four types of commands:
        * Arithmetic commands perform arithmetic and logical operations on the stack.
        * Memory Access commands transfer data between the stack and virtual memory segments.
        * Program flow commands facilitate conditional and unconditional branching operatioins.    
        * Function calling commands call functions and return from them.

    Program and Command Structure
        .vm extension
    7.2.2 Arithmetic and Logical Commands
        The VM represents true and false as -1 and 0
        
        Command Return Value    Comment   
        -----------------------------------------
        add     x + y           Integer addition    (2's complement)
        sub     x - y           Integer subtraction (2's complement)
        neg     -y              Arithmetic negation (2's complement)
        eq      true if x = y   Equality
        gt      true if x > y   Greater than
        lt      true if x < y   Less than
        and     x And y         Bit-wise
        or      x Or y          Bit-wise
        not     Not y           Bit-wise

    7.2.3 Memory Access Commands
        All the memory access commands are accessed by the same two commands
        * push segment index    Push the value of segment[index] onto the stack.
        * pop segment index     Pop the top stack value and store it in segment[index]
    
        Segment     Purpose                         Comments
        -----------------------------------------------------------------------------
        argument    Stores the function's           Allocated dynamically by the VM
                    arguments.                      implementation when the function
                                                    is entered.
        -----------------------------------------------------------------------------
        local       Stores the function's           Allocated dynamically by the VM
                    variables.                      implementation and initialized to
                                                    0's when the function is entered.
        -----------------------------------------------------------------------------
        static      Stores static variables         Allocated by the VM imp.
                    shared by all functions         for each .vm file; shared by all
                    in the same .vm file.           functions in the .vm file
        -----------------------------------------------------------------------------
        constant    Pseudo-segment that holds       Emulated by the VM
                    all the constants in the        implementation; Seen by all the
                    range 0...32767.                functions in the program.
        -----------------------------------------------------------------------------
        this        General-purpose segments.       Any VM function can use these
        that        Can be made to correspond       segments to manipulate selected
                    to different areas in the       areas on the heap
                    heap. Serve various
                    programming needs.
        -----------------------------------------------------------------------------
        pointer     A tow-entry segment that        Any VM function can set pointer
                    holds the base addresses of     0 or 1 to some address; this has
                    the this and that segments.     the effect of aligning the this
                                                    or that segment to the heap area
                                                    begining in that address.
        -----------------------------------------------------------------------------
        temp        Fixed eight-entry segment       May be used by any VM function
                    that holds temporary            for any purpose. Shared by all
                    variables for general use.      functions in the program
        
        -----------------------------------------------------------------------------
                Figure 7.6 The memory segments seen by every VM function.



        The relationship among VM files, VM functions, and their respective virtual
        memory segments is depicted in figure 7.7
        
        ----------------------------------------------------------------
        Foo.vm                              Bar.vm
        f1          f2          f3          f1          f2

        ----------------------------------------------------------------
                                    VM
                                translator

        ----------------------------------------------------------------
                static                      static
        argument    argument    argument    argument    argument
        local       local       local       local       local
        this        this        this        this        this
        that        that        that        that        that
        pointer     pointer     pointer     pointer     pointer

                                temp

                                constant
        ----------------------------------------------------------------
                                    VM
                                 translator
        ----------------------------------------------------------------
                        Hack machine language code                               
        ----------------------------------------------------------------
                                Figure 7.7
          

        The Stack
            Consider the commands sequence push argument 2 and pop local 1,
            mentioned befor. The working memory of such VM operations is in the stack.

        The Heap
            The heap is the name of the RAm area dedicated  for storing objects and arrrays data.

    7.2.4 Program Flow and function Calling Commands
    Program Flow Commands
        label symbol        // Label decalration
        goto symbol         // Unconditional branching
        if-goto symbol      // Conditional branching

    Function Calling Commands
        function functionName nLocals   // Function decalration
        call functionName nArgs         // Function invocation
        return                          // Transfer contrl back to the calling function


    7.2.5 Program Elements in the Jack-VM-Hack Platform
        prog directory
        --------------------------------------------
        Foo.jack                    Bar.jack
        m1      m2      m3          m1      m2
        --------------------------------------------

                        compiler
        prog directory
        --------------------------------------------
        Foo.vm                      Bar.vm
        Foo.m1  Foo.m2  Foo.m3      Bar.m1  Bar.m2
        --------------------------------------------

                        VM translator
        Prog.asm
        --------------------------------------------
                    Hack assembly code
        --------------------------------------------

                        assembler
        Prog.hack
        --------------------------------------------
                    Hack binary code
        --------------------------------------------
                Figure 7.8 Program elements

    7.2.6 VM Programming Examples
    Before You Read This Section

    A Typical Arithmetic Task
    
        High-level code (C style)
        int mult(int x, int y) {
            int sum;
            sum = 0;
            for (int j = y; j != 0; j--)
               sum += x;
            return sum;
        }

        First approximation     Pseudo VM code          Final VM code
        function mult           function mult(x, y)     function mult 2         // 2 local variables
            args x, y               push 0                  push constant 0
            vars sum, j             pop sum                 pop local 0         // sum=0
            sum = 0                 push y                  push argument 1
            j = y                   pop j                   pop local 1         // j=y
        loop:                   label loop              label loop
            if j = 0 goto end       push 0                  push constant 0
            sum = sum + x           push j                  push local 1
            j = j - 1               eq                      eq
            goto loop               if-goto end             if-goto end         // if j=0 goto end
        end:                        push sum                push local 0
            reutrn sum              push x                  push argument 0
                                    add                     add                 // sum=sum+x
                                    pop sum                 pop local 0
                                    push j                  push local 1
                                    push 1                  push constant 1
                                    sub                     sub                 // j=j-1
                                    pop j                   pop local 1
                                    goto loop               goto loop
                                label end               label end
                                    push sum                push local 0
                                    return                  return              // return sum

                            Figure 7.9 VM proramming example

    Array Handling
        VM code
        // Assume that the bar arry is the first local variable declared in the
        // high-level program. The following VM code implements the operation
        // bar[2]=19, i.e., *(bar+2)=19.
        push local 0        // Get bar's base address
        push constant 2
        add
        pop pointer 1       // Set that's base to (bar+2)
        push constant 19  
        pop that 0          // *(bar+2)=19
        ...

    Object Handling
        VM code
        // Assume that the b object and the r integer were passed to the function as
        // its first two arguments. The following code implements the operation
        // b.radius=r.
        push argument 0     // Get b's  base address
        pop pointer 0       // Point the this segment to b
        push argument 1     // Get r's value
        pop this 2          // Set b's third field to r
        ...
        
    7.3 Implementation
    7.3.1 Standard VM Mapping on the Hack Platform, Part 1
    VM to Hack Translation
        The VM translator takes a collection of .vm files as input and produces a
    single Hack assembly language .asm file as output.
    
    RAM Usage
        RAM address     Usage
        0-15            Sixteen virtual registers, usage described below
        16-255          Static variables (of all the VM functions in the VM program)
        256-2046        Stack
        2048-16383      Heap (used to store objects and arrays)
        16384-24575     Memory mapped I/O
        24575-32767     Unused memory space

        Register    Name    Usage
        RAM[0]      SP      Stack pointer: pointers to the next topmost location in the stack
        RAM[1]      LCL     Points to the base of the current VM function's local segment
        RAM[2]      ARG     Points to the base of the current VM function's argument segment
        RAM[3]      THIS    Points to the base of the current this segment(within the heap)
        RAM[4]      THAT    Points to the base of the current that segment(within the heap)
        RAM[5-12]           Hold the contents of the temp segment
        RAM[13-15]          Can be used by the VM implementtation as general purpose registers

    Memory Sements Mapping
        local, arguemnt, this, that: ith entry of any one of these segment should be translated
        to assembly code that accesses address (base + i) in the RAM, where base is the current
        value stored in the register decicated to the respective segment.

        pointer, temp: The access to pointer i should be translated to assembly code that accesses
        RAM location 3 + i, and access to temp i should be translated to assembly code that accesses
        RAM location 5 + i.

        constant: access to constant i by simply supplying the constant i.

        static: the asscembler allocates a new RAM address to it, starting at address 16. This convention
        can be exploited to represent each static variable number j in a VM file f  as the assembly
        language symbol f.j.

    Assembly Language Symbols

        Symbol                  Usage
        SP,LCL,ARG,THIS,THAT    These predifined symbols point, respectively, to the
                                stack top and to the base address of the virtual
                                segments local, argument, this, and that.
        R13-R15                 These predifined symbols can be used for any purpose.
        Xxx.j symobls           Each static variable j in file Xxx.vm is translated into
                                the assembly symbol Xxx.j. In the subsequent assembly process,
                                these symbolic variables will be allocated RAM space by
                                the Hack assembler.
        Flow of control symbols The implementation of the VM commands
                                function, call, and label involves generating
                                special label symbols, to be discussed in chapter 8.

    7.3.2 Design Suggestion for the VM Implementation
    
    prompt> VMTranslator source
        Where source is either a file name of the form Xxx.vm or a directory name

    7.3.3 Program Structure
    The Parser Module
    The CodeWriter Module
    Main Program

    7.4 Perspective

    7.5 Project
    Proposed Implementation Stages
        Stages 1: Stack Arithmetic Commands
        Stages 2: Memory Access Commands 

    Test Programs
        Stages 1: Stack Arithmetic
        * SimpleAdd
        * StackTest
        Stages 2: Memory Access
        * BasicTest
        * PointerTest
        * StaticTest

    Tips
        1. Run VM emulator to get acquainted with the intended program's behavior.
        2. translator .vm file to .asm program.
        3. Inspect the translated .asm program.
        4. Run CPU emulator
*/
