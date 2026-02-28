# Nand to Tetris — Projects 1–4
> Boolean Logic · Boolean Arithmetic · Sequential Logic · Machine Language

---

## Overview

| Project | Title | Goal |
|---------|-------|------|
| 1 | Boolean Logic | Build 15 logic gates from only NAND |
| 2 | Boolean Arithmetic | Build adders and the Hack ALU |
| 3 | Sequential Logic | Build memory chips and the Program Counter |
| 4 | Machine Language | Write Hack assembly programs directly |

**The one rule:** Everything traces back to a single primitive — the NAND gate.

### HDL Syntax Basics

```hdl
CHIP ChipName {
    IN  in1, in2, bus[16];    // input pins
    OUT out1, bus[16];        // output pins

    PARTS:
    SubChip(in=in1, out=wire1);
    SubChip(in=wire1, out=out1);
}
```

**Bus slicing:**
- `a[0]` — bit 0
- `a[0..7]` — bits 0 through 7
- `a[0..15]` — full 16-bit bus
- `true` / `false` — constant 1 / 0

---

## Project 1 — Boolean Logic

### The NAND Gate (only built-in primitive)

```hdl
CHIP Nand {
  IN  a, b;
  OUT out;
  BUILTIN Nand;
}
```

| a | b | out |
|---|---|-----|
| 0 | 0 | 1   |
| 0 | 1 | 1   |
| 1 | 0 | 1   |
| 1 | 1 | 0   |

---

### Elementary Gates

#### Not
`in → out`

| in | out |
|----|-----|
| 0  | 1   |
| 1  | 0   |

```hdl
PARTS:
Nand(a=in, b=in, out=out);
```

---

#### And
`a, b → out`

| a | b | out |
|---|---|-----|
| 0 | 0 | 0   |
| 0 | 1 | 0   |
| 1 | 0 | 0   |
| 1 | 1 | 1   |

```hdl
PARTS:
Nand(a=a, b=b, out=nandOut);
Not(in=nandOut, out=out);
```

---

#### Or
`a, b → out`

| a | b | out |
|---|---|-----|
| 0 | 0 | 0   |
| 0 | 1 | 1   |
| 1 | 0 | 1   |
| 1 | 1 | 1   |

```hdl
PARTS:
Not(in=a, out=notA);
Not(in=b, out=notB);
Nand(a=notA, b=notB, out=out);  // De Morgan: !((!a)&(!b)) = a|b
```

---

#### Xor
`a, b → out`

| a | b | out |
|---|---|-----|
| 0 | 0 | 0   |
| 0 | 1 | 1   |
| 1 | 0 | 1   |
| 1 | 1 | 0   |

```hdl
PARTS:
Nand(a=a,  b=b,  out=n1);
Nand(a=a,  b=n1, out=n2);
Nand(a=n1, b=b,  out=n3);
Nand(a=n2, b=n3, out=out);
```

---

### Multiplexers & Demultiplexers

#### Mux — 2-way 1-bit selector
`a, b, sel → out`

> If sel=0 → out=a. If sel=1 → out=b.

| a | b | sel | out |
|---|---|-----|-----|
| 0 | 0 | 0   | 0   |
| 1 | 0 | 0   | 1   |
| 0 | 1 | 1   | 1   |
| 1 | 1 | 1   | 1   |

```hdl
PARTS:
// out = (a AND !sel) OR (b AND sel)
Not(in=sel,  out=nSel);
And(a=a,    b=nSel, out=w1);
And(a=b,    b=sel,  out=w2);
Or(a=w1,   b=w2,   out=out);
```

---

#### DMux — 1-to-2 router
`in, sel → a, b`

> If sel=0 → {a=in, b=0}. If sel=1 → {a=0, b=in}.

| in | sel | a | b |
|----|-----|---|---|
| 0  | 0   | 0 | 0 |
| 1  | 0   | 1 | 0 |
| 0  | 1   | 0 | 0 |
| 1  | 1   | 0 | 1 |

```hdl
PARTS:
Not(in=sel, out=nSel);
And(a=in,  b=nSel, out=a);
And(a=in,  b=sel,  out=b);
```

---

### 16-bit & Multi-way Chips

#### Not16 / And16 / Or16
Same logic as 1-bit versions, applied independently to all 16 bits.

```hdl
// Not16 example
PARTS:
Not(in=in[0],  out=out[0]);
Not(in=in[1],  out=out[1]);
// ... repeat for bits 2–14
Not(in=in[15], out=out[15]);
```

---

#### Mux16
Selects between two 16-bit buses using a single sel bit.

```hdl
PARTS:
Mux(a=a[0], b=b[0], sel=sel, out=out[0]);
Mux(a=a[1], b=b[1], sel=sel, out=out[1]);
// ... bits 2–15
```

---

#### Or8Way
Output is 1 if ANY of the 8 input bits is 1.

```hdl
PARTS:
Or(a=in[0], b=in[1], out=w1);
Or(a=w1,   b=in[2], out=w2);
Or(a=w2,   b=in[3], out=w3);
// ... chain through in[7]
Or(a=w6,   b=in[7], out=out);
```

---

#### Mux4Way16
Selects 1 of 4 buses using 2-bit sel.

```hdl
PARTS:
Mux16(a=a, b=b, sel=sel[0], out=w1);
Mux16(a=c, b=d, sel=sel[0], out=w2);
Mux16(a=w1, b=w2, sel=sel[1], out=out);
```

---

#### DMux4Way
Routes input to 1 of 4 outputs using 2-bit sel.

```hdl
PARTS:
DMux(in=in, sel=sel[1], a=w1, b=w2);
DMux(in=w1, sel=sel[0], a=a,  b=b);
DMux(in=w2, sel=sel[0], a=c,  b=d);
```

---

### Project 1 — Full Chip List

| Chip | Signature | Description |
|------|-----------|-------------|
| Not | in → out | Bitwise NOT |
| And | a, b → out | Bitwise AND |
| Or | a, b → out | Bitwise OR |
| Xor | a, b → out | Bitwise XOR |
| Mux | a, b, sel → out | 2-way selector |
| DMux | in, sel → a, b | 1-to-2 router |
| Not16 | in[16] → out[16] | 16-bit NOT |
| And16 | a[16], b[16] → out[16] | 16-bit AND |
| Or16 | a[16], b[16] → out[16] | 16-bit OR |
| Mux16 | a[16], b[16], sel → out[16] | 16-bit 2-way selector |
| Or8Way | in[8] → out | 8-input OR reduction |
| Mux4Way16 | a/b/c/d[16], sel[2] → out[16] | 4-way 16-bit selector |
| Mux8Way16 | a..h[16], sel[3] → out[16] | 8-way 16-bit selector |
| DMux4Way | in, sel[2] → a/b/c/d | 1-to-4 router |
| DMux8Way | in, sel[3] → a..h | 1-to-8 router |

---

## Project 2 — Boolean Arithmetic

### Two's Complement

Hack uses 16-bit two's complement to represent −32768 to 32767.

**To negate a number:**
1. Flip all bits (NOT)
2. Add 1

```
Example: negate 3
  0000000000000011   (3)
  1111111111111100   (NOT)
+ 0000000000000001   (+1)
= 1111111111111101   (-3) 
```

| Binary | Decimal |
|--------|---------|
| 0000000000000000 | 0 |
| 0000000000000001 | 1 |
| 0111111111111111 | 32767 (max positive) |
| 1000000000000000 | −32768 (most negative) |
| 1111111111111111 | −1 |

---

### Adder Chips

#### HalfAdder
`a, b → sum, carry`
Adds 2 single bits. No carry-in.

| a | b | sum | carry |
|---|---|-----|-------|
| 0 | 0 | 0   | 0     |
| 0 | 1 | 1   | 0     |
| 1 | 0 | 1   | 0     |
| 1 | 1 | 0   | 1     |

```hdl
PARTS:
Xor(a=a, b=b, out=sum);
And(a=a, b=b, out=carry);
```

---

#### FullAdder
`a, b, c → sum, carry`
Adds 3 bits (a + b + carry-in).

| a | b | c | sum | carry |
|---|---|---|-----|-------|
| 0 | 0 | 0 | 0   | 0     |
| 0 | 0 | 1 | 1   | 0     |
| 0 | 1 | 1 | 0   | 1     |
| 1 | 1 | 0 | 0   | 1     |
| 1 | 1 | 1 | 1   | 1     |

```hdl
PARTS:
HalfAdder(a=a,  b=b, sum=s1, carry=c1);
HalfAdder(a=s1, b=c, sum=sum, carry=c2);
Or(a=c1, b=c2, out=carry);
```

---

#### Add16
`a[16], b[16] → out[16]`
Adds two 16-bit numbers. Overflow is ignored.

```hdl
PARTS:
HalfAdder(a=a[0], b=b[0], sum=out[0], carry=c0);
FullAdder(a=a[1], b=b[1], c=c0, sum=out[1], carry=c1);
FullAdder(a=a[2], b=b[2], c=c1, sum=out[2], carry=c2);
// ... carry ripples through bits 3–14
FullAdder(a=a[15], b=b[15], c=c14, sum=out[15], carry=);
```

---

#### Inc16
`in[16] → out[16]`
Adds 1 to a 16-bit number. Used in the Program Counter.

```hdl
PARTS:
Add16(a=in, b=false, out=out);
// Trick: wire b[0]=true, b[1..15]=false
```

---

### The Hack ALU

The ALU takes two 16-bit inputs (x, y) and 6 control bits, and produces a 16-bit output plus two status flags.

**Inputs:** `x[16]`, `y[16]`, `zx`, `nx`, `zy`, `ny`, `f`, `no`  
**Outputs:** `out[16]`, `zr` (out==0), `ng` (out<0)

#### Control Bits

| Bit | Meaning | Effect |
|-----|---------|--------|
| zx | zero x | if zx=1, set x=0 |
| nx | negate x | if nx=1, set x=!x |
| zy | zero y | if zy=1, set y=0 |
| ny | negate y | if ny=1, set y=!y |
| f | function | if f=1: x+y; if f=0: x&y |
| no | negate output | if no=1, set out=!out |

#### ALU Function Table

| zx | nx | zy | ny | f | no | out |
|----|----|----|----|---|----|-----|
| 1  | 0  | 1  | 0  | 1 | 0  | 0   |
| 1  | 1  | 1  | 1  | 1 | 1  | 1   |
| 1  | 1  | 1  | 0  | 1 | 0  | -1  |
| 0  | 0  | 1  | 1  | 0 | 0  | x   |
| 1  | 1  | 0  | 0  | 0 | 0  | y   |
| 0  | 0  | 1  | 1  | 0 | 1  | !x  |
| 1  | 1  | 0  | 0  | 0 | 1  | !y  |
| 0  | 0  | 1  | 1  | 1 | 1  | -x  |
| 1  | 1  | 0  | 0  | 1 | 1  | -y  |
| 0  | 1  | 1  | 1  | 1 | 1  | x+1 |
| 1  | 1  | 0  | 1  | 1 | 1  | y+1 |
| 0  | 0  | 1  | 1  | 1 | 0  | x-1 |
| 1  | 1  | 0  | 0  | 1 | 0  | y-1 |
| 0  | 0  | 0  | 0  | 1 | 0  | x+y |
| 0  | 1  | 0  | 0  | 1 | 1  | x-y |
| 0  | 0  | 0  | 1  | 1 | 1  | y-x |
| 0  | 0  | 0  | 0  | 0 | 0  | x&y |
| 0  | 1  | 0  | 1  | 0 | 1  | x\|y |

#### ALU Implementation

```hdl
PARTS:
// Step 1: handle zx/nx on x
Mux16(a=x,    b=false,   sel=zx, out=x1);
Not16(in=x1,  out=notX1);
Mux16(a=x1,   b=notX1,   sel=nx, out=x2);

// Step 2: handle zy/ny on y
Mux16(a=y,    b=false,   sel=zy, out=y1);
Not16(in=y1,  out=notY1);
Mux16(a=y1,   b=notY1,   sel=ny, out=y2);

// Step 3: f selects add or and
Add16(a=x2,   b=y2,      out=added);
And16(a=x2,   b=y2,      out=anded);
Mux16(a=anded, b=added,  sel=f,  out=fOut);

// Step 4: no negates output
Not16(in=fOut,  out=notFOut);
Mux16(a=fOut,   b=notFOut, sel=no, out=out, ...);

// Step 5: compute zr (out==0) and ng (out[15]==1)
```

---

## Project 3 — Sequential Logic

### The DFF — D Flip-Flop (built-in primitive)

```
out(t) = in(t-1)
// Output at time t equals input from the previous clock tick
```

This is the **only** place where time exists in the hardware. All memory chips are built on top of DFF.

| time | in | out |
|------|----|-----|
| 0    | 0  | ?   |
| 1    | 1  | 0   |
| 2    | 0  | 1   |
| 3    | 1  | 0   |
| 4    | 1  | 1   |

---

### Register Chips

#### Bit — 1-bit register
`in, load → out`

> If load=1: stores `in` on next tick. If load=0: holds current value.

| in | load | out(t+1) |
|----|------|----------|
| -  | 0    | no change |
| d  | 1    | d (loaded) |

```hdl
PARTS:
// Mux decides: keep old value or load new
Mux(a=dffOut, b=in, sel=load, out=muxOut);
DFF(in=muxOut, out=dffOut, out=out);
```

---

#### Register — 16-bit
`in[16], load → out[16]`

16 Bit chips in parallel. Same load signal shared across all bits.

```hdl
PARTS:
Bit(in=in[0],  load=load, out=out[0]);
Bit(in=in[1],  load=load, out=out[1]);
// ... repeat for bits 2–14
Bit(in=in[15], load=load, out=out[15]);
```

---

### RAM Chips

Each level uses 8 of the previous size, a DMux8Way to route the load signal, and a Mux8Way16 to select the output.

```
Register  ×8 →  RAM8  ×8 →  RAM64  ×8 →  RAM512  ×8 →  RAM4K  ×4 →  RAM16K
(1 word)       (8w)        (64w)          (512w)         (4Kw)         (16Kw)
```

| Chip | Address bits | Description |
|------|-------------|-------------|
| RAM8 | 3 | 8 × Register |
| RAM64 | 6 | 8 × RAM8 |
| RAM512 | 9 | 8 × RAM64 |
| RAM4K | 12 | 8 × RAM512 |
| RAM16K | 14 | 4 × RAM4K (used in CPU) |

#### RAM8 — template for all RAM chips

```hdl
PARTS:
// Route load to correct register
DMux8Way(in=load, sel=address,
         a=l0, b=l1, c=l2, d=l3,
         e=l4, f=l5, g=l6, h=l7);

// 8 registers, all receive in
Register(in=in, load=l0, out=r0);
Register(in=in, load=l1, out=r1);
// ... r2 through r7
Register(in=in, load=l7, out=r7);

// Select correct output
Mux8Way16(a=r0, b=r1, c=r2, d=r3,
          e=r4, f=r5, g=r6, h=r7,
          sel=address, out=out);
```

#### RAM64 — address slicing pattern

```hdl
// 6-bit address: top 3 bits = which RAM8, bottom 3 = which register inside
PARTS:
DMux8Way(in=load, sel=address[3..5], a=l0, ... h=l7);

RAM8(in=in, load=l0, address=address[0..2], out=r0);
RAM8(in=in, load=l1, address=address[0..2], out=r1);
// ... r2 through r7

Mux8Way16(a=r0, ..., h=r7, sel=address[3..5], out=out);
```

> **Key insight:** Each RAM level splits the address in two — top bits select the sub-chip, bottom bits go inside it. This pattern repeats all the way to RAM16K.

---

### Program Counter (PC)

`in[16], load, inc, reset → out[16]`

Tracks the current instruction address. Three control inputs determine behaviour each clock tick.

| Control | Effect | Use case |
|---------|--------|----------|
| inc=1 | out(t+1) = out(t) + 1 | Advance to next instruction |
| load=1 | out(t+1) = in | Jump to address in `in` |
| reset=1 | out(t+1) = 0 | Restart from beginning |

**Priority:** reset > load > inc

```hdl
PARTS:
// Increment current value
Inc16(in=regOut, out=incOut);

// Priority mux chain
Mux16(a=regOut, b=incOut,  sel=inc,   out=w1);
Mux16(a=w1,     b=in,      sel=load,  out=w2);
Mux16(a=w2,     b=false,   sel=reset, out=w3);

// Store result
Register(in=w3, load=true, out=regOut, out=out);
```

---

### Project 3 — Full Chip List

| Chip | Description |
|------|-------------|
| Bit | 1-bit register using DFF + Mux |
| Register | 16-bit register (16 × Bit) |
| RAM8 | 8 registers, 3-bit address |
| RAM64 | 64 registers, 6-bit address |
| RAM512 | 512 registers, 9-bit address |
| RAM4K | 4K registers, 12-bit address |
| RAM16K | 16K registers, 14-bit address |
| PC | Program Counter: inc / load / reset |

---

## Project 4 — Machine Language

### Overview

Project 4 is the first time you write software instead of hardware. You program the Hack computer directly in its native assembly language, getting hands-on with the machine you've been building from the ground up.

**Two programs to write:**
| Program | File | Goal |
|---------|------|------|
| Mult | `Mult.asm` | Multiply R0 × R1, store result in R2 |
| Fill | `Fill.asm` | Fill screen black on keypress, clear on release |

---

### The Hack Architecture

The Hack computer has two separate memory spaces and two registers.

#### Registers

| Register | Description |
|----------|-------------|
| `D` | Data register — general-purpose 16-bit value storage |
| `A` | Address register — doubles as a value register and memory pointer |
| `M` | Not a real register; shorthand for `RAM[A]` (memory at address A) |

#### Memory Map

| Address Range | Name | Purpose |
|---------------|------|---------|
| 0–15 | Virtual registers | `R0`–`R15` (predefined symbols) |
| 16–255 | Static variables | Compiler-allocated statics |
| 256–2047 | Stack | Call stack |
| 2048–16383 | Heap | Dynamic memory |
| 16384–24575 | Screen | Memory-mapped display (256×512 pixels) |
| 24576 | Keyboard | Memory-mapped keyboard input |

**Predefined symbols:**

| Symbol | Address |
|--------|---------|
| `R0`–`R15` | 0–15 |
| `SP` | 0 |
| `LCL` | 1 |
| `ARG` | 2 |
| `THIS` | 3 |
| `THAT` | 4 |
| `SCREEN` | 16384 |
| `KBD` | 24576 |

---

### Instruction Set

Hack has exactly two instruction types.

#### A-Instruction — Load a value into A
`@value`

Sets the A register to a constant, a variable name, or a label address.

```asm
@42       // A = 42
@R0       // A = 0  (predefined symbol)
@SCREEN   // A = 16384
@myVar    // A = address allocated to symbol "myVar"
@LOOP     // A = address of label (LOOP)
```

**Encoding:** `0vvvvvvvvvvvvvvv` — bit 15 is 0, bits 14–0 hold the 15-bit value.

---

#### C-Instruction — Compute, store, and/or jump
`dest = comp ; jump`

All three parts are optional, but at least one of `dest` or `jump` must be present.

```asm
D=D+A       // compute D+A, store in D
D=M         // load RAM[A] into D
M=D         // store D into RAM[A]
0;JMP       // unconditional jump
D;JGT       // jump if D > 0
AMD=D+1     // store D+1 into A, M, and D
```

**Encoding:** `111accccccdddjjj`

| Field | Bits | Description |
|-------|------|-------------|
| `111` | 15–13 | C-instruction marker |
| `a` | 12 | Use A (a=0) or M (a=1) in comp |
| `cccccc` | 11–6 | ALU computation |
| `ddd` | 5–3 | Destination (A, D, M) |
| `jjj` | 2–0 | Jump condition |

##### comp field (a=0 / a=1)

| comp | a=0 | a=1 |
|------|-----|-----|
| `0`   | 101010 | — |
| `1`   | 111111 | — |
| `-1`  | 111010 | — |
| `D`   | 001100 | — |
| `A`   | 110000 | `M` |
| `!D`  | 001101 | — |
| `!A`  | 110001 | `!M` |
| `-D`  | 001111 | — |
| `-A`  | 110011 | `-M` |
| `D+1` | 011111 | — |
| `A+1` | 110111 | `M+1` |
| `D-1` | 001110 | — |
| `A-1` | 110010 | `M-1` |
| `D+A` | 000010 | `D+M` |
| `D-A` | 010011 | `D-M` |
| `A-D` | 000111 | `M-D` |
| `D&A` | 000000 | `D&M` |
| `D\|A` | 010101 | `D\|M` |

##### dest field

| dest | bits | Stored in |
|------|------|-----------|
| null | 000  | (nowhere) |
| M    | 001  | RAM[A] |
| D    | 010  | D register |
| MD   | 011  | RAM[A] and D |
| A    | 100  | A register |
| AM   | 101  | A and RAM[A] |
| AD   | 110  | A and D |
| AMD  | 111  | A, RAM[A], and D |

##### jump field

| jump | bits | Condition |
|------|------|-----------|
| null | 000  | No jump |
| JGT  | 001  | out > 0 |
| JEQ  | 010  | out = 0 |
| JGE  | 011  | out ≥ 0 |
| JLT  | 100  | out < 0 |
| JNE  | 101  | out ≠ 0 |
| JLE  | 110  | out ≤ 0 |
| JMP  | 111  | Always |

---

### Symbols & Labels

#### Label declarations
```asm
(LOOP)      // declares a label; not an instruction
(END)
```
Labels map to the ROM address of the **next** instruction after the declaration.

#### Variable symbols
Any symbol used with `@` that isn't predefined or a label is automatically allocated a RAM address starting at 16.

```asm
@counter    // first use: allocates RAM[16], then A = 16
@sum        // next new symbol: RAM[17]
```

---

### Assembly Patterns

#### if (condition) goto LABEL
```asm
// if D == 0 goto ZERO
@ZERO
D;JEQ
```

#### Unconditional goto
```asm
@LOOP
0;JMP
```

#### while loop
```asm
(LOOP)
  // ... loop body ...
  @LOOP
  0;JMP       // always jump back
(END)
```

#### Read / write memory
```asm
// D = RAM[17]
@17
D=M

// RAM[17] = D
@17
M=D
```

#### Read keyboard input
```asm
@KBD        // A = 24576
D=M         // D = current key code (0 if no key pressed)
```

#### Write to screen (set one word = 16 pixels)
```asm
@SCREEN
M=-1        // fill 16 pixels black (all bits = 1)

@SCREEN
M=0         // clear 16 pixels
```

---

### Mult.asm — R2 = R0 × R1

Multiplication via repeated addition (R0 added R1 times).

```asm
// R2 = R0 * R1
// Uses: R0 (multiplicand), R1 (loop counter), R2 (accumulator)

    @R2
    M=0         // R2 = 0

(LOOP)
    @R1
    D=M         // D = R1
    @END
    D;JEQ       // if R1 == 0, done

    @R0
    D=M         // D = R0
    @R2
    M=D+M       // R2 += R0

    @R1
    M=M-1       // R1--

    @LOOP
    0;JMP

(END)
    @END
    0;JMP       // infinite loop to halt
```

> **Note:** Hack has no HALT instruction. Programs end by looping on themselves.

---

### Fill.asm — Screen I/O

Reads the keyboard register each cycle. Fills the screen black if a key is pressed; clears it otherwise.

```asm
// Continuously poll keyboard; fill or clear screen

(MAINLOOP)
    @KBD
    D=M             // D = key code

    @FILL
    D;JNE           // if key pressed, go fill

    // else: clear screen
    @color
    M=0
    @DRAW
    0;JMP

(FILL)
    @color
    M=-1            // -1 = 1111111111111111 (all black)

(DRAW)
    // Draw all 8192 words of the screen (256 rows × 32 words/row)
    @SCREEN
    D=A
    @addr
    M=D             // addr = SCREEN base

    @8192
    D=A
    @n
    M=D             // n = 8192

(DRAWLOOP)
    @n
    D=M
    @MAINLOOP
    D;JEQ           // if n == 0, loop back to poll keyboard

    @color
    D=M
    @addr
    A=M
    M=D             // RAM[addr] = color

    @addr
    M=M+1           // addr++

    @n
    M=M-1           // n--

    @DRAWLOOP
    0;JMP
```

> **Key insight:** The screen is 256 rows × 512 pixels. Each memory word covers 16 pixels, so the screen is 256 × 32 = **8192 words** starting at address 16384 (`SCREEN`).

---

### Project 4 — Key Concepts Summary

| Concept | Detail |
|---------|--------|
| Two instruction types | A-instruction (`@`) and C-instruction (`dest=comp;jump`) |
| No multiplication opcode | Must implement as repeated addition |
| No halt instruction | End programs with an infinite loop |
| Screen is memory-mapped | 8192 words at `SCREEN` (16384); each word = 16 pixels |
| Keyboard is memory-mapped | Single word at `KBD` (24576); 0 = no key |
| Variables auto-allocated | New symbols assigned RAM addresses from 16 upward |
| Labels don't use memory | `(LABEL)` declarations consume no ROM space |
