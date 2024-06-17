# MPC custom language documentation (Story section lmfao)
The programming language for MPC does seem to be a huge roadblock but we have some concepts for now, code it later.

What we are thinking is to have a programming language similar to the level of assembly where its highly oversimplified and few syntax available. This language will be interpreted as base-10 numbers that can be saved in a mindustry memory cell which opens for data transfer between processors.

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

**First digits** same as before, opCodes. indicates the general type of operation such as **Math operation, Data transfer, Flow control, etc.**

**Second digits** same as before, sub-opCodes. indicates the specific type of operation such as in the general type of **math operation**, the sub-opCodes will have **addition, subtraction, division, multiplication, etc.**

**Third and fourth digits** `999999` Theese numbers are the values to be processed. we got rid of the 16-bit limit because why do we even have it, and went with 5 * 2 values to be processed in a single command.




#
# Actual documentation (Skipping story)