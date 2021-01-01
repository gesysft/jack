/*
    3   Sequential Logic

    3.1 Background

    The Clock

        The exact hardware implmentation is usually based on an oscilator that
    alternates continuously between two phases labeled 0-1, low-high, tick-tock, etc.

    Filp-Flops

        In this book we use a variant called a data flip-flop, or DFF.
        out(t) = in(t-1)

    Registers

        if load(t-1) then out(t) = in(t-1)
                     else out(t) = out(t-1)
        The basic design parameter of such a register is its width - the number of bits
    that is holds, 16, 32, or 64. The multi-bit contents of such registers are typically referred to as words.

    Memories

        we can proceed to build memory banks of arbitrary length,
    this can be done by stacking together many registers to form a Random Access Memory RAM unit.

    Counters
        Typically, the counter will have to be equipped with some additional functionality, such as
    possibilities for resetting the count to zero, loading a new counting base, or decrementing instead
    of incrementing 

    Time Matters
        This way, we are guaranteed that by the time the sequential chip updates its state
    (at the beginning of the next clock cycle), the inputs that it receives from the ALU
    will be valid.

    3.3 Specification

    3.2.1 Data-Flip-Flop

        Chip name:  DFF
        Inputs:     in
        Outputs:    out
        Function:   out(t) = in(t-1)
        Comment:    This clocked gate has a built-in implementation and thus
                    there is no need to implement it.
        Hardware implementtations achieve this time dependency by simultaneously 
    feeding the master clock signal to all the DFF gates in the platform. Hardware simulators
    emulate the same effect in software.

    3.2.2 Registes
    
        Chip name:  Bit
        Inputs:     in, load
        Outputs:    out
        Function:   if load(t-1) then out(t)=in(t-1) else out(t)=out(t-1)

        Chip name:  Register
        Inputs:     in[16], load
        Outputs:    out[16]
        Function:   if load(t-1) then out(t)=in(t-1) else out(t)=out(t-1)
        comments:   "=" is a 16-bit operation
    
    3.2.3 Memory
        
        Chip name:  RANn    // n and k are listed below
        Inputs:     in[16], address[k], load
        Outputs:    out[16]
        Function    out(t)=RAM[address[t]](t)
                    if load(t-1) then RAM[address(t-1)](t) = in(t-1)
        
        The specific RAM chips needed for the Hack platform are:
        Chip name       n       k
        RAM8            8       3
        RAM64           64      6
        RAM512          512     9
        RAM4K           4096    12
        RAM16K          16384   14
            
        Read:   To read the contents of register number m, we put m in the address input.
        The RAM's direct-access logic will select register number m, which will then emit
        its output value to the RAM's output pin. This is a combinational operation, independent of the clock.
        
        Write:  To write a new data d into register number m, we put m in the address input,
        d in the in input, adn assert the load input bit. This causes the RAM's direct-access logic
        to select register number m, and the load bit to enable it. In the next clock cycle, the
        selected register will commit to the value(d), and the RAM's output will start emitting it.


    3.2.4 Counter
        Chip name:  PC
        Inputs:     in[16], inc, load, reset
        Outputs:    out[16]
        Function:   if reset(t-1) then out(t)=0
                    else if load(t-1) then out(t)=in(t-1)
                    else if inc(t-1) then out(t)=out(t-1)+1
                    else out(t)=out(t-1)
        
    3.3 Implementation

    n-Register Memory

        select a particular register from the RAM64 memory we use a 6-bit address, say xxxyyy.
    The MSB xxx bits select one of the RAM8 chips,
    The LSB yyy bits select one of the registers whithin the selectd RAM8.

    3.4 Perspective

    3.5 Project
    
    Steps
        0. hardware simulator
        1. Read appendix A, focusing on sections A.6 and A.7
        2. hardware simulator tutorial, focusing on parts 4 and 5
        3. projects/03
*/
