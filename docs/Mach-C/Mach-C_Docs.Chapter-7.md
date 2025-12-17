Project TEST presents

# Mach-C - Chapter 7

MPC's Custom system language, nicknamed Mach-C (Mindustry Machine Code).

This documentation covers **Chapter 7** of the Mach-C language for the MPC project.

> ### Current interpreted code formatting (Chapter 7) is now _Dynamically implemented._

The structure of the code is no longer fixed to one single format, but rather multiple formats that are chosen based on the opcode of the instruction.

This was decided so it can be easily adaptible and not constraining in implementation.

# Contents
1. [CPU Execution Model](#cpu-execution-model)
2. [Architecture & Data Addressing](#architecture--data-addressing)
3. [Assembly Language Syntax](#readable-mach-c-assembly-syntax)
4. [Instruction Formats](#formats-of-instructions)
5. [Encoding Notes](#encoding-notes)

#

# CPU Execution Model

The MPC processor executes instructions stored in the L2 cache memory. Understanding the execution model is essential for programming the system.

## Instruction Pointer Behavior

- **Starting Point**: The processor begins execution at cache address `@0`. For multi-core configurations, each processor starts at cache address equal to its `CPUID` register value (e.g., core 0 starts at `@0`, core 1 starts at `@1`, etc.).
- **Sequential Execution**: After executing an instruction, the instruction pointer automatically advances to the next cache address.
- **Wrap-Around**: When the instruction pointer reaches the end of the cache address space, it wraps around to `@0` and continues execution.
- **Stopping Conditions**: Execution pauses when the instruction pointer encounters:
  - `NOP` (No Operation) - Skips and continues
  - `HLT` (Halt) - Stops execution completely until reset
  - Invalid opcodes - Triggers exception handling

## Multi-Core Considerations

Each processor core has a unique `CPUID` register (read-only, address `0x01`) that identifies it. This allows:
- Partitioned code execution across cores
- Core-specific branching logic
- Synchronized multi-core operations via shared cache/RAM

#

# Architecture & Data Addressing

The MPC uses **separate address spaces** for different memory types:

| Address Space | Size                    | Description                      |
| ------------- | ----------------------- | -------------------------------- |
| Registry      | 64 locations (6-bit)    | Per-processor internal registers |
| Cache         | 2^32 locations (32-bit) | Fast L2 cache (Memory Banks)     |
| RAM/Unified   | 2^32 locations (32-bit) | External memory via RAM module   |

## A. Registry (Internal processor)

Registers are stored within the CPU core using L100 technology. 6-bit addressing supports 64 registers.

#### SYSTEM REGISTERS (0x00 - 0x0F)

| Address | Decimal | Name | Description |
|---------|---------|------|-------------|
| 0x00 | 00 | ZERO | Immutable zero constant (always 0) |
| 0x01 | 01 | CPUID | Processor core ID (read-only, used for multi-core startup offset) |
| 0x02 | 02 | PC | Program counter |
| 0x03 | 03 | SP | Stack pointer |
| 0x04 | 04 | RA | Return address (saved by CALL) |
| 0x05 | 05 | FLAGS | Status flags register (see FLAGS structure below) |
| 0x06 | 06 | PI | Constant π (3.14159..., read-only, native Mindustry) |
| 0x07 | 07 | E | Constant e (2.71828..., read-only, native Mindustry) |
| 0x08 | 08 | LINK | Current link count (read-only) |
| 0x09 | 09 | TIME | Game tick counter (read-only) |
| 0x0A | 10 | UNIT | Currently bound unit reference |
| 0x0B | 11 | THIS | Current processor reference (read-only) |
| 0x0C | 12 | IPTR | Instruction pointer (alias for PC) |
| 0x0D | 13 | CNTR | General counter register |
| 0x0E | 14 | ACCM | Accumulator register |
| 0x0F | 15 | - | Reserved for future system use |

#### ARGUMENT REGISTERS (0x10 - 0x1F)

| Address | Decimal | Name | Description |
|---------|---------|------|-------------|
| 0x10 | 16 | A0 | Argument register 0 |
| 0x11 | 17 | A1 | Argument register 1 |
| 0x12 | 18 | A2 | Argument register 2 |
| 0x13 | 19 | A3 | Argument register 3 |
| 0x14 | 20 | A4 | Argument register 4 |
| 0x15 | 21 | A5 | Argument register 5 |
| 0x16 | 22 | A6 | Argument register 6 |
| 0x17 | 23 | A7 | Argument register 7 |
| 0x18-0x1F | 24-31 | A8-AF | Argument registers 8-15 |

#### TEMPORARY REGISTERS (0x20 - 0x2F)

| Address | Decimal | Name | Description |
|---------|---------|------|-------------|
| 0x20 | 32 | T0 | Temporary register 0 (caller-saved) |
| 0x21 | 33 | T1 | Temporary register 1 |
| 0x22 | 34 | T2 | Temporary register 2 |
| 0x23 | 35 | T3 | Temporary register 3 |
| 0x24 | 36 | T4 | Temporary register 4 |
| 0x25 | 37 | T5 | Temporary register 5 |
| 0x26 | 38 | T6 | Temporary register 6 |
| 0x27 | 39 | T7 | Temporary register 7 |
| 0x28-0x2F | 40-47 | T8-TF | Temporary registers 8-15 |

#### SAVED REGISTERS (0x30 - 0x3F)

| Address | Decimal | Name | Description |
|---------|---------|------|-------------|
| 0x30 | 48 | S0 | Saved register 0 (caller-saved) |
| 0x31 | 49 | S1 | Saved register 1 |
| 0x32 | 50 | S2 | Saved register 2 |
| 0x33 | 51 | S3 | Saved register 3 |
| 0x34 | 52 | S4 | Saved register 4 |
| 0x35 | 53 | S5 | Saved register 5 |
| 0x36 | 54 | S6 | Saved register 6 |
| 0x37 | 55 | S7 | Saved register 7 |
| 0x38-0x3F | 56-63 | S8-SF | Saved registers 8-15 |

### `FLAGS` Register Structure (0x04)

| Bit | Name | Description |
|-----|------|-------------|
| 0 | ZF | Zero flag (result was zero) |
| 1 | SF | Sign flag (result was negative) |
| 2 | OF | Overflow flag (signed overflow) |
| 3 | CF | Carry flag (unsigned overflow/borrow) |
| 4 | EF | Equal flag (comparison result) |
| 5 | LF | Less-than flag (comparison result) |
| 6 | GF | Greater-than flag (comparison result) |
| 7 | IF | Interrupt enable flag |
| 8 | HF | Halt flag (processor halted) |
| 9 | EXF | Exception flag (exception occurred) |
| 10 | MF | Memory fault flag |
| 11 | UF | Unit bound flag |
| 12-31 | - | Reserved for future use |

## B. L2 Cache (Memory Banks Only)

L2 cache uses Mindustry Memory Banks placed near the processor. Each memory cell stores a single value—whether negative, positive, decimal, etc., data types are handled natively by Mindustry. Users must process or accommodate specific data types themselves.

Since each memory bank only holds 512 cells, all linked banks are combined into one **unified cache address space** using a 32-bit address.

### Unified Cache Addressing

When accessing a cache address, the processor calculates:
- **Bank ID** = `address // 512` (integer division)
- **Cell Index** = `address % 512` (modulo)

```
--- L2 CACHE ADDRESS RESOLUTION ---
Address Space: 32-bit (0x00000000 - 0xFFFFFFFF)

Example: Accessing cache address @515 (0x203)
  Bank ID   = 515 // 512 = 1    → Memory Bank 1
  Cell Index = 515 % 512 = 3    → Cell 3 within that bank

Example: Accessing cache address @1030
  Bank ID   = 1030 // 512 = 2   → Memory Bank 2
  Cell Index = 1030 % 512 = 6   → Cell 6 within that bank
```

### Access Pattern

```
1. Load 32-bit address into a register (STR or arithmetic)
2. Issue cache memory operation with address register
3. Processor resolves Bank ID and Cell Index internally
4. Data transferred to/from destination register

Example:
  STR  #A0 515        ; Store address 515 into register A0
  LRC  #T0 #A0        ; Load from CACHE[515] (Bank 1, Cell 3) to T0
  SRC  #T1 #A0        ; Store T1 to CACHE[515]
```

## C. RAM (Memory Banks via RAM Module)

RAM provides external memory located outside the CPU/processor space. RAM is accessed exclusively through cache-to-RAM operations (F_CM format). Direct register-to-RAM operations are not supported.

### RAM Addressing Model

Each RAM address points to exactly **8 cells** within the RAM module. This block-based addressing enables efficient bulk transfers between cache and RAM.

- **Block Address** = RAM address (32-bit)
- **Block Size** = 8 cells per address
- **Cell Range** = `[address * 8]` to `[address * 8 + 7]`

```
--- RAM ADDRESS RESOLUTION ---
Address Space: 32-bit (0x00000000 - 0xFFFFFFFF)
Cells per Address: 8

Example: Accessing RAM address $128
  Starting Cell = 128 * 8 = 1024  → Cell 1024
  Ending Cell   = 128 * 8 + 7 = 1031  → Cell 1031
  Total Cells   = 8 cells (1024-1031)

Example: Accessing RAM address $0
  Starting Cell = 0 * 8 = 0       → Cell 0
  Ending Cell   = 0 * 8 + 7 = 7   → Cell 7
  Total Cells   = 8 cells (0-7)
```

### Access Pattern

RAM can only be accessed through cache operations (F_CM format):

```
1. Load 32-bit RAM address into a register
2. Issue cache-memory operation (F_CM format)
3. RAM module resolves the 8-cell block
4. Data transferred between cache and RAM block

Example (bulk transfer - 8 cells):
  STR  #A0 128        ; Store RAM address 128 into register A0
  STR  #A1 1000       ; Store cache address 1000 into register A1
  LCM  #A1 #A0        ; Load 8 cells from RAM[128] (cells 1024-1031) to CACHE[1000-1007]
  SCM  #A1 #A0        ; Store 8 cells from CACHE[1000-1007] to RAM[128]

Example (single cell access):
  STR  #A0 128        ; Store RAM address 128 into register A0
  STR  #A1 1000       ; Store cache address 1000 into register A1
  STR  #A2 3          ; Store cell offset (0-7) into register A2
  LCMS #A1 #A0 #A2    ; Load single cell at offset 3 from RAM[128] to CACHE[1000]
```

> **Note:** RAM operations may take additional cycles compared to cache due to external module communication. Use bulk transfers (LCM/SCM) when possible for efficiency.

#

# Readable Mach-C Assembly Syntax

The Mach-C compiler accepts human-readable assembly code and compiles it to the 53-bit encoded instruction format. This section documents the text-based syntax.

## General Syntax

```
INSTRUCTION operand1 [operand2] [operand3] [operand4]  / comment lines

/ comment lines
```

- **Instructions** are case-insensitive (e.g., `ADD`, `add`, `Add` all work)
- **Comments** start with `/` and extend to end of line
- **Labels** end with `:` and define jump targets (e.g., `loop_start:`)
- **Blank lines** and comment-only lines are ignored

## Operand Prefixes

| Prefix | Type | Description | Example |
|--------|------|-------------|---------|
| `#` | Register | CPU register (0-63) | `#16`, `#T0`, `#A0` |
| `@` | Cache | L2 Cache address | `@1000`, `@0x400` |
| `$` | RAM | External RAM module | `$8192` |
| (none) | Immediate | Literal numeric value | `42`, `100` |

> **Note:** Building and unit references (obtained via `LINK` or `UBND`) can only be stored in registers. They cannot be saved to cache or RAM.

## Instruction Examples by Format

### F_RR (Register-Register) - Opcodes 1-51

```
MOV #16 #17      / Move: R16 = R17
SWP #16 #17      / Swap: R16 <=> R17
ADD #16 #17      / Add: R16 = R16 + R17
SUB #32 #33      / Subtract: R32 = R32 - R33
MUL #T0 5        / Multiply: T0 = T0 * 5
SQR #20          / Square root: R20 = sqrt(R20)
SIN #21          / Sine: R21 = sin(R21)
MAX #22 #23      / Maximum: R22 = max(R22, R23)
AND #24 255      / Bitwise AND: R24 = R24 & 255
```

### F_RI (Register-Immediate) - Opcodes 52-61

```
STR #16 42       / Store immediate: R16 = 42
ADDI #17 10      / Add immediate: R17 = R17 + 10
SUBI #18 5       / Subtract immediate: R18 = R18 - 5
MULI #19 2       / Multiply immediate: R19 = R19 * 2
SHLI #20 3       / Shift left immediate: R20 = R20 << 3
```

### F_RC (Register-Cache) - Opcodes 62-66

```
LRC #T0 #A0      / Load from cache: T0 = CACHE[A0]
SRC #T1 #A1      / Store to cache: CACHE[A1] = T1
XCHC #T2 #A2     / Exchange: swap(T2, CACHE[A2])
CCPY #A0 #A1 #A2 / Bulk cache copy: copy A2 values from CACHE[A0] to CACHE[A1]
```

### F_CM (Cache-Memory) - Opcodes 67-76

```
LCM #A0 #A1        / Load 8 cells from RAM[A1] to CACHE[A0..A0+7]
SCM #A0 #A1        / Store 8 cells from CACHE[A0..A0+7] to RAM[A1]
LCMS #A0 #A1 #A2   / Load single cell at offset A2 from RAM[A1] to CACHE[A0]
SCMS #A0 #A1 #A2   / Store single cell from CACHE[A0] to RAM[A1] at offset A2
CMCPY #A0 #A1 #A2  / Bulk copy: copy A2*8 cells from RAM[A1] to CACHE[A0]
CMSTR #A0 #A1 #A2  / Bulk copy: copy A2*8 cells from CACHE[A0] to RAM[A1]
```

### F_RW (Register-World) - Opcodes 77-96

```
LINK #S0 0        / Get linked building at index 0 into S0
SENS #T0 #S1 #S2  / Sense property S2 from building S1 (Correction: param1=Building, param2=Property)
CTRL #S0 #A0 #A1  / Control building S0 with params
DRAW #A0 #A1 #A2  / Draw operation
PRNT #T0          / Print value
```

### F_JC (Flow Control) - Opcodes 97-106

```
JMP @100          / Jump to cache address @100
JEQ #16 #17 @200  / Jump to @200 if R16 == R17
JNE #16 #17 @300  / Jump to @300 if R16 != R17
JLT #18 #19 @400  / Jump to @400 if R18 < R19
CALL @1000        / Call subroutine at cache address @1000
RET               / Return from subroutine
HLT               / Halt execution
```

> **Note:** All jump/call targets are **cache addresses** where the instruction pointer will be redirected.

## Compiler CLI

The MPC compiler is a command-line tool that compiles readable Mach-C code into a format that can be processed *In-game* (as binary).

The compiler is also suited with a decompiler mode that can decompile an already compiled Mach-C binary (represented as decimals) into readable Mach-C code.

> **Note:** The compiler is currently in development and may not be fully accurate.

```bash
machc [options]

Options:
  -i, --input <file>       Input file path
  -s, --string <code>      Input code as string
  -o, --output <file>      Output file path
  -d, --decompile          Decompile mode
  -n <threads>             Thread count (0=auto)
  --benchmark-mode <iter>  Run benchmark iterations
  -l, --log-level <0-3>    Set log level
  --max-memory <MB>        Max memory limit
  --map-file-to-memory     Use memory mapping (fast)
  -y                       Skip any confirmation(s)
  -h, --help               Shows this help message
```

### Example Usage

```bash
# Compile a file
machc -i program.mach -o output.bin -y

# Compile inline code
machc -s "ADD #16 #17" -y

# Compile with verbose logging
machc -i program.mach -l 3 -y
```

#

# Formats Of Instructions

The following are the formats of instructions that will be decoded based on opcodes.

## A. Format 0 (F_0) : NOOP

```
Bits:     [52:46] [45:0]
Desc:      OpCode padding
Count:       7b     46b
```

`OpCode` = Type of operation to be run. F_0 will only run for OpCodes 0.

`padding` = Unused bits that serves as padding to keep the size of the instructions consistent. (ignored)

### - Types of operations (OpCode **0**)

```
000  :  NOP     |  No operation, skip
```

## B. Format 1 (F_RR) : Register - Register

```
Bits:     [52:46] [45:40] [39:34] [33:28] [27:22] [21:16] [15:0]
Desc:      OpCode   out    src1    src2    opt1    opt2   padding
Count:       7b     6b      6b      6b      6b      6b      16b
```

`OpCode` = Type of operation to be run. F_RR will only run for OpCodes from 1 to 51.

`out` = The Registry address that the output of this instruction will be pushed to. (usually the same as `src1`)

`src1` = The Registry address of the first value being calculated.

`src2` = The Registry address of the second value being calculated. (ignored for some instructions)

`opt1` = An optional registry address used for specific instructions, used for input/output registries. (ignored for some instructions)

`opt2` = An optional registry address used for specific instructions, used for input/output registries. (ignored for some instructions)

`padding` = Unused bits that serves as padding to keep the size of the instructions consistent. (ignored)

### - Types of operations (OpCode **1 - 51**)

#### REGISTER TRANSFER

| OpCode | Mnemonic | Operation | Description |
|--------|----------|-----------|-------------|
| 001 | MOV | `src1 => out` | Move value from src1 to out |
| 002 | SWP | `src1 <=> src2` | Swap values between src1 and src2 |

#### MATH

| OpCode | Mnemonic | Operation | Description |
|--------|----------|-----------|-------------|
| 003 | ADD | `src1 + src2 => out` | Summation |
| 004 | SUB | `src1 - src2 => out` | Subtraction |
| 005 | MUL | `src1 * src2 => out` | Multiplication |
| 006 | DIV | `src1 / src2 => out` | Regular division |
| 007 | IDV | `src1 // src2 => out` | Integer division |
| 008 | MOD | `src1 % src2 => out` | Modulo |
| 009 | TMD | `src1 %% src2 => out` | True modulo |
| 010 | POW | `src1 ^ src2 => out` | Exponentiation by value |
| 011 | LOG | `{src2}LOG(src1) => out` | Logarithm of `src1` with base `src2` |
| 012 | SQR | `sqrt(src1) => out` | Square root |
| 013 | MAX | `MAX(src1, src2) => out` | Maximum value between both registries |
| 014 | MIN | `MIN(src1, src2) => out` | Minimum value between both registries |
| 015 | FLR | `FLOOR(src1) => out` | Floor value |
| 016 | CIL | `CEIL(src1) => out` | Ceiling value |
| 017 | ABS | `ABS(src1) => out` | Absolute value |
| 018 | SIN | `SIN(src1) => out` | Sine value in degrees |
| 019 | COS | `COS(src1) => out` | Cosine value in degrees |
| 020 | TAN | `TAN(src1) => out` | Tangent value in degrees |
| 021 | ASN | `ASIN(src1) => out` | Arc-Sine value in degrees |
| 022 | ACS | `ACOS(src1) => out` | Arc-Cosine value in degrees |
| 023 | ATN | `ATAN(src1) => out` | Arc-Tangent value in degrees |
| 024 | CSC | `1 / SIN(src1) => out` | Cosecant value in degrees |
| 025 | SEC | `1 / COS(src1) => out` | Secant value in degrees |
| 026 | COT | `1 / TAN(src1) => out` | Cotangent value in degrees |
| 027 | ACSC | `ASIN(1 / src1) => out` | Arc-Cosecant value in degrees |
| 028 | ASEC | `ACOS(1 / src1) => out` | Arc-Secant value in degrees |
| 029 | ACOT | `ATAN(1 / src1) => out` | Arc-Cotangent value in degrees |

#### COMPARISONS

| OpCode | Mnemonic | Operation | Description |
|--------|----------|-----------|-------------|
| 030 | AND | `src1 && src2 => out` | Bitwise and operator |
| 031 | OR | `src1 \|\| src2 => out` | Bitwise or operator |
| 032 | XOR | `src1 ^^ src2 => out` | Bitwise xor operator |
| 033 | NOT | `NOT(src1) => out` | Bitwise flip (flips every bit) |
| 034 | SHL | `src1 << src2 => out` | Bit shift left |
| 035 | SHR | `src1 >> src2 => out` | Bit shift right |
| 036 | EQ | `src1 == src2 => out` | Logical equality |
| 037 | NEQ | `src1 != src2 => out` | Logical inequality |
| 038 | STE | `src1 === src2 => out` | Strict equality |
| 039 | LAD | `src1 & src2 => out` | Logical and operator |
| 040 | LTH | `src1 < src2 => out` | Logical less than operator |
| 041 | LTE | `src1 <= src2 => out` | Logical less or equal than operator |
| 042 | MTH | `src1 > src2 => out` | Logical more than operator |
| 043 | MTE | `src1 >= src2 => out` | Logical more or equal than operator |

#### NATIVE MINDUSTRY

| OpCode | Mnemonic | Operation | Description |
|--------|----------|-----------|-------------|
| 044 | A2V | `ACOS((src1 * src2) / (ABS(src1 * src2))) => out` | Angle of two vectors in degrees |
| 045 | ADV | `ABS(src1 - src2) => out` | Absolute distance of two angles in degrees |
| 046 | LOV | `ABS(SQRT(src1^2 + src2^2)) => out` | Length of vector |
| 047 | 2DS | `NOISE(src1, src2) => out` | 2D simplex noise |
| 048 | RDN | `RAND(0, src1) => out` | Random number between 0 and `src1` |
| 049 | PKC | `PACKCOLOR(src1, src2, opt1, opt2) => out` | Color packing (R, G, B, A) to `out` |
| 050 | UPC | `UNPACKCOLOR(src1) => out, src2, opt1, opt2` | Color unpacking to R, G, B, A |
| 051 | SGN | `SIGN(src1) => out` | The positive/negative/zero value of `src1` |

## C. Format 2 (F_RI) : Register - Immediate

```
Bits:     [52:46] [45:40] [39:8] [7:0]
Desc:      OpCode   out    src1  padding
Count:       7b     6b     32b     8b
```

`OpCode` = Type of operation to be run. F_RI will only run for OpCodes from 52 to 61.

`out` = The Registry address that the output of this instruction will be pushed to.

`src1` = A 32 bit immediate value to be processed/calculated in this instruction.

`padding` = Unused bits that serves as padding to keep the size of the instructions consistent. (ignored)

### - Types of operations (OpCode **52 - 61**)

#### IMMEDIATE OPERATIONS

| OpCode | Mnemonic | Operation | Description |
|--------|----------|-----------|-------------|
| 052 | STR | `src1 => out` | Store immediate value to registry address `out` |
| 053 | STRF | `src1 => FLAGS` | Store immediate value to FLAGS register |
| 054 | ADDI | `out + src1 => out` | Add immediate to register |
| 055 | SUBI | `out - src1 => out` | Subtract immediate from register |
| 056 | MULI | `out * src1 => out` | Multiply register by immediate |
| 057 | DIVI | `out / src1 => out` | Divide register by immediate |
| 058 | ANDI | `out && src1 => out` | Bitwise AND with immediate |
| 059 | ORI | `out \|\| src1 => out` | Bitwise OR with immediate |
| 060 | XORI | `out ^^ src1 => out` | Bitwise XOR with immediate |
| 061 | SHLI | `out << src1 => out` | Shift left by immediate |

## D. Format 3 (F_RC) : Register - Cache

```
Bits:     [52:46] [45:40] [39:34] [33:28] [27:0]
Desc:      OpCode  dest  addrReg srcReg padding
Count:       7b     6b      6b      6b     28b
```

`OpCode` = Type of operation to be run. F_RC will only run for OpCodes from 62 to 66.

`dest` = Destination registry address for loaded data.

`addrReg` = Registry containing the cache address.

`srcReg` = Source registry for stores, or size parameter for copy.

`padding` = Unused bits for instruction consistency. (ignored)

### - Types of operations (OpCode **62 - 66**)

| OpCode | Mnemonic | Operation | Description |
|--------|----------|-----------|-------------|
| 062 | LRC | `CACHE[addrReg] => dest` | Load from cache to register |
| 063 | SRC | `srcReg => CACHE[addrReg]` | Store register to cache |
| 064 | XCHC | `CACHE[addrReg] <=> srcReg` | Exchange cache and register |
| 065 | CCPY | `CACHE[addrReg..addrReg+srcReg] => CACHE[dest]` | Bulk cache copy: copy srcReg values from CACHE[addrReg] to CACHE[dest] |
| 066 | - | (reserved) | Reserved for future use |

## E. Format 4 (F_CM) : Cache - Memory (RAM)

```
Bits:     [52:46] [45:40] [39:34] [33:28] [27:0]
Desc:      OpCode cacheReg ramReg offsetReg padding
Count:       7b     6b      6b      6b     28b
```

`OpCode` = Type of operation to be run. F_CM will only run for OpCodes from 67 to 73.

`cacheReg` = Registry containing the cache address.

`ramReg` = Registry containing the RAM address (each RAM address = 8 cells).

`offsetReg` = Registry containing cell offset (0-7) for single-cell operations, or size for bulk operations.

`padding` = Unused bits for instruction consistency. (ignored)

> **Note:** Each RAM address points to exactly 8 cells. Bulk operations (LCM/SCM) transfer all 8 cells. Single-cell operations (LCMS/SCMS) use the offset register to select one cell within the 8-cell block.

### - Types of operations (OpCode **67 - 73**)

| OpCode | Mnemonic | Operation | Description |
|--------|----------|-----------|-------------|
| 067 | LCM | `RAM[ramReg*8..(ramReg*8)+7] => CACHE[cacheReg..cacheReg+7]` | Load 8 cells from RAM block to cache |
| 068 | SCM | `CACHE[cacheReg..cacheReg+7] => RAM[ramReg*8..(ramReg*8)+7]` | Store 8 cells from cache to RAM block |
| 069 | LCMS | `RAM[ramReg*8 + offsetReg] => CACHE[cacheReg]` | Load single cell at offset from RAM block to cache |
| 070 | SCMS | `CACHE[cacheReg] => RAM[ramReg*8 + offsetReg]` | Store single cell from cache to RAM block at offset |
| 071 | CMCPY | `RAM[ramReg*8..ramReg*8+(offsetReg*8)] => CACHE[cacheReg]` | Bulk copy: copy offsetReg blocks (offsetReg*8 cells) from RAM to cache |
| 072 | CMSTR | `CACHE[cacheReg..cacheReg+(offsetReg*8)] => RAM[ramReg*8]` | Bulk copy: copy offsetReg blocks (offsetReg*8 cells) from cache to RAM |
| 073 | LEA | `ramReg + offsetReg => cacheReg` | Load effective address (address calculation) |

## F. Format 5 (F_RW) : Register - World (Mindustry interactive)

```
Bits:     [52:46] [45:40] [39:34] [33:28] [27:22] [21:0]
Desc:      OpCode   out   param1  param2  param3 padding
Count:       7b     6b      6b      6b      6b     22b
```

`OpCode` = Type of operation to be run. F_RW will only run for OpCodes from 74 to 93.

`out` = The Registry address for the output of this operation.

`param1` = Registry address for first parameter (often building reference).

`param2` = Registry address for second parameter.

`param3` = Registry address for third parameter.

`padding` = Unused bits for instruction consistency. (ignored)

> **Note:** Building and unit references (from LINK/UBND) can only be stored in registers. They cannot be saved to cache or RAM.

### - Types of operations (OpCode **74 - 93**)

#### SENSOR & CONTROL

| OpCode | Mnemonic | Operation | Description |
|--------|----------|-----------|-------------|
| 074 | SENS | `SENSE(param1, param2) => out` | Sense property from building |
| 075 | CTRL | `CONTROL(out, param1, param2)` | Control building with params |
| 076 | RADR | `RADAR(param1, param2, param3) => out` | Radar detection from building |
| 077 | LINK | `GETLINK(param1) => out` | Get linked building at index |
| 078 | LNKC | `@links => out` | Get total link count |

#### DRAWING

| OpCode | Mnemonic | Operation | Description |
|--------|----------|-----------|-------------|
| 079 | DRAW | `DRAW(out, param1, param2)` | Draw operation |
| 080 | DRWF | `DRAWFLUSH(out)` | Flush draw buffer to display |
| 081 | DCOL | `DRAWCOL(out, param1, param2, param3)` | Set draw color (R,G,B,A) |
| 082 | DSTR | `DRAWSTROKE(out)` | Set stroke width |

#### PRINTING

| OpCode | Mnemonic | Operation | Description |
|--------|----------|-----------|-------------|
| 083 | PRNT | `PRINT(out)` | Print value to buffer |
| 084 | PRTF | `PRINTFLUSH(out)` | Flush print buffer to message |

#### UNIT CONTROL

| OpCode | Mnemonic | Operation | Description |
|--------|----------|-----------|-------------|
| 085 | UBND | `UBIND(out)` | Bind unit by type stored in `out` |
| 086 | UCTL | `UCONTROL(out, param1, param2)` | Control bound unit |
| 087 | URDR | `URADAR(out, param1, param2)` | Radar from bound unit |
| 088 | ULOC | `ULOCATE(out, param1)` | Locate feature |

#### WORLD INFO

| OpCode | Mnemonic | Operation | Description |
|--------|----------|-----------|-------------|
| 089 | WSET | `SETPROP(out, param1, param2)` | Set world property |
| 090 | WFLG | `SETFLAG(out, param1)` | Set world flag |
| 091 | WRUL | `SETRULE(out, param1)` | Set world rule |
| 092 | WFCH | `FETCH(out, param1, param2, param3)` | Fetch world object |
| 093 | WEFF | `EFFECT(out, param1, param2, param3)` | Create visual effect |

## G. Format 6 (F_JC) : Flow Control

```
Bits:     [52:46] [45:40] [39:34] [33:2] [1:0]
Desc:      OpCode  cond    reg    target padding
Count:       7b     6b      6b     32b     2b
```

`OpCode` = Type of operation to be run. F_JC will only run for OpCodes from 94 to 103.

`cond` = Registry address containing the condition value.

`reg` = Registry address for comparison or return address storage.

`target` = 32-bit **cache address** where the instruction pointer will jump to (supports up to 17 billion instructions).

`padding` = Unused bits for instruction consistency. (ignored)

### - Types of operations (OpCode **94 - 103**)

#### FLOW CONTROL

| OpCode | Mnemonic | Operation | Description |
|--------|----------|-----------|-------------|
| 094 | JMP | `goto target` | Unconditional jump to cache address |
| 095 | JEQ | `if cond == reg, goto target` | Jump if equal |
| 096 | JNE | `if cond != reg, goto target` | Jump if not equal |
| 097 | JLT | `if cond < reg, goto target` | Jump if less than |
| 098 | JLE | `if cond <= reg, goto target` | Jump if less or equal |
| 099 | JGT | `if cond > reg, goto target` | Jump if greater than |
| 100 | JGE | `if cond >= reg, goto target` | Jump if greater or equal |
| 101 | CALL | `call target; PC => RA` | Subroutine call to cache address (saves return address) |
| 102 | RET | `goto RA` | Return from subroutine |
| 103 | HLT | `halt` | Halt execution |

## H. Format 7 (F_EX) : Exception Handling

```
Bits:     [52:46] [45:40] [39:8] [7:0]
Desc:      OpCode  errcode handler padding
Count:       7b      6b     32b     8b
```

`OpCode` = Always **`127`** for exception format.

`errcode` = Error code identifier (see list below).

`handler` = 32-bit cache address of exception handler routine.

`padding` = Unused bits for instruction consistency. (ignored)

### - Exception Error Codes

| Code | Mnemonic | Description |
|------|----------|-------------|
| 0x00 | ERR_UNKNOWN | Unknown/general error |
| 0x01 | ERR_DIVZERO | Division by zero |
| 0x02 | ERR_OVERFLOW | Integer overflow |
| 0x03 | ERR_UNDERFLOW | Integer underflow |
| 0x04 | ERR_BADADDR | Invalid memory address |
| 0x05 | ERR_BADOP | Invalid opcode |
| 0x06 | ERR_STACKOF | Stack overflow |
| 0x07 | ERR_STACKUF | Stack underflow |
| 0x08 | ERR_NOSUB | Subroutine not found |
| 0x09 | ERR_NORET | Missing return statement |
| 0x0A | ERR_NOLNK | No linked building found |
| 0x0B | ERR_NOUNIT | No unit bound |

#

# Encoding Notes

## Handling Negative Values

Negative values in immediate operands are handled by sign extension within the 32-bit immediate field (F_RI format). For register operations, the sign is preserved in the register value itself (native mindustry - IEEE 754 Double).

## Mindustry World Object References

Building and unit references in F_RW format are stored as numeric IDs. Use `LINK` to get building references and `UBND` to get unit references which are then stored in registers for subsequent operations.

#

###### Project Test team.
