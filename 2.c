/*
    2   Boolean Arithmetic

    2.1 Background

    Binary Numbers
        (10011)two = 1 * 16 + 0 * 8 + 0 * 4 + 1 * 2 + 1 * 1 = 19

    Binary Addition
    
        0   0   0   1       (carry)     1   1   1   1
            1   0   0   1   x               1   0   1   1
        +   0   1   0   1   y           +   0   1   1   1
        -----------------               -----------------
        0   1   1   1   0   x+y         1   0   0   1   0
        no overflow                     overflow

    Signed Binary Numbers

        2's complement
        
            _       pow(2, n) - x   (if x != 0)
            x   =   
                    0               (if x == 0)

        For example, in a 5-bit binary system 2's complement representation of -2 is
        pow(2, 5) - (00010)two = (32)ten - (2)ten = (30)ten = (11110)two

        (00010)two + (11110)two = (00000)two
        in 5-bit binary system the left-most sixth bit is ignored
        As a rule, x + (-x) = pow(2, n)

            Positive    Negative
            0   0000
            1   0001    1111 -1
            2   0010    1110 -2
            2   0011    1101 -3
            4   0100    1100 -4
            5   0101    1011 -5
            6   0110    1010 -6
            7   0111    1001 -7
                        1000 -8

        Figure 2.1 2's complement representation of signed numbers in a 4-bit binary system
        
        * The maximal and minimal numbers is pow(2, n-1) - 1 and -pow(2, n-1)
        * The codes of all positive numbers  begin with a 0
        * The codes of all negative numbers  begin with a 1
        * To obtain the code of -x from the code x, which is easier to implement in hardware,
          is to flip all the bits of x and add 1 to the result

    2.2 Specification
    2.2.1 Adders
    
            a   b   carry   sum
            0   0   0       0
            0   1   0       1
            1   0   0       1
            1   1   1       0

        Chip name:  HalfAdder
        Inputs:     a, b
        Outputs:    sum, carry
        Function:   sum = LSB of a + b
                    carry = MSB of a + b

            a   b   c   carry   sum
            0   0   0   0       0
            0   0   1   0       1
            0   1   0   0       1
            0   1   1   1       0
            1   0   0   0       1
            1   0   1   1       0
            1   1   0   1       0
            1   1   1   1       1

        Chip Name:  FullAdder
        Inputs:     a, b, c
        Outputs:    sum, carry
        Function:   sum = LSB of a + b + c
                    carry = MSB of a + b + c



            ... 1   0   1   1   a
                0   0   1   0   b
            -----------------
            ... 1   1   0   1   out

        Chip name:  Add16
        Inputs:     a[16], b[16]
        Outputs:    out[16]
        Function:   out = a + b
        Comment:    Integer 2's complement addition.
                    overflow is neither detected nor handled.

        Chip name:  Inc16
        Inputs      in[16]
        Outputs:    out[16]
        Function:   out = in + 1       
        Comment:    Integer 2's complement addition.
                    overflow is neither detected nor handled.

    2.2.2 The Arthmetic Logic Unit(ALU)
    
        Chip name:  ALU
        Inputs:     x[16], y[16], 
                    zx,                 // zero the x input
                    nx,                 // negate the x input
                    zy,                 // zero the y input
                    ny,                 // negate the y input
                    f,                  // function code: 1 for Add, 0 for And
                    no                  // negate the out
        Outputs:    out[16],
                    zr,                 // true if out=0
                    ng                  // true if out<0
        Function:   if zx then x = 0
                    if nx then x = !x
                    if zy then y = 0
                    if ny then y = !y
                    if f then out = x + y
                         else out = x & y
                    if no then out = !out
                    if out=0 then zr = 1 else zr = 0
                    if out<0 then ng = 0 else ng = 0
        Comment:    overflow is neither detected nor handled.    

                Figure 2.5 The Arithmetic Logic Unit.


        zx      nx      zy      ny      f       no      out
        1       0       1       0       1       0       0
        1       1       1       1       1       1       1
        1       1       1       0       1       0       -1
        0       0       1       1       0       0       x
        1       1       0       0       0       0       y
        0       0       1       1       0       1       !x
        1       1       0       0       0       1       !y
        0       0       1       1       1       1       -x
        1       1       0       0       1       1       -y
        0       1       1       1       1       1       x+1
        1       1       0       1       1       1       y+1
        0       0       1       1       1       0       x-1
        1       1       0       0       1       0       y-1
        0       0       0       0       1       0       x+y
        0       1       0       0       1       1       x-y
        0       0       0       1       1       1       y-x
        0       0       0       0       0       0       x&y
        0       1       0       1       0       1       x|y

                Figure2.6 The ALU truth table
        
    2.3 Implementation
    
    2.4 Perspective

        For example, our ALU features neither multiplication nor division nor floating point arithmetic.
    We will implement some of these operations at the operating system level, described in chapter 12.
        Detailed treatments of Boolean arithmetic and ALU design can be found in most
    computer architecture textbooks.

    2.5 Project
        projects/02
*/
