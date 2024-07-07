Project TEST presents

> # MPC Custom language, nicknamed Mach-C (Mindustry Machine Code)

# Mach-C structure documentation

This page covers the strtucture of the encoding method used within the Mach-C language (Chapter 6).

**(This documentation covers the `interpreted` code's structure and commands.)**

## Current interpreted code formatting (Chapter 6):

### `111111111 1 00 1 0 1111..(x20 total) 0000...(x20 total) ` or in decimal?

`11111111` : opCodes (1 to 255)

`1` : datatype of value1 (0 to 1)

`00` : datatype of value2 (0 to 3, even though we only need 0, 1, and 2)

`1` : negative / positive property of value1 (0 and 1)

`0` : negative / positive property of value2 (0 and 1) discord...

`1111..(x20 total)` : Register address for the target output and first value for operation (0 to 33,554,432)

`0000...(x20 total)` : Register address or regular value as the second value of operation (0 to 33,554,432)

## Further documentation to formatting

### 1. opCodes and their respecting sub-opCodes

**opCodes** or Operator codes are the **functions** or type of operation that a line of code is going to be in a mode of (fuckin re-write this sentence please).

```
(Math operations)
01 : Addition              [value1 + value2]
02 : Subtraction           [value1 - value2]
03 : Multiplication        [value1 * value2]
04 : Division              [value1 / value2]
05 : Int Division          [value1 // value2] (outputs interger only)
06 : Modulo                [value1 % value2]
07 : Power                 [value1 ^ value2]
08 : Exponent to the e     [value1 ^ e]
09 : Base-10 logarithm     [ log 10 (value 1) ]
10 : Natural logarithm     [ log e (value 1) ]
11 : Square root           [ sqrt(value1) ]
12 : Maximum of two values [ max(value1) ]
13 : Minimum of two values [ min(value1) ]
14 : Floor                 [ floor(value1) ]
15 : Ceiling               [ ceil(value1) ]
16 : Absolute value        [ abs(value1) ]
17 : Sine                  [ sin(value1) ]
18 : Cosine                [ cos(value1) ]
19 : Tangent               [ tan(value1) ]
20 : Arc-sine              [ asin(value1) ]
21 : Arc-cosine            [ acos(value1) ]
22 : Arc-tangent           [ atan(value1) ]
23 : Angle of 2 vectors    [ acos( (value1 x value2) / (abs(value1) x abs(value2)) ) ]
24 : Distance of 2 vectors [ abs( value1 - value2 )  ]
25 : length of vector      [ abs(sqrt( value1 ^ 2 + value2 ^ 2 )) ]
26 : 2D simplex noise      [ noise(value1, value2) ] (Function from mindustry)
27 : Random number         [ rand(0, value1) ] (doubtfully practical)
28 : Cosecant              [ 1 / sin(value1) ]
29 : Secant                [ 1 / cos(value1) ]
30 : Cotangent             [ 1 / tan(value1) ]
31 : Arc-cosecant          [ asin( 1 / value1 ) ]
32 : Arc-secant            [ acos( 1 / value1 ) ]
33 : Arc-cotangent         [ atan( 1 / value1 ) ]

(Data comparisons)
34 : Equal-To           [value1 == value2]
35 : Not-Equal          [value1 != value2]
36 : logical-And        [value1 & value2]
37 : Less-Than          [value1 < value2]
38 : Less-Or-Equal-Than [value1 <= value2]
39 : More-Than          [value1 > value2]
40 : More-Or-Equal-Than [value1 >= value2]
41 : Strict-Equal       [value1 === value2]
42 : Bitwise-OR         [value1 || value2]
43 : Bitwise-AND        [value1 && value2]
44 : Bitwise-XOR        [value1 ^^ value2]
45 : Bitwise-Flip       [ flip(value1) ]
46 : Bitshift-Left      [value1 << value2]
47 : Bitshift-Right     [value1 >> value2]

(Flow control)
48 : Jump
49 : While (loop)
50 : Call
51 : Return

(Data management)
52 : Save
53 : Load
54 : Move
54 : Set
55 : Load Subroutine
56 : Unload Subroutine
57 : Decode to V6Helper
58 : Decode to V7
```

```
- Exception Errors
    01 : unknown error, better check every part.
    02 : Kill task (killed a program by request)
    03 : Syntax error (find it, the code can't find it for you)
    04 : Subroutine is not detected or not preloaded
    05 : No exit on subroutine (No return block passed in subroutine)
    06 : Invalid datatype specified.
```

##

### 2. Datatype of values and their (Positive/negative) properties.

This section explains the datatype and data property variables.

-   Datatype of value1\
    `0` : Register address
    `1` : Memory address

-   Datatype of value2\
    `0` : Regular decimal value
    `1` : Register address
    `2` : Memory address

-   Data property of value1\
    `0` : value1 is positive
    `1` : value1 is negative

-   Data property of value2\
    `0` : value2 is positive
    `1` : value2 is negative

##

### 3. Value(s) being processed

Theese two values are the data that will be processed, the first value is an address to which the result of the operation will be stored in, simultaniously it will be used as the first value of the operation for example when using ADD, the instructiion can depicted as:

`value1 += value2`

the Value on Value2 when in regular decimal value datatype, can only be as high as `2^25` or `33,554,432`

Any number higher than this requires a register or memory address which is explained [Here](#registers-and-memory-addresses).

##

### Registers and Memory addresses

#### 1. Registers

Registers are small memory points that is placed in the CPU core. You might think of L1, L2, or L3 caches but **Caches and Registers are different!**

Caches are also small storages placed around the CPU. but in the context of this MPC, registers are variables in 1 single core, whilst caches are stored in small memory cells around it.

Our Registers uses a tech by @Hasganter called L100 (yes even the name is misleading, sorry lol). basically it unlocks the ability to store values within a processor like a memory cell but if optimized, can be denser than a memory cell (more cells per block).

> _"it is called L100 because it is actually slower than reading from a memory cell lmfao" - hasganter_

So basically it is just extra space filler, meh.

The register address in this context can access **both actual registers and caches** depending on the ID provided.

**Currently, we have not decided on the numbers to access registers yet.**

#### 2. Memory Addresses

Memory in this context is the **RAM**. with this ID, the CPU will send a request to the RAM and obtain the value within the memory address provided.

**Currently, we have not decided on the numbers to access the memory yet.**

##

### Subroutines

Subroutines are basically custom functions that supports the `DRY` programming rule, `Don't Repeat Yourself`.

Subroutines have the same structure as a regular scipt, but loaded as a subroutine. with the load subroutine function in Data control opCode.

##

### Exceptions

Exceptions will occur when certain conditions did not went as expected. the procedure of an exception will be stopping current running program completely by rejecting every code inputted until an end block.

-   Unknown error (00)
-   Task killed (01)
-   Syntax error (02)
-   Subroutine is not detected or not preloaded (03)
-   No return line in subroutine (04)
-   Invalid datatype (05)

##

## Data structure #TODO - CHANGE EXAMPLE FORMATTING

Every "file" will begin with `4 00 00 0000 0000 000` and end with `4 99 00 0000 0000 000` this is to make identification easier.

##

###### Many thanks to everyone involved. - Sincerely, PT team.
