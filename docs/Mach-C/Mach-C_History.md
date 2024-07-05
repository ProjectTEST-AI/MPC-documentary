Project TEST presents

> # MPC Custom language, nicknamed Mach-C (Mindustry Machine Code)

# Mach-C history documentation

This page concludes the history on the growth of Mach-C, a custom-made low-level language that is compiled and processed inside the game Mindustry with the use of Logic blocks.

Chapters are assigned to each major progression to the language. Each chapter will be / is thoroughly described for it to be understandable, Feel free to add comments for improvements.

Many thanks, The Project TEST team.

##

### 0. Conundrum

The programming language for MPC does seem to be a huge roadblock but we have some concepts for now, code it later.

What we are thinking is to have a programming language similar to the level of assembly where its highly oversimplified and few syntax available. This language will be **interpreted as base-10 numbers** that can be saved in a mindustry memory cell which opens for data transfer between processors.

**(This documentation covers the `interpreted` code's structure and commands.)**

##

### 1. First impression :

We designed the first interpreted code: `10 100 65536 65536 65536 65536` (spaces is just for readability the real interpreted code does not have them)

**The first digits** `10` indicates the general type of operation or **opCodes**, such as **Math operation, Data movement, etc.** which will group several commands together for easier reference. (this means we have a max of 89 opCodes).
Another thing that is we have to start with a number higher than 0 because we cant have 0's in the front of a value in mindustry.

**The second digits** `100` indicates the **sub-opCodes**. it is a supporting code that indicates a specific operation within the type of operation, such as in the general type of **math operation**, the sub-opCodes will have **addition, subtraction, division, multiplication, etc.** Which can be directly processed and/or handled in mindustry logic.

**The third digits and so on..** `65536` indicates the value to be processed, this specific value is actually **2<sup>16</sup>** where we just randomly decided it to be the max number of a value.

After some testing we found out the mindustry memory cell has some limitations, where if you input a number so high it would **round up the numbers**. based on some testing, we have found the limit before rounding happens, that is:

**2<sup>53</sup> or `9 007 199 254 740 992`**

With this, we can redesign our format to fit the limitations.

##

### 2. Improvements! :

With the newfound info, we can conclude that our interpreted code has to fit within 15 digits (even though the limit is 16 digits, just to be safe), and redesigned the interpreted code to be:

**`10 100 99999 99999`** (15 digits, no spaces in the actual interpreted code)

**First digits** `10` same as before, **opCodes**. indicates the general type of operation such as **Math operation, Data transfer, Flow control, etc.**

**Second digits** `100` same as before, **sub-opCodes**. indicates the specific type of operation such as in the general type of **math operation**, the sub-opCodes will have **addition, subtraction, division, multiplication, etc.**

**Third and fourth digits** `999999` Theese numbers are the values to be processed. we got rid of the 16-bit limit because why do we even have it, and went with 5 \* 2 values to be processed in a single command.

With all that done, all that's left is to assign opCodes and sub-opCodes and **code it all!**

##

### 3. Realization

After some thinking, we thought of values higher than the `99999` limit, because admittedly it was small. So we planned of using registers and memory address to make up for it. with the plan above we eventually end up with this formatting:

`10 12 1 99999 99999`

**First digits** `10` same as before, **opCodes**. indicates the general type of operation such as **Math operation, Data transfer, Flow control, etc.**

**Second digits** `12` same as before but one digit less, **sub-opCodes**. indicates the specific type of operation such as in the general type of **math operation**, the sub-opCodes will have **addition, subtraction, division, multiplication, etc.**

**Third digit** `1` This one digit value represents the **datatypes of the values within the line**.

> -   1 = value1 is a regular number, value2 is also a regular number
> -   2 = value1 is a regular number, value2 is a register ID
> -   3 = value1 is a regular number, value2 is a memory ID
> -   4 = value1 is a register ID, value2 is a normal number
> -   5 = value1 is a register ID, value2 is also a register ID
> -   6 = value1 is a register ID, value2 is a memory ID
> -   7 = value1 is a memory ID, value2 is a normal number
> -   8 = value1 is a memory ID, value2 is a register ID
> -   9 = value1 is a memory ID, value2 is also a memory ID
> -   0 = value1 is a disk ID, value2 is a memory ID

**Fourth and fifth digits** `999999` same as before, theese numbers are the values to be processed.

##

### 4. Another forgotten thing

Totally forgot about two things, that is the positive or negative state of a value, and numbers behind commas. If the value was in a memory cell, it would have no problem differentiating between negative nor positive. But within one line of code, we currently cannot differentiate between negative and positive numbers and don't get us started with commas.

With the problem in hand, we decided to let go of the commas, because there is no efficient way to make that work. And to solve the positive/negative number problem we _(again)_ redesigned the interpreted code format to be:

**`1 12 3 4 99999 99999`**

**First digits** `1` same as before but one digit less, **opCodes**. indicates the general type of operation such as **Math operation, Data transfer, Flow control, etc.**

**Second digits** `12` same as before, **sub-opCodes**. indicates the specific type of operation such as in the general type of **math operation**, the sub-opCodes will have **addition, subtraction, division, multiplication, etc.**

**Third digit** `3` same as before, represents datatypes of the values on hand.

**Fourth digit** `4` This one digit value represents the positive/negative status of the values on hand.

> -   1 = value1 positive, value2 positive
> -   2 = value1 negative, value2 positive
> -   3 = value1 positive, value2 negative
> -   4 = value1 negative, value2 negative
> -   any numbers above will return **1**.

**Fifth and sixth digits** `999999` same as before, theese numbers are the values to be processed.

##

### 5. Redesign

Before, we were thinking that the result of an equation can be overwritten on `value1` when available. Realizing it would be just easier to add a third value for the output variable, we want to move things around now. Since we want to add another value to the current formatting, we decided to tap into the 16th digit since the first digit (opCode) will not go over the limit of 9 quadrilion, since the practical opCodes ranges from 1 to 8, therefore not exceeding the limit. The newly designed code format:

`1 12 34 9999 9999 999`

**First digits** `1` same as before but one digit less, **opCodes**. indicates the general type of operation such as **Math operation, Data transfer, Flow control, etc.**

**Second digits** `12` same as before, **sub-opCodes**. indicates the specific type of operation such as in the general type of **math operation**, the sub-opCodes will have **addition, subtraction, division, multiplication, etc.**

**Third digit** `34` This two digit value represents the datatypes of all values AND `value1` / `value2` properties (negative/positive). This will be thoroughly described in the Actual documentation part.

**Fourth and fifth digits** `999999` same as before, theese numbers are the values to be processed.

**Sixth digit** `999` This three digit value represents either a Register address or a Memory address as the target of the code line's output.

##

### 6. A whole new realm

So @hasganter just had a brainfart; Our formatting structure right now uses base-10 numerical digits, but what if we use base-2 binary system within base-10 numerical system, confused? ill explain.

Currently, our formatting structure uses base-10 numerical system (numbers 1-10). We section off certain parts of a whole number and assigning it to a specific value like:

`1 12 34 9999 9999 999` or `1123499999999999`

`1` : **opCodes** (Operation Codes). Refrence numbers for the **general** type of operation.

`12` : **sub-opCodes** (sub Operation Codes). Reference numbers for the **specific** type of operation within the opCode.

`34` : **Datatype** of values being processed and **positive/negative property** of the values (`value1` or `value2`) being processed.

`9999` : **Primary value** to be processed.

`9999` : **Secondary value** to be processed.

`999` : **Register or Memory address target for line's output.**

This results in one value that can be stored in one single memory cell easily with some readability. But, what if we **throw readability out of the window and maximize usability from one singluar value that can be stored on a memory cell**?

Thats where the base-2 binary system comes to mind. Base-2 system uses only 2 numbers whereas base-10 numerical system uses 10. between both the numerical systems, we can convert them to eachother. For example:

Base-2 `01101` = Base-10 `13`. Learn more about binary to base-10 conversion [Here](https://www.wikihow.com/Convert-from-Binary-to-Decimal)

From what we are currently seeing here, it seems like base-2 just takes more space yet having less data stored efficiently. But in our context it has more power than we thought.

Mindustry logic's interger limit is 2<sup>53</sup> or `9 007 199 254 740 992` in base-10. but in base-2 system we have 53 digits of 1 or 0's. Now why is this important?

well, instead of partitioning base-10 numbers to have multiple data stored at once, we can use binary! How? well, see for yourself:

`111 00000 11 00 1 0 0 1111..(x13) 0000...(x13) 11111..(x12) `

`111` : opCodes (converted to base-10, it can store a number from 0 to 7)

`00000` : sub-opCodes (0 to 31)

`11` : datatype of value1 (0 to 3, even though we only need 0, 1, and 2)

`00` : datatype of value2 (0 to 3, even though we only need 0, 1, and 2)

`1` : datatype of value3 (0 and 1)

`0` : negative / positive property of value1 (0 and 1)

`0` : negative / positive property of value2 (0 and 1)
