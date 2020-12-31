/*
    8   Virtual Machine 2: Program Control

    8.1 Background
        * Parsing parameters from the caller to the called subroutine
        * Saving the state of the caller before switching to execute the called subroutine
        * Allocating space for the local variables of the called subroutine
        * Jumping to execute the called subroutine
        * Returing values from the called subroutine back to the caller
        * Recycling the memory space occupied by the caller subroutine, when it retuns
        * Reinstaing the state of the caller
        * Jumping to execute the code of the caller immediately following the spot where we left it

    8.1.1 Program Flow
        Flow of control structure       Pseudo VM code
        if (cond)                           VM code for computing ~(cond)
            s1                              if-goto L1
        else                                VM code for executing s1
            s2                              goto L2
        ...                             label L1
                                            VM code for executing s2
                                        label L2
                                            ...

        while (cond)                    label L1
            s1                              VM code for computing ~(cond)
        ...                                 if-goto L2
                                            V code for executing s1
                                            goto L1
                                        label L2
                                            ...

    8.1.2 Subroutine Calling    
        suggests a stack-based solution to implementing this returen logic.
    When we encounter a call xxx operation, we know exactly what the returen address should be:
    It's the address of the next command in the caller's code. Thus, we can push this returen address
    on the stack and preceed to execute the code of the called subroutine. When we latter encouter a
    return command, we can pop the saved returen address and simply goto it. In other words, the
    return address can also be placed in the caller's frame

    8.2 VM Specification, Part 2
    8.2.1 Program Flow Commands
        * label label   This command labels the current location in the function's code.
                        Only labeled locations can be jumped to from other parts of the program. The
                        scope of the label is the function in which it is defined.
        * goto label    This command effects an unconditional goto operation, causing execution
                        to continue from the location marked by the label. The jump destination must
                        be located in the same function
        * if-goto label This command effects a conditional goot operation. The stack's
                        topmost value is poped; if the value is not zero, execution continues from the
                        location marked by the label; otherwise, execution continues from the next command in
                        the program. The jump destination must be located in the same function.

    8.2.2 Function Calling Commands
        * function f nLocals
        * call f mArgs
        * return 

    8.2.3 The Function Calling Protocol

    8.2.4 Initialization
        When the VM implementation starts running, the convention is that it always executes
    an argument-less VM function called Sys.init. Typically, this function then calls the  main
    function in the user's program. Thus, compilers that generate VM code must ensure that each
    translated program will have one such Sys.init function.

    8.3 implementtation
    8.3.1 Standard VM Mapping on the Hack Platform, Part 2

    The Global Stack

                |frames of all the functions| 
                |    up the calling chain   |
                 ---------------------------
        ARG->   |        argument 0         |
                |        argument 1         |
                |        ...                |    
                |        argument n-1       |    
                |        return address     |    
                |        saved LCL          |
                |        saved ARG          |    
                |        saved THIS         |    
                |        saved THAT         |    
        LCL->   |        local 0            |    
                |        local 1            |    
                |        ...                |    
                |        local k-1          |        
        SP->    |                           |    
                |            |              |    
                |            v              |

        Figure 8.4 The global stack structure
        
    Function Calling Protocl Implementation

        VM command                  Generated(pseudo)code emitted by the VM implementation
        --------------------------------------------------------------------------------------------------
        call f n                    push returen-address    // Using the label decalred below
                                    push LCL                // Save LCL of the calling function
        (calling a function f       push ARG                // Save ARG of the calling function
        after n arguments           push THIS               // Save THIS of the calling function
        have been pushed            push THAT               // Save THAT of the calling function
        onto the stack)             ARG = SP-n-5            // Reposition ARG(n = number of args)
                                    LCL = SP                // Reposition LCL
                                    goto f                  // Transfer control
                                    (return-address)        // Decalre a label for the reutnr-address
        --------------------------------------------------------------------------------------------------
        function f k                (f)                     // Declare a label for the function entry
                                    repeat k times:         // k = number of local variables
        (declaring a function       push 0                  // Initialize all of them to 0
        f that has k local
        variables)
        --------------------------------------------------------------------------------------------------
        return                      FRAME = LCL             // FRAME is a temporary variable
                                    RET = *(FRAME-5)        // Put the returen-address in a temp var
        (from a function)           *ARG = pop()            // Reposition the returen value for the caller
                                    SP = ARG+1              // Restore SP of the caller
                                    THAT = *(FRAME-1)       // Restore THAT of the caller
                                    THIS = *(FRAME-2)       // Restore THIS of the caller
                                    ARG = *(FRAME-3)        // Restore ARG of the caller
                                    LCL = *(FRAME-4)        // Restore LCL of the caller
                                    goto RET                // Goto returen-address(in the caller's code)
        --------------------------------------------------------------------------------------------------
            Figure 8.5 VM implementation of the three function calling commands of the VM language

    Assembly Language Symbols
        Symbol                  Usage
        ---------------------------------------------------------------------------------------
        SP,LCL,ARG,THIS,THAT    These predefined symbols point, respectively, to the
                                stack top and to the base address of the virtual
                                segments local, argument, this and that.
        ---------------------------------------------------------------------------------------
        R13-R15                 These predefined symbols can be used for any purpose.
        ---------------------------------------------------------------------------------------
        Xxx.j                   Each static variable j in a VM file Xxx.vm is
                                translated into assembly symbol Xxx.j. In the
                                subsequent assembly process, these symbolicc variables
                                will be allocated RAM space by the Hack assembler.
        ---------------------------------------------------------------------------------------
        functionName$label      Each label b command in a VM function f should
                                cause the VM immplementation to generate a globally
                                unique symbol "f$b" where "f" is the function name
                                and "b" is the label symbol within the VM function's
                                code. When translating goto b and if-goto b  VM
                                commands into the target language, the full label
                                specification "f$b" must be used instead of "b".
        ---------------------------------------------------------------------------------------
        (functionName)          Each VM function f should cause the VM
                                implementation to generate a symbol "f" that refers
                                to its entry point in the instruction memory of the
                                target computer.
        ---------------------------------------------------------------------------------------
        return-addresss         Each VM function call should cause the VM
                                implementation to generate the insert into the
                                translated code stream a unique symbol that server as
                                a return address, namely the memory location(in the
                                target paltform's memory) of the command following
                                the function call.
        ---------------------------------------------------------------------------------------
                    Figure 8.6 All the special assembly symbols

    Bootstrap Code
        SP=256
        call Sys.init

    8.3.2 Example

    8.3.3 Design Suggestions for the VM Implementation
    The Parser Module
    The CodeWriter Module

    8.4 Perspective

    8.5 Projects
    Test Programs for Program Flow  Commands
        * BasicLoop
        * Fibonacci
    
    Test Programs for Function Calling Commands
        * SimpleFunction
        * NestedCall
        * FibonacciElement
        * StaticsTest
*/

