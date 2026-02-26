# Nand to Tetris — Projects 1–3
> Boolean Logic · Boolean Arithmetic · Sequential Logic

---

## Overview

| Project | Title | Goal |
|---------|-------|------|
| 1 | Boolean Logic | Build 15 logic gates from only NAND |
| 2 | Boolean Arithmetic | Build adders and the Hack ALU |
| 3 | Sequential Logic | Build memory chips and the Program Counter |

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
