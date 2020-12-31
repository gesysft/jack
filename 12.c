/*
    12  Operating System 
    12.1 Background
    12.1.1 Mathmatical Operations
    Multiplication
                        Long multiplication
        x               1   0   1   1   =   1   1
        y           *       1   0   1   =       5   j-th bit of y    
                        -------------       -----
                        1   0   1   1                   1
                    0   0   0   0                       0
                1   0   1   1                           1
                ---------------------
        x * y   1   1   0   1   1   1   =   5   5

        multiply(x, y):
        // Where x, y >= 0
        sum = 0
        shiftedX = x
        for j = 0...(n-1) do
            if (j-th bit of y) = 1 then
                sum = sum + shiftedX
            shiftedX = shiftedX * 2
   
    Division
        divide(x, y):
            // Integer part of x/y, where x >= 0 and y > 0
            if y > x return 0
            q = divice(x, 2 * y)
            if (x - 2 * q *  y) < y
                return 2 * q;
            else 
                return 2 * q + 1

    Sqrt Root
        sqrt(x):
            // Computer the integer part of y = sqrt(x). Strategy:
            // Find an integer y such that y * y <= x  < (y + 1) * (y + 1) (for 0 <= x < pow(2, n))
            // by performing a binary search in the range 0...pow(2, n/2) - 1.
            y = 0
            for j = n/2-1...0 do
                if pow(y + pow(2, j), 2) <= x
                    then y = y + pow(2, j)
            return y

    12.1.2 String Representation of Numbers
        // Convert a non-negative number to a string
        int2String(n):
            lastDigit = n % 10
            c = character representing lastDigit
            if (n < 10)
                return c (as a string)
            else
                return int2String(n/10).append(c)

        // Convert a string to a non-negative number
        string2Int(s):
            v = 0
            for i = 1...length of s do
                d = integer value of the digit s[i]
                v = v * 10 + d
            return v
            // (Assuming that s[1] is the most significant digit character of s.)

    12.1.3 Memory Management
    Dynamic Memory Allocation

    Basic Memory Allocation Algorithm
        Initialization: free = heapBase
        // Allocate a memory block of size words.
        alloc(size):
            pointer = free
            free = free + size
            return pointer

        // De-allocate the memory space of a given object.
        deAlloc(object):
            do nothing

    Improved Memory Allocation Algorithm 
        Initialization:
            freeList = heapBase
            freeList.length = heapLength
            freeList.next = null

        // Allocate a memory space of size words.
        alloc(size):
            Search freeList using best-fit or first-bit heuristics
                to obtain a segment with segment.length > size
            If no such segment is found, return failure
                (or attemet defragmentation)
            block = needed part of the found segment
                (or all of it, if the segment remainder is too small)
            Update freeList to reflect  the allocation
            block[-1] = size + 1 // Remember block size, for de-allocation
            Return block

        // Deallocate a decommissioned object.
        deAlloc(object):
            segment = object - 1
            segment.length = object[-1]
            Insert segment into the freeList
    
    12.1.4 Variable-length Arrays and Strings

    12.1.5 Input/Output Management
    Graphic Output
    Pixel Drawing
        drawPixel(x, y):
            // Hardware-specific.
            // Assuming a memory mapped screen:
            Write a predetermined  value in the RAM localtion corresponding to screen
            location(x, y).

    Line Drawing
        drawLine(x, y, x + dx, y + dy):
            // Assuming dx, dy > 0
            initialize (a, b) = (0, 0)
            while a <= dx  and b <= dy do
                drawPixel(x + a, y + b)
                if a/dx < b/dy then a++ else b++

            Figure 12.8a Line drawing

        // To test whether a/dx < b/dy, maintain a variable adyMinusbdx
        // and test if it becomes negative.
            Initialization:         set adyMinuxbdx = 0
            When a++ is performed:  set adyMinusbdx = adyMinuxbdx + dy
            When b++ is performed:  set adyMinusbdx = adyMinuxbdx - dx

            Figure 12.8b Efficient testing using addition operations only.
            
    Circle Drawing
        drawCircle(x, y, r):
            for each dy in (-r...r) do
                drawLine from (x - sqrt(pow(r, 2)), y + dy) 
                         to (x  + sqrt(pow(r, 2) - pow(pow(dy, 2))), y + dy)
    
    Character Output
        screen that is 256 rows by 512 columns
        drawing a single character (11 rows, 8 columns)
        then our screen can show 23 lines of 64 characters each (with 3 extra rows of pixels left unused).
 
    Keyboard Handling
    Detection Keyboard Input
        keyPressed():
            // Depends on the specifics of the keyboard interface
            if a key is presently pressed on the keyboard
                return the ASCII value of the key
            else 
                return 0

    Reading a Single Character 
        readChar():
            // Read and echo a single character
            display the cursor
            while no key is pressed on the keyboard
                do nothing // wait till a key is pressed
            c = code of currently pressed key
            while a key is pressed
                do nothing // wait for the user to let go
            print c at the current cursor location
            move the cursor one position to the right
            return c

    Reading a String
        readLine():
            // Read and echo a "line" (until newline)
            s = empty string
            repeat
                c = readChar()
                if c = newline character
                    print newline
                    return s
                else if c = backspace character
                    remove last character from s
                    move the cursor 1 position back
                else
                    s = s.append(c)

    12.2 The Jack OS Specification
        * Math
        * String
        * Array
        * Output
        * Screen
        * Keyboard
        * Memory
        * Sys
        see ./projects/09/Jack OS API.pdf
        
        12.2.1 Math
        * function void init(): for internal use only;
        * function int abs(int): returns the absolute value of x;
        * function int multiply(int x, int y): returns the product of x and y;
        * function itn divide(int x, int y): returns the interger part of x / y;
        * function itn min(int x, int y): returns the minimum of x and y;
        * function itn max(int x, int y): returns the maximum of x and y;
        * function int sqrt(int x): returns the integer part of the square root of x.
    
        12.2.2 String
        * constructor String new(int maxLength): constructs a new empty string(of length zero)
        that can contain at most maxLength characters;
        * method void dispose(): dispose this string;
        * method int length(): returns the length of this string;
        * method char charAt(int j): returns the character at location j of this string;
        * method void setCharAt(int j, char c): sets the j-th element of this string to c;
        * method String appendChar(char c): appends c to this string and returns this string;
        * method void eraseLastChar(): erases the last character form this string;
        * method int intValue(): returns the integer value of this string(or the string prefix
        until a non-digit character is detected);
        * method void setInt(int j): set this string to hold a representation of j;
        * function char backSpace(): returns the backspace characer;
        * function char doubleQuote(): returns the double quote("") characer;
        * function char backSpace(): returns the  newline characer;

        12.2.3 Array
        * function Array nwe(int size): constructs a new array of the given size;
        * method void dispose(): dispose this array.

        12.2.4 Output
        * function void init(): for internal use only;
        * function void moveCursor(int i, int j): moves the cursor to the j-th column of the
        i-th row, and erases the character displayed there;
        * function void printChar(char c): prints c at the cursor location and advances the
        cursor one column forward;
        * function void printString(String s): prints s starting at the cursor location and
        advances the cursor appropriately;
        * function printInt(int i): prints i starting at the cursor location and advances
        cursor appropriately;
        * function println(): advances the cursor to the begging of the next line;
        * function backSpace(): moves the cursor one column back;

        12.2.5 Screen
        * function void init(): for internal use only;
        * function void clearScreen(): erases the screen;
        * function void setColor(boolean b): sets a color (white = false, balck = true) to
        be used for all further drawXXX commands;
        * function void drawPixel(int x, int y): draws the (x, y) pixel;
        * function void drawLine(int x1, int y1, int x2, int y2): draws  a line from pixel
        (x1, y1) to pixel(x2, y2);
        * function void drawRectangle(int x1, int y1, int x2, int y2): draws a filled 
        rectangle whose top left cornel is (x1, y1) and whose bottom right corner is (x2, y2);
        * function void drawCircle(int x, int y, int r): draws a filled circle of radius 
        r <= 181 around (x, y).

        12.2.6 Keyboard
        * function void init(): for internal use only;
        * function char keyPressed(): return the character of the currently pressed key 
        on the keyboard; if no key is currently pressed, returns 0;
        * function char readChar(): waits until a key is pressed on the keyboard and released,
        then echoes the key to the screen and returns the character of the pressed key;
        * function String readLine(String message): prints the message on the screen, reads
        the line (text until a newline character is detected) from the keybord, echoes the line
        to the screen, and returns its value. This function alse handles user backspaces;  
        * function int readInt(String message): prints the message on the screen, reads the
        line (text until a newline character is detected) from the keyboard, echoes the line to
        the screen, and returns its integer value (until the fist non-digit character ini the line
        is detected). This function also handles user backspaces.

        12.2.7 Memory
        * function void init(): for internal use only;
        * function int peek(int address): returns the value of the main memory at this address
        * function void poke(int address, int value): sets the contents of the main memory 
        at this address to value;
        * function Array alloc(int size): finds and allocated from the heap a memory block
        of the specified size and returns a reference to its base address;
        * function void deAlloc(Array o): De-allocates the given object and frees its memory space.
        
        12.2.8 Sys
        * function void init(): calls the init functions of the other OS classes and
        then calls the Main.main() function. For internal use only;
        * function void halt(): hats the program execution;
        * function error(int errorCode): prints the error code on the screen and halts;
        * function void wait(int duration): waits approximately duration milliseconds and
        returns.


    12.3 Implementation
        12.3.1 Math
        12.3.2 String
        12.3.3 Array
        12.3.4 Output
        12.3.5 Screen
        12.3.6 Keyboard
        12.3.7 Memory

    12.3.8 Sys
 
        
    12.4 Perspective

    12.5 Project
    Test Strategy
        1. Put, in the same directory, the OS class Xxx.jack that you are developing, and
        the relevant supplied test program (a collection of one or more .jack files);
        2. COmpile the directory using the supplied Jack compiler.
        3. Load the directory's code (OS + test program) into the VM emulator.
        4. Execute and check

    OS Classed and Test Programs
        Memory, Array, Math
        String
        Output
        Screen
        Keyboad
        Sys
        Complete Test
            projects/11/Pong

*/
