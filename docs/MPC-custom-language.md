# MPC custom language documentation (Story section lmfao)
The programming language for MPC does seem to be a huge roadblock but we have some concepts for now, code it later.

What we are thinking is to have a programming language similar to the level of assembly where its highly oversimplified and few syntax available. This language will be interpreted as base-10 numbers that can be saved in a mindustry memory cell which opens for data transfer between processors.


#
## 1. First impression :
We designed the first interpreted code: `10 100 65536 65536 65536 65536` (spaces is just for readability the real interpreted code does not have them)

**The first digits** `10` indicates the general type of operation or opCodes, such as **Math operation, Data movement, etc.** which will group several commands together for easier reference. (this means we have a max of 89 opCodes). 
Another thing that is we have to start with a number higher than 0 because we cant have 0's in the front of a value in mindustry.

**The second digits** `100` indicates the sub-opCodes. it is a supporting code that indicates a specific operation within the type of operation, such as in the general type of **math operation**, the sub-opCodes will have **addition, subtraction, division, multiplication, etc.** Which can be directly processed and/or handled in mindustry logic.

**The third digits and so on..** `65536` indicates the value to be processed, this specific value is actually **2<sup>16</sup>** where we just randomly decided it to be the max number of a value.

After some testing we found out the mindustry memory cell has some limitations, where if you input a number so high it would **round up the numbers**. based on some testing, we have found the limit before rounding happens, that is:

### 2<sup>53</sup> or `9 007 199 254 740 992`

With this, we can redesign our format to fit the limitations.


#
## 2. Improvements! :
With the newfound info, we can conclude that our interpreted code has to fit within 15 digits (even though th elimit is 16 digits, just to be safe), and redesigned the interpreted code to be:

### ```10 100 99999 99999``` (15 digits, no spaces in the actual interpreted code)

**First digits** `10` same as before, opCodes. indicates the general type of operation such as **Math operation, Data transfer, Flow control, etc.**

**Second digits** `100` same as before, sub-opCodes. indicates the specific type of operation such as in the general type of **math operation**, the sub-opCodes will have **addition, subtraction, division, multiplication, etc.**

**Third and fourth digits** `999999` Theese numbers are the values to be processed. we got rid of the 16-bit limit because why do we even have it, and went with 5 * 2 values to be processed in a single command.

With all that done, all that's left is to assign opCodes and sub-opCodes and **code it all!**


#
## 3. Revision #1
After some thinking, we thought of values higher than the `99999` limit, because admittedly it was small. So we planned of using registrys and memory address to make up for it. with the plan above we eventually end up with this formatting:

```10 12 1 99999 99999```

**First digits** `10` same as before, opCodes. indicates the general type of operation such as **Math operation, Data transfer, Flow control, etc.**

**Second digits** `12` same as before but one digit less, sub-opCodes. indicates the specific type of operation such as in the general type of **math operation**, the sub-opCodes will have **addition, subtraction, division, multiplication, etc.**

**Third digit** `1` This one digit value represents the data types of the values within the line.
> - 1 = value1 is a regular number, value2 is also a regular number
> - 2 = value1 is a regular number, value2 is a registry ID
> - 3 = value1 is a regular number, value2 is a memory ID
> - 4 = value1 is a registry ID, value2 is a normal number
> - 5 = value1 is a registry ID, value2 is also a registry ID
> - 6 = value1 is a registry ID, value2 is a memory ID
> - 7 = value1 is a memory ID, value2 is a normal number
> - 8 = value1 is a memory ID, value2 is a registry ID
> - 9 = value1 is a memory ID, value2 is also a memory ID

**Fourth and fifth digits** `999999` same as before, theese numbers are the values to be processed.


#
# Actual documentation (Skipping story)
Current latest interpreted code formatting:
### ```10 12 1 99999 99999```

`10` : opCodes (Operation Codes). Refrence numbers for the general type of operation.

`12` : sub-opCodes (sub Operation Codes). Reference numbers for the specific type of operation within the opCode.

`1` : data type of values being processed.

`99999` : Value that will be processed.

### 1. opCodes and their respecting sub-opCodes
```
(opCode)10 : Arithmetic operations
(sub-opCode)01 : Addition
            02 : Subtraction
            03 : Multiplication
            04 : Division
            05 : Int Division (outputs interger, throwing numbers behind comma)
            05 : Power (value1 ^ value2)
            06 : Exponent to the e (value1 ^ e)
            07 : Modulo
            08 : 

11 : 
12 : 
13 : 
14 : 
15 : 
16 : 
17 : 
18 : 
19 : 
20 : 
```

### 2. data type of values
```
 - 1 = value1 is a regular number/value, value2 is also a regular number/value
 - 2 = value1 is a regular number/value, value2 is a registry ID
 - 3 = value1 is a regular number/value, value2 is a memory ID

 - 4 = value1 is a registry ID, value2 is a regular number/value
 - 5 = value1 is a registry ID, value2 is also a registry ID
 - 6 = value1 is a registry ID, value2 is a memory ID

 - 7 = value1 is a memory ID, value2 is a regular number/value
 - 8 = value1 is a memory ID, value2 is a registry ID
 - 9 = value1 is a memory ID, value2 is also a memory ID

 or

- 1 = value2 is a regular number/value, value 1 is a regular number/value
- 4 = value2 is a regular number/value, value 1 is a registry ID
- 7 = value2 is a regular number/value, value 1 is a memory ID

- 2 = value2 is a regular registry ID, value 1 is a regular number/value
- 5 = value2 is a regular registry ID, value 1 is a registry ID
- 8 = value2 is a regular registry ID, value 1 is a memory ID

- 3 = value2 is a memory ID, value 1 is a regular number/value
- 6 = value2 is a memory ID, value 1 is a registry ID
- 9 = value2 is a memory ID, value 1 is a memory ID
```

###### Many thanks to everyone involved.