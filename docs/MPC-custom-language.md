# MPC custom language documentation (Story section lmfao)
The programming language for MPC does seem to be a huge roadblock but we have some concepts for now, code it later.

What we are thinking is to have a programming language similar to the level of assembly where its highly oversimplified and few syntax available. This language will be interpreted as base-10 numbers that can be saved in a mindustry memory cell which opens for data transfer between processors.


#
## 1. First impression :
We designed the first interpreted code: `10 100 65536 65536 65536 65536` (spaces is just for readability the real interpreted code does not have them)

**The first digits** `10` indicates the general type of operation or **opCodes**, such as **Math operation, Data movement, etc.** which will group several commands together for easier reference. (this means we have a max of 89 opCodes). 
Another thing that is we have to start with a number higher than 0 because we cant have 0's in the front of a value in mindustry.

**The second digits** `100` indicates the **sub-opCodes**. it is a supporting code that indicates a specific operation within the type of operation, such as in the general type of **math operation**, the sub-opCodes will have **addition, subtraction, division, multiplication, etc.** Which can be directly processed and/or handled in mindustry logic.

**The third digits and so on..** `65536` indicates the value to be processed, this specific value is actually **2<sup>16</sup>** where we just randomly decided it to be the max number of a value.

After some testing we found out the mindustry memory cell has some limitations, where if you input a number so high it would **round up the numbers**. based on some testing, we have found the limit before rounding happens, that is:

### 2<sup>53</sup> or `9 007 199 254 740 992`

With this, we can redesign our format to fit the limitations.


#
## 2. Improvements! :
With the newfound info, we can conclude that our interpreted code has to fit within 15 digits (even though th elimit is 16 digits, just to be safe), and redesigned the interpreted code to be:

### ```10 100 99999 99999``` (15 digits, no spaces in the actual interpreted code)

**First digits** `10` same as before, **opCodes**. indicates the general type of operation such as **Math operation, Data transfer, Flow control, etc.**

**Second digits** `100` same as before, **sub-opCodes**. indicates the specific type of operation such as in the general type of **math operation**, the sub-opCodes will have **addition, subtraction, division, multiplication, etc.**

**Third and fourth digits** `999999` Theese numbers are the values to be processed. we got rid of the 16-bit limit because why do we even have it, and went with 5 * 2 values to be processed in a single command.

With all that done, all that's left is to assign opCodes and sub-opCodes and **code it all!**


#
## 3. Revision #1
After some thinking, we thought of values higher than the `99999` limit, because admittedly it was small. So we planned of using registers and memory address to make up for it. with the plan above we eventually end up with this formatting:

```10 12 1 99999 99999```

**First digits** `10` same as before, **opCodes**. indicates the general type of operation such as **Math operation, Data transfer, Flow control, etc.**

**Second digits** `12` same as before but one digit less, **sub-opCodes**. indicates the specific type of operation such as in the general type of **math operation**, the sub-opCodes will have **addition, subtraction, division, multiplication, etc.**

**Third digit** `1` This one digit value represents the **data types of the values within the line**.
> - 1 = value1 is a regular number, value2 is also a regular number
> - 2 = value1 is a regular number, value2 is a register ID
> - 3 = value1 is a regular number, value2 is a memory ID
> - 4 = value1 is a register ID, value2 is a normal number
> - 5 = value1 is a register ID, value2 is also a register ID
> - 6 = value1 is a register ID, value2 is a memory ID
> - 7 = value1 is a memory ID, value2 is a normal number
> - 8 = value1 is a memory ID, value2 is a register ID
> - 9 = value1 is a memory ID, value2 is also a memory ID
> - 0 = value1is a disk ID, value2 is a memory ID

**Fourth and fifth digits** `999999` same as before, theese numbers are the values to be processed.


#
## 4. Revision #2
Totally forgot about two things, that is the positive or negative state of a value, and numbers behind commas. If the value was in a memory cell, it would have no problem differentiating between negative nor positive. But within one line of code, we currently cannot differentiate between negative and positive numbers and don't get us started with commas.

With the problem in hand, we decided to let go of the commas, because there is no efficient way to make that work. And to solve the positive/negative number problem we *(again)* redesigned the interpreted code format to be:

### ```1 12 3 4 99999 99999```

**First digits** `1` same as before but one digit less, **opCodes**. indicates the general type of operation such as **Math operation, Data transfer, Flow control, etc.**

**Second digits** `12` same as before, **sub-opCodes**. indicates the specific type of operation such as in the general type of **math operation**, the sub-opCodes will have **addition, subtraction, division, multiplication, etc.**

**Third digit** `3` same as before, represents data types of the values on hand.

**Fourth digit** `4` This one digit value represents the positive/negative status of the values on hand.
> - 1 = value1 positive, value2 positive
> - 2 = value1 negative, value2 positive
> - 3 = value1 positive, value2 negative
> - 4 = value1 negative, value2 negative
> - any numbers above will return **1**.

**Fifth and sixth digits** `999999` same as before, theese numbers are the values to be processed.
#
#
#
# Actual documentation (Skipping story)
Current latest interpreted code formatting:
### ```1 12 3 4 99999 99999```

`1` : opCodes (Operation Codes). Refrence numbers for the general type of operation.

`12` : sub-opCodes (sub Operation Codes). Reference numbers for the specific type of operation within the opCode.

`3` : data type of values being processed.

`4` : positive/negative status of values being processed.

`99999` : Value that will be processed.

### The processed value will be stored within the address of value1 or if none, Defaults and/or overwrites on register #0

## Explanation for the interpreted code formatting

### 1. opCodes and their respecting sub-opCodes
**opCodes** or Operator codes are a **generalized group of functions** or type of operation.

**sub-opCodes** are the **specific functions** or type of operation within a group of generalized functions (opCodes).
```
(opCode) 1 : Mathematical operations
(sub-opCode) 01 : Addition              [value1 + value2]
             02 : Subtraction           [value1 - value2]
             03 : Multiplication        [value1 * value2]
             04 : Division              [value1 / value2]  
             05 : Int Division          [value1 // value2] (outputs interger only)
             05 : Modulo                [value1 % value2]
             06 : Power                 [value1 ^ value2]
             07 : Exponent to the e     [value1 ^ e]
             08 : Base-10 logarithm     [ log 10 (value 1) ]
             09 : Natural logarithm     [ log e (value 1) ]
             10 : Square root           [ sqrt(value1) ]
             11 : Maximum of two values [ max(value1) ]
             12 : Minimum of two values [ min(value1) ]
             13 : Floor                 [ floor(value1) ]
             14 : Ceiling               [ ceil(value1) ]
             15 : Absolute value        [ abs(value1) ]
             16 : Sine                  [ sin(value1) ]
             17 : Cosine                [ cos(value1) ]
             18 : Tangent               [ tan(value1) ]
             19 : Arc-sine              [ asin(value1) ]
             20 : Arc-cosine            [ acos(value1) ]
             21 : Arc-tangent           [ atan(value1) ]
             22 : Angle of 2 vectors    [ arc-cos( (value1 x value2) / (abs(value1) x abs(value2)) ) ]
             23 : Distance of 2 vectors [ abs( value1 - value2 )  ]
             24 : length of vector      [ abs(sqrt( value1 ^ 2 + value2 ^ 2 )) ]
             25 : 2D simplex noise,      function from mindustry
             26 : Random number,         function from mindustry (doubtfully practical)
             27 : Cosecant              [ 1 / sin(value1) ]
             28 : Secant                [ 1 / cos(value1) ]
             29 : Cotangent             [ 1 / tan(value1) ]
             30 : Arc-cosecant          [ asin( 1 / value1 ) ]
             31 : Arc-secant            [ acos( 1 / value1 ) ]
             32 : Arc-cotangent         [ atan( 1 / value1 ) ]

2 : Logical operations
    01 : EqualTo            [value1 == value2]
    02 : NotEqual           [value1 != value2]
    03 : logical And        [value1 & value2]
    04 : LessThan           [value1 < value2]
    05 : LessOrEqualThan    [value1 <= value2]
    06 : MoreThan           [value1 > value2]
    07 : MoreOrEqualThan    [value1 >= value2]
    08 : StrictEqual        [value1 === value2]
    09 : Bitwise-OR         [value1 || value2]
    10 : Bitwise-AND        [value1 && value2]
    11 : Bitwise-XOR        [value1 ^^ value2]
    12 : Bitwise-Flip       [ flip(value1) ]
    13 : Bitshift-Left      [value1 << value2]
    14 : Bitshift-Right     [value1 >> value2]

3 : Flow control
    01 : Jump
    02 : While
    03 : Call
    04 : Return

4 : Data control
    00 : Data start
    01 : Load
    02 : Move [Moves data either Reg to Mem or Mem to Reg, dataSourceID = value1, dataDestinationID = value2]
    03 : Load Subroutine
    99 : End

5 : None yet
6 : None yet
7 : None yet
8 : None yet
9 : Exception Error
    00 : unknown error, better check every part.
    01 : Kill task (killed a program by request)
    02 : Syntax error
    03 : Subroutine is not detected or not preloaded
    04 : No exit on subroutine (No return block passed in subroutine)
    05 : Invalid data type
```

### 2. Data type of values
this value represents what type of data the value that's being processed has. if one value is unused, then the data type of that value will be ignored and/or skipped.
```
  1 = value1 is a regular number/value, value2 is also a regular number/value
  2 = value1 is a regular number/value, value2 is a register ID
  3 = value1 is a regular number/value, value2 is a memory ID

  4 = value1 is a register ID, value2 is a regular number/value
  5 = value1 is a register ID, value2 is also a register ID
  6 = value1 is a register ID, value2 is a memory ID

  7 = value1 is a memory ID, value2 is a regular number/value
  8 = value1 is a memory ID, value2 is a register ID
  9 = value1 is a memory ID, value2 is also a memory ID

 or

  1 = value2 is a regular number/value, value 1 is a regular number/value
  4 = value2 is a regular number/value, value 1 is a register ID
  7 = value2 is a regular number/value, value 1 is a memory ID

  2 = value2 is a regular register ID, value 1 is a regular number/value
  5 = value2 is a regular register ID, value 1 is a register ID
  8 = value2 is a regular register ID, value 1 is a memory ID

  3 = value2 is a memory ID, value 1 is a regular number/value
  6 = value2 is a memory ID, value 1 is a register ID
  9 = value2 is a memory ID, value 1 is a memory ID

  0 = value1 is a disk ID, value2 is a memory ID
```

### 3. Positive/Negative properties of the values
Values that is specified to be negative will be **multiplied by -1** when calculated.
```
 1 = value1 positive, value2 positive
 2 = value1 negative, value2 positive
 3 = value1 positive, value2 negative
 4 = value1 negative, value2 negative
 any other number will return **1**
```

### 4. Value(s) being processed
The first value is generally called V1 or value1 and the second value can be called V2 or value2.

The range of the value extends from 00000 to 99999. Negative numbers can be achieved by the positive/negative value property or by register or memory address.

Any number higher than this requires a register or memory address which will be explained next.


## Flow control (3)
Flow control operations control how the data processing flows. This section is here to explain each functions in `Flow control`

- Jump (3 01)\
 A function that **jumps a `value2`** ammount of lines **if `value1` is true**. Example syntax:

     ```3 01 4 1 00055 00002``` or ```30141000550002```

     The code above states if the value on register #`55` is 1, the processor will jump `2` lines after this line.

- While (3 02)\
  A While loop function. We decided a for loop is unecessary since it can be achieved with only a while loop.

    **Loops a `value2` ammount of lines if `value1` is still true**. Example syntax:

    ```3 02 4 1 00102 00005``` or ```30241001020005```

    The code above states that if the value on register #`102` is 1, it will run the next `5` lines of code and then checks again if register #`102` is still true, if it is then it will run it until the checked condition is false. 
    
    **The system will not be able to detect infinite loops, so you must terminate the task to stop it.**

- Call (3 03)\
  Calls a preloaded subroutine with a subroutine ID (`value1`) that is just a regular number, so you can use the data type of regular number for this. Example syntax:

  ```3 03 1 1 00005 00000``` or ```303110000500000```

  The code above states to call a subroutine with an ID of #`5`.

- Return (3 04)\
  A return line from the subroutine, must be placed on the end of a subroutine code.


## Data control (4)
Data control operations control data positioning over several devices.
- Data start (4 00)
  Starting line of some data, Must be placed on the start of every data. `value1` and `value2` can and will be used as identifiers for this specific data. if another data has the same identifier. the older one is eradicated.

- Load (4 01)
  Loads data from a disk ID (`value1`) to a memory ID (`value2`), therefore you must use the data type of `0`. Example syntax:

  ```4 01 0 1 04124 00205``` or ```401010412400205```
  
  The code above loads a data from diskID #`4124` to memoryID #`205`

- Move (4 02)
  A function to move data between Registers and Memory(RAM). this function depends on the data type as it moves data from the ID of `value1` to the ID of `value2`. Example syntax:

  ```4 02 5 1 00520 00200``` or ```402510052000200```

  The code above has the data type of `value1` and `value2` to be register ID's, therefore moving data between registers, or in this example moving data from register #`520` to register #`200`.

- Load Subroutine (4 03)
  Loads a regular script from a memory address and assigns it with an ID that can be called mid-code multiple times. `value1` will be noted as the memoryID of the subroutine. Example syntax:

  ```4 03 7 05764 00000``` or ```40370576400000```

  The code above assigns memoryID #`5764` a subroutine ID that can be called anytime during the main code run.

- Data End (4 99)\
  End line for data, must be placed at the end of data line. `value1` and `value2` can and will be used as identifiers combined with the data start identifier creating a 20-digit long unique identifier for this specific data. **otherwise, the system will not be able to detect if the data line ended.**

## Registers and Memory addresses
### 1. Registers
Registers are small memory points that is placed in the CPU core. You might think of L1, L2, or L3 caches but **Caches and Registers are different!**

Caches are also small storages placed around the CPU. but in the context of this MPC, registers are variables in 1 single core, whilst caches are stored in small memory cells around it.

Our Registers uses a tech by @Hasganter called L100 (yes even the name is misleading, sorry lol). basically it unlocks the ability to store values within a processor like a memory cell but if optimized, can be denser than a memory cell (more cells per block).

> *"it is called L100 because it is actually slower than reading from a memory cell lmfao" - hasganter*

So basically it is just extra space filler, meh.

The register address in this context can access **both actual registers and caches** depending on the ID provided.

**Currently, we have not decided on the numbers to access registers yet.**

### 2. Memory Addresses
Memory in this context is the **RAM**. with this ID, the CPU will send a request to the RAM and obtain the value within the memory address provided.

**Currently, we have not decided on the numbers to access the memory yet.**


## Subroutines
Subroutines are basically custom functions that supports the `DRY` programming rule, `Don't Repeat Yourself`.

Subroutines have the same structure as a regular scipt, but loaded as a subroutine.


## Exceptions
Exceptions will occur when certain conditions did not went as expected. the procedure of an exception will be stopping current running program completely by rejecting every code inputted until an end block.
- Unknown error (00)
- Task killed (01)
- Syntax error (02)
- Subroutine is not detected or not preloaded (03)
- No return line in subroutine (04)

## Data structure
Every data will begin with ```4 00 0 0 00000 00000``` and end with ```4 99 0 0 00000 00000``` this is to make data storage easier.

the data start line and the data end line values can be used as ID's for easier identification of the data.

Information lines are another story... uuugh

#
###### Many thanks to everyone involved.