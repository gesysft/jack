/*
    9   High-Level Language
    9.1 Background
    9.1.1 Example 1: Hello World   // Hello World program. 
        class Main {
            function void main() {
                do Output.printString("hello, world");
                do Output.println();
                return;
            }
        }

    9.1.2 Example 2: Procedural Programming and Array Handling
        // Computers the average of a sequence of integers.
        class Main {
            function void main() {
                var Array a;
                var int length;
                var int i, sum;
                
                let length = Keyboard.readInt("How many numbers? ");
                let a = Array.new(length);
                let i = 0;
                while (i < length) {
                    let a[i] = Keyboard.readInt("Enter the next number: ");
                    let sum = sum + a[i];
                    let i = i + 1;
                }

                do Output.printString("The average is: ");
                do Output.printInt(sum / length);
                do Output.println();
                return;
            }
        }
    
    9.1.3 Example 3: Abstract Data Types

    Defining a Class Interface
        In Jack, operations on the current object (referred to as this) are represented by
    methods, wheres class-level operations (equivalent to static methods in Java) are
    represented by functions. Operations that create new objects are called constructors.
        // A Fraction is an object representation of n/m where n and m are integers.
        field int  numerator, denominator       // Fraction object properties
        constructor Fraction new(int a, int b)  // Returns a new Fraction object
        method int getNumerato()                // Returns the numerator of this  fraction
        method int getDenominator()             // Returns the denominoator of this fraction
        method Fraction plus(Fraction other)    // Returns the sum of this fraction and another  fraction, as a fraction
        method void print()                     // Prints this fraction in the format "numerator/denominator"

    Using Classes
        // Computer the sum 2/3 and 1/5.
        class Main {
            function void main() {
                var Fraction a, b, c;
                let a = Fraction.new(2, 3);
                let b = Fraction.new(1, 5);
                let c = a.plus(b);
                do c.print();
                return;
            }
        }

    Implementing the Class
        // Provides the Fraction tye and related services.
        class Fraction {
            field int numerator, denominator;

            constructor Fraction new(int a, int b) {
                let numerator = a;
                let denominator = b;
                do reduce();
                return this;
            }

            method void reduce() {
                var int g;
                let g = Fraction.gcd(numerator, denominator);
                if (g > 1) {
                    let numerator = numerator / g;
                    let denominator = denominator / g;
                }
                return;
            }

            // Computers the greatest common denominator of a and b.
            function int gcd(int a, int b) {
                var int r;
                while (~(b = 0)) {              // Apply Euclid's algorithm
                    let r = a - (b * (a / b));  // r = remainder of a / b
                    let a = b;
                    let b = r;
                }
                return a;
            }

            method int getNumerator() { return numerator; }
            method int getDenominator() { return denominator; }

            method Fraction plus(Fraction other) {
                var int sum;
                let sum = (numerator * other.getDenominator()) + (other.getNumerator() * denominator);
                return Fraction.new(sum, denominator * other.getDenominator());
            }

            method void print() {
                do Output.printInt(numerator);
                do Output.printString("/");
                do Output.printInt(denominator);
                return;
            }
        }

    9.1.4 Example 4: Linked List Implementation
        // The List class provides a linked list abstraction
        class List {
            field int data;
            field List next;
        
            constructor List new(int car, List cdr) {
                let data = car;
                let next = cdr;
                return this;
            }

            method void dispose() {
                if (~(next = null)) {
                    do next.dispose();
                }

                do Memory.deAlloc(this);
                return;
            }
        }

        // creates a list holding the number (2,3,5)
        function void create235() {
            var List v;
            let v = List.new(5, null);
            let v = List.new(2, List.new(3, v));
            ... // Does something with the list
            do v.dispose();
            return;
        }

    9.2 The Jack Language Specification
    9.2.1 Syntatic Elements
                        
        ----------------------------------------------------------------------------------------------------
        White Space     Space characters, newline characters, and comments are ignored
        Space           
        and             
        comments        
        ----------------------------------------------------------------------------------------------------
        Symbols         ()
                        []
                        {}
                        ,
                        ;
                        =
                        .
                        + - * / & | ~ < >
        ----------------------------------------------------------------------------------------------------
        Reserved        class constructor, method, function     Program components
        words           int, boolean, char, void                Primitive types
                        var, static, field                      Variable declarations
                        let, do, if, else, while, return        Statements
                        then, false, null                       Constant values
                        this                                    Object reference
        ----------------------------------------------------------------------------------------------------
        Constants       Integer constants must be positive
                        String constants are enclosed within two quote(")
                        Boolean constants can be true of false
                        The constant null signifies a null reference
        ----------------------------------------------------------------------------------------------------
        Identifiers     letters(A-z, a-z), digits(0-9), and "_". The first character must be a letter of "_"
        ----------------------------------------------------------------------------------------------------
        ---------------------------------------------------------------------------------------------------
    
    9.2.2 Program Structure

        class name {
            Field and static variable declarations
            Subroutine declarations
        }

        subroutine type name (parameter-list) {
            local variable declarations
            statements
        }
        subroutine is either constructor, method or function.
            
    9.2.3 Variables

    Data Types
        Primitive Types     Jack features three primitive data type
        * int
        * boolean
        * char

        Object Types        Every class defines an object type
            Arrays
            Strings
    Type Conversions
        * Characters and integers are auto matically converted into each other as needed
            var char c;
            let c = 33;     // 'A'
            let s = "A";
            // Equivalently:
            let c = s.charAt(0);

        * An integer can be assigned to a reference variable(of any object type), in which case it is treated
        as an address in memory. Example
            var Array a;
            let a = 5000;
            let a[100] = 77; // memory addresss 5100 is set to 77
        
        * An object variable(whose type is a class name) may be converted into an Array variable,
        and vice versa. The conversion allows accessing the object fields as array entries, and
        vice versa. Example:
            // Assume that class Complex has two int fields: re and im.
            var Complex c;
            var Array a;
            let a = Array.new(2);
            let a[0] = 7;
            let a[1] = 8;
            let c = a; // c = Complex(7, 8)

    Variable Kinds and Scope
        -----------------------------------------------------------------------------------------------------------
        Variable kind       Definition/Description                          Decalred in         Scope
        -----------------------------------------------------------------------------------------------------------
        Static variables    static type name1, name2, ...;                  Class               The class in
                                                                            declaration.        Which they are
                            Only one copy of each static variable                               declared.
                            exists, and this copy is shared by all the
                            object instantces of the class(like private
                            static variables in Java)
        -----------------------------------------------------------------------------------------------------------
        Field variables     field type name1, name2, ...;                   Class               The class in
                                                                            declaration.        which the are
                            Every object instance of the class has a                            declared, except
                            private copy of the field variables(like                            for functions
                            private object variables in Java)
        -----------------------------------------------------------------------------------------------------------
        Local variables     var type name1, name2, ...;                     Subroutine          The subroutine
                                                                            declaration.        in which they
                            Local variables are allocated on the stack                          are declared.
                            when the subroutine is called and freed
                            when it returns(like local variables in java)
        -----------------------------------------------------------------------------------------------------------
        Parameter           type name1, name2, ...                          Appear in           The subroutine
        variables                                                           parameter lists     in which they
                            Used to specify inputs of subroutines, for      as part of          are decalred.
                            example:                                        subroutine
                                                                            declarations                                                                            declarations
                            function void drive(Car c, int miles)
        -----------------------------------------------------------------------------------------------------------

    9.2.4 Statements
        ---------------------------------------------------------------------------------------
        Statement   Syntax                          Description
        ---------------------------------------------------------------------------------------
        let         let variable = expression;      An assignment operation (where
                    or                              variable is either single-valued or
                    let variable[expression] =      an array). The variable kind may be
                        expression;                 static, local, field, or parameter.

        ---------------------------------------------------------------------------------------
        if          if (expression) {               Typical if statement which an
                        statements                  optional else cause.
                    }                               The curly brackets are mandatory
                    else {                          even if statements is a single
                        statements                  statement
                    }
        ---------------------------------------------------------------------------------------
        while       while (expression) {            Typical while statement
                        statements                  The curly brackets are mandatory
                    }                               even if statements is a single
                                                    statement
        ---------------------------------------------------------------------------------------
        do          do function-or-menthod-call;    Used to call a function or a
                                                    method for its effect, ignoring the
                                                    returned value
        ---------------------------------------------------------------------------------------
        return      return expression;              Used to return a value from a
                    or                              subroutine. The second  form must
                    return;                         be used by functions and methods
                                                    that return a void value.
                                                    Constructors must return the
                                                    expression this.
        ---------------------------------------------------------------------------------------

    9.2.5 Expressions
        A Jack expression is one of the following:
        * A constant;
        * A variable name in scope(static, field, local, or parameter);
        * The this keyword, denoting the current object(cannot be used in functions);
        * An array element using the syntax name[expression], where name is a variable
        name of tyep Array in scope;
        * A subroutine call the returns a non-void type;
        * An expresion prefixed by one of the unary operations - or ~:
        * An expression of the form expression operation expression where oprator is 
        one of the following binary operations:
            + - * /
            & |
            < > =
        * (expression) An expression in parentheses.

    9.2.6 Subroutine Calls
        Such a function can be invoked useing calls like Math.sqrt(17)

    Object Construction and Disposal
        let varName = className.constructorName(argument-list);
        
    9.2.7 The Jack Standard Libray
        * Math
        * String
        * Array
        * Output
        * Screen
        * Keyboard
        * Memory
        * Sys
        see ./projects/09/Jack OS API.pdf

    9.3 Writing Jack Applications
    Examples
    Application Design and Implementation
    The Jack OS

    9.4 Perspective
    
    9.5 Project
    Compiling and Running a Jack Program
    0. Each program must be stored in a sepqrate directory, say Xxx. Start by creating
    this directory, then copy all the files from tools/OS into it.
    1. Write your jack program-a set of one or more Jack classes-each stored in a 
    seperate ClassName.jack text file. Put all these .jack files in the Xxx directory.
    2. Compile your program using the supplied Jack compiler.
    3. At this point the program directory should contain three sets of files: 
        (1) your source .jack files
        (2) .vm files, one for each of your .jack class files
        (3) additional .vm files, comprising the supplied Jack OS.
    To test the compiled program, invoke the VM emulator and load the entire Xxx program director.

        
*/

