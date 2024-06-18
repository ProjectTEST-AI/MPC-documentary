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

`10` : opCodes (Operation Codes). Refrence numbers for the general type of operation.

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
(sub-opCode) 01 : Addition
             02 : Subtraction
             03 : Multiplication
             04 : Division
             05 : Int Division (outputs interger, throwing numbers behind comma)
             05 : Modulo
             06 : Power (value1 ^ value2)
             07 : Exponent to the e (value1 ^ e)
             08 : base-10 logarithm (value 1)
             09 : logarithm (value1 = base, value2 = anti-logarithm) = ( Log value1(value2) )
             10 : Maximum of two values
             11 : Minimum of two values

2 : Logical operations
    01 :
    02 :
    03 :
    04 :
    05 :
    06 :

3 : Flow control

4 : Data control

5 :  
6 : 
7 : 
8 : 
9 : 
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
```

### 3. Positive/Negative properties of the values
Self explanatory.
```
 1 = value1 positive, value2 positive
 2 = value1 negative, value2 positive
 3 = value1 positive, value2 negative
 4 = value1 negative, value2 negative
 any numbers above will return **1**
```

### 4. Value(s) being processed
the first value is generally called V1 or value1 and the second value can be called V2 or value2.

the range of the value extends from 00000 to 99999. any number higher than this requires a register or memory address which will be explained next.


## Register and Memory addresses
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


## Next thing here

###### Many thanks to everyone involved.