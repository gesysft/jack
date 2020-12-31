/*
    1   Boolean Logic

    1.1 Background
    1.1.1 Boolean Algebra
        
    Truth Table Representation

        x   y   z   f(x,y,z)
        0   0   0   0
        0   0   1   0
        0   1   0   1
        0   1   1   0
        1   0   0   1
        1   1   0   1
        1   1   1   0

    Boolean Expression
                         _
        f(x,y,z) = (x+y).z
       
    Canonical Representation
        every Boolean function can be expressed
        using at least one Boolean expression called the canonical representation.
                   _ _    __     _
        f(x,y,z) = xyz + xyz + xyz

    1.1.4 Hardware Description Language(HDL)

    Example: Building a Xor Gate

        HDL progarm(Xor.hdl)
        CHIP Xor {
            IN a, b;
            OUT out;
            PARTS:
            Not(in=a, out=nota);
            Not(in=b, out=notb);
            And(a=a, b=notb, out=w1);
            And(a=nota, b=b, out=w2);
            Or(a=w1, b=w2, out=out);
        }

    Explanation
        An HDL definition of chip consists of a header section and a parts section.
        The header section specifies the chip interface, namely the chip name and the 
        names of its input and output pins.
        The parts section describes the names and topology of all the lower-level parts(other chips)
        from which this chip is constructed. Each part is represented by a statement that specifies the
        part name and the way it is connected to other parts in the design.
        Inter-part connections are describe by creating and connecting internal pins, as needes. For 
        example, Not(..., out=nota) creates an internal pin(outbound wire) named nota, feeding out into it.
        And(a=nota,...) feeds the value of nota into the a input of an And gate.

    1.2 Specification
    
    1.2.1 The Nand Gate

    Boolean function:

        a   b   Nand(a,b)
        0   0       1
        0   1       1
        1   0       1
        1   1       0

    chip API boxes

        Chip name:  Nand
        Inputs:     a, b    
        Outputs:    out
        Function:   If a=b=1 then out=0 else out=1.
        Comments:   This gate is considered primitive and thus there is
                    no need to implement it.

    1.2.2 Basic Logic Gates
        every one of them can be composed from Nand gates alone.

    Not
        Chip name:  Not
        Inputs:     in
        Outputs:    out
        Function:   if in=0 then out=1 else out=0

    And
        Chip name:  And
        Inputs:     a, b
        Outputs:    out
        Functoin:   if a=b=1 then out=1 else out=0

    Or
        Chip name:  Or
        Inputs:     a, b
        Outputs:    out
        Function:   if a=b=0 then out=0 else out=1

    Xor
        known as "exclusive or"
        Chip name:  Xor
        Inputs:     a, b
        Outputs:    out
        Function:   if a!=b then out=1 else out-0
        
    Multiplexor
        a better name for this device might have been selector

        a   b   sel     out
        0   0   0       0
        0   1   0       0
        1   0   0       1
        1   1   0       1
        0   0   1       0
        0   1   1       0
        1   0   1       0
        1   1   1       1

        sel     out
        0       a
        0       b

        Chip name:  Mux
        Inputs:     a, b, sel   
        Outputs:    out
        Function:   if sel=0 then out=a else out=b

    Demultiplexor       
        A demultiplexor performs the opposite function of a multiplexor: It takes
        a single input and channels it to one of two possible outputs according to
        a selector bit that specifies which output to chose.
        
        sel     a   b
        0       in  0
        1       0   in

        Chip name:  DMux
        Inputs:     in, sel
        Outputs:    a, b
        Function:   If sel=0 then {a=in, b=0} else {a=0, b=in}.

    1.2.3 Multi-Bit Versions of Basic Gates

    Multi-Bit Not
        Chip name:  Not16
        Intputs:    in[16] // a 16-bit pin
        Outputs:    out[16]
        Function:   For i=0..15 out[i]=Not(in[i])

    Multi-Bit And
        Chip name:  And16
        Intputs:    a[16], b[16]
        Outputs:    out[16]
        Function:   For i=0..15 out[i]=And(a[i], b[i])

    Multi-Bit Or
        Chip name:  Or16
        Inputs:     a[16], b[16]
        Outputs:    out[16]
        Function:   For i=0..15 out[i]=Or(a[i], b[i])

    Multi-Bit Multiplexor
        Chip name:  Mul16
        Inputs:     a[16], b[16], sel
        Outputs:    out
        Function:   if sel=0 then for i=0..15 out[i] = a[i]
                    else          for i=0..15 out[i] = b[i]

    1.2.4 Multi-Way Versoins of Basic Gates
    
    Multi-Way Or
        Chip name:  Or8Way
        Inputs:     in[8]
        Outputs:    out[8]
        Function:   out=Or(in[0], in[1], in[2], in[3], in[4], in[5], in[6], in[7])

    Multi-Way/Multi-Bit Multiplexor

        sel[1]  sel[0]  out
        0       0       a
        0       1       b
        1       0       c
        1       1       d

        Chip name:  Mux4Way16
        Inputs:     a[16], b[16], c[16], d[16]
        Outputs:    out[16]
        Function:   if      sel=00 then out=a
                    else if sel=01 then out=b
                    else if sel=10 then out=c
                    else if sel=11 then out=d
        
        Chip name:  Mux8Way16
        Inputs:     a[16], b[16], c[16], d[16], e[16], f[16], g[16], h[16]
        Outputs:    out[16]
        Function    if      sel = 000 then out=a
                    else if sel = 001 then out=b
                    ...

    Multi-Way/Multi-Bit Demultiplexor

        sel[1]  sel[0]  a   b   c   d
        0       0       in  0   0   0
        0       1       0   in  0   0
        1       0       0   0   in  0
        1       1       0   0   0   in
        
        Chip name:  DMux4Way
        Inputs:     in, sel[2]
        Outputs:    a, b, c, d
        Function:   if      sel=00 then {a=in, b=c=d=0}
                    else if sel=01 then {b=in, a=c=d=0}
                    else if sel=10 then {c=in, a=b=d=0}
                    else if sel=11 then {d=in, a=b=c=0}

        Chip name:  DMux8Way
        Inputs:     in, sel[3]
        Outputs:    a, b, c, d, e, f, g, h
        Functions:  if      sel=000 then {a=in, b=c=d=e=f=g=h=0}
                    else if sel=001 then {b=in, a=c=d=e=f=g=h=0}
                    ...
    

    1.3 Implementation
        each gate can be implemented in more then one way, the simpler the implementation, the better
    
    1.4 Perspective
       
    1.5 Projects
    Tips
        built-in toools/builtIn/Nand.hdl
        If our simulator fails to find a Mux.hdl in the current directory, it automatically invokes a
        built-in Mux implementation, pre-supplied with the simulator's software. This built-in
        implementtation - a java class stored in the builtIn directory - has the same interface and 
        functionality as those of the Mux gate described in the book.

    Steps
        0. hardware simulator
        1. appendix A, A1-A6
        2. hardware simulator tutorial
        3. projects/01 directory
*/
