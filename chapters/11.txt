/*
    11 Compiler 1: Code Generation

    11.1 Background
    11.1.1 Data Translation
    Symbol Table
        ---------------------------------
        Name        Tyep    Kind        #
        ---------------------------------
        nAccounts   int     static      0
        id          int     field       0
        name        String  field       1
        balance     int     field       2
        sum         int     argument    0
        status      boolean local       0
        nAccounts   int     static      0
        ---------------------------------
    Handling Variables
    
    Handling Arrays
        // arr[expression1] = expression2
        push arr
        VM code for computing and pushing the value of expression2
        add         // top stack value = RAM address of arr[expression1]
        VM code for computing and pushing the value of expression1
        pop temp 0  // temp 0 = the value of expression2
                    // top statck value = RAM address of arr[expression1]
        pop pointer 1
        push temp 0
        pop that 0
    
    Handling Objects
        // Java code                                                RAM
        Class Complex {                                     0       
            // Properties (fields):                                        ...
            int re;                                         326     6712    a (local 0)
            int im;                                         327     7002    b (local 1)
            public Complex(int aRe, int aIm) {              328     6712    c (lcaol 2)
                re = aRe;                                      
                im = aIm;                                   6712    5       a object
            }                                               6713    17      a object
            ...                                                     ...
        }                                                   7002    12      c object
            // The following code can be in any class       7003    192     c object
            public void bla() {
                Complex a, b, c;
                a = new Complex(5, 17);
                b = new Complex(12, 192);
                c = a;
            }                                            

    11.1.2 Commands Translation
    Evaluating Expressins
        Source code     Parse Tree          Target Code:
        x+g(2,y,-z)*5       +               push x
                          /   \             push 2
                        x       *           push y
                              /   \         push z
                            g       5       neg
                          / | \             call g
                        2   y   -           push 5
                                |           call multiply
                                z           add

    Translating Flow Control
        Source code     Generated code                      Generated code (version 2)
        if (cond)           VM code for computing ~(cond)       if-goto IF_TRUE
            s1              if-goto IF_FALSE                    goto IF_FALSE
        else                VM code for executing s1        label IF_TRUE
            s2              goto IF_END                         VM  code for executing s1
        ...             label IF_FALSE                          goto IF_END
                            VM code for executing s2        label IF_FALSE
                        label IF_END                            VM  code for executing s2
                            ...                             label IF_END
                                                                ...

        if (cond)           VM code for computing ~(cond)       if-goto IF_TRUE
            s1              if-goto IF_FALSE                    goto IF_FALSE
        ...                 VM code for executing s1        label IF_TRUE
                        IF_FALSE                                VM code for executing s1
                            ...                             label IF_FALSE
                                                                ...

        while (cond)    Label WHILE_EXP
            s1              VM code for executing ~(cond)
        ...                 if-goto WHILE_END
                            VM code for executing s1
                            goto WHILE_EXP
                        label WHILE_END
                            ...
    
    11.2 Specification
    11.2.1 Standard Mapping over the Virtal Machine

    Fiel and Function Naming
        * A Jack subroutine xxx() in a Jack class Yyy is compiled into a VM function 
        called Yyy.Xxx
        * A Jack function or constructor with k arguments is compiled into a VM function 
        that operates on k arguments.
        * A Jack method with k arguments is compiled into a VM function that  operates on
        k + 1 arguments. The first argument always refers to the this object.
    
    Memory Allocation and Access
        * The local variables to the virtual local segment
        * The argument variables to the virtual argument segment
        * The static variables to the virtual static segment
        * The field variables to the virtual this segment(using pointer 0)
        * array to the virtual that segment(using pointer 1)

    Subroutine Calling
        * When compiling a Jack method into a VM function, the compiler must insert
        VM code that sets the base of this segment properly. When compiling
        a Jack constructor, the compile must insert VM code that allocates a memory
        block for the new object and then sets the base of the this segment to point
        at its base.

        * Before calling, mush push the arguments onto the stack. If is Jack method,
        push pointer 0, and then push argument 0, push argument 1...
        * Void subroutine, push constant 0, and then return
        * do sub statement, pop temp 0

    Returning from Void Methods and Functions
        * void, return the constant 0
        * the caller must pop the returned value

    Constans
        * null and false are mapped to constant 0. True is mapped to
        constant -1(this constant can be obtained via push constant 1 folled by neg)

    Use of Operating System Services
        * + add, - sub, - neg, = eq, > gt, < lt, & and, | or, ~ not
        * Multiplication and division are handled using the OS functions
        Math.multiply() and Math.divide()
        * String using String.new(length), String.appendChar(nextChar)
        * Constructors allocate space for new object using the OS function Memory.alloc(size)
        



    11.3 Implementation
        * JackCompiler
        * JackTokenizer
        * SymbolTable
        * VMWriter
        * CompilationEngine

    11.4 Perspective

    11.5 Porject
    Test Method
        1. Copy all the supplied OS .vm files from tools/OS into the program directory.
        2. Compile the program directory using your compiler.
        3. fix your compiler and return to step 2
        4. the program directory should contain one .vm file as well as all the supplied OS .vm files.
        5. Execute the VM Emulator, loding the entire directory
        6. if the program behaves unexpectedly or some error message is displayed, fix your compiler

    Test Programs
        Seven
        ConvertToBin
        Square
        Average
        Pong
        ComplexArrays
*/
