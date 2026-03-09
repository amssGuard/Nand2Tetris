# Nand to Tetris — Projects 1–6
> Boolean Logic · Boolean Arithmetic · Sequential Logic · Machine Language · Computer Architecture · Assembler

My solutions and notes for the first half of the [Nand to Tetris](https://www.nand2tetris.org/) course — building a working computer from scratch, starting from a single NAND gate and ending with an assembler that generates binary machine code.

---

## What I Built

| Project | Title | What I did |
|---------|-------|------------|
| 1 | Boolean Logic | Built 15 logic gates from only NAND |
| 2 | Boolean Arithmetic | Built adders and the Hack ALU |
| 3 | Sequential Logic | Built memory chips and the Program Counter |
| 4 | Machine Language | Wrote Hack assembly programs directly |
| 5 | Computer Architecture | Wired together the Memory, CPU, and full Hack computer |
| 6 | Assembler | Wrote an assembler to translate `.asm` files into binary `.hack` files |

**The one rule I had to follow:** everything traces back to a single primitive — the NAND gate.

---

### HDL Syntax

Throughout projects 1–5 I wrote chips in HDL (Hardware Description Language). The structure is: a chip name, input/output pin declarations, and a PARTS section where I wired sub-chips together using named internal wires.

**Bus slicing I used:**
- `a[0]` — bit 0
- `a[0..7]` — bits 0 through 7
- `a[0..15]` — full 16-bit bus
- `true` / `false` — constant 1 / 0

---

## Project 1 — Boolean Logic

The goal was to build every gate I'd need later using only NAND. No other primitive is allowed.

### The NAND Gate (the only built-in)

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

**How I built it:** Feed `in` into both inputs of a NAND. A NAND with identical inputs always inverts — that's NOT for free.

---

#### And
`a, b → out`

| a | b | out |
|---|---|-----|
| 0 | 0 | 0   |
| 0 | 1 | 0   |
| 1 | 0 | 0   |
| 1 | 1 | 1   |

**How I built it:** NAND(a, b) gives NOT-AND. Slap a NOT on the output and I've got AND.

---

#### Or
`a, b → out`

| a | b | out |
|---|---|-----|
| 0 | 0 | 0   |
| 0 | 1 | 1   |
| 1 | 0 | 1   |
| 1 | 1 | 1   |

**How I built it:** De Morgan's law: `a OR b = NOT(NOT(a) AND NOT(b))`. Negate both inputs then NAND them — that's OR using only NAND.

---

#### Xor
`a, b → out`

| a | b | out |
|---|---|-----|
| 0 | 0 | 0   |
| 0 | 1 | 1   |
| 1 | 0 | 1   |
| 1 | 1 | 0   |

**How I built it:** NAND(a, b) → n1. Then NAND(a, n1) → n2 and NAND(n1, b) → n3. Finally NAND(n2, n3) → out. XOR from four NANDs.

---

### Multiplexers & Demultiplexers

#### Mux — 2-way 1-bit selector
`a, b, sel → out`

> sel=0 → passes a through. sel=1 → passes b through.

| a | b | sel | out |
|---|---|-----|-----|
| 0 | 0 | 0   | 0   |
| 1 | 0 | 0   | 1   |
| 0 | 1 | 1   | 1   |
| 1 | 1 | 1   | 1   |

**How I built it:** `out = (a AND NOT sel) OR (b AND sel)`. Two AND gates (one on each input with the appropriate sel polarity), then OR them together.

---

#### DMux — 1-to-2 router
`in, sel → a, b`

> sel=0 → routes in to a. sel=1 → routes in to b.

| in | sel | a | b |
|----|-----|---|---|
| 0  | 0   | 0 | 0 |
| 1  | 0   | 1 | 0 |
| 0  | 1   | 0 | 0 |
| 1  | 1   | 0 | 1 |

**How I built it:** `a = in AND NOT(sel)`, `b = in AND sel`. Straightforward once I saw the Mux pattern.

---

### 16-bit & Multi-way Chips

#### Not16 / And16 / Or16

**How I built them:** Apply the 1-bit version in parallel across all 16 bit positions. Repetitive to write, but it clicked why busses matter.

#### Mux16

**How I built it:** 16 independent 1-bit Muxes sharing a single sel line.

#### Or8Way

**How I built it:** Chain 7 Or gates — Or(bit0, bit1), then Or(that, bit2), all the way to bit7. A reduction tree.

#### Mux4Way16

**How I built it:** Two Mux16s pick between the first pair (a/b) and second pair (c/d) using sel[0], then a third Mux16 picks between those results using sel[1].

#### DMux4Way

**How I built it:** A DMux on sel[1] splits the signal first. Then a DMux on each branch using sel[0] produces the four final outputs.

---

### Project 1 — Chips I Built

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

Here I built the arithmetic layer — adders up to 16-bit, and the full Hack ALU.

### Two's Complement

The Hack computer represents negative numbers in 16-bit two's complement, covering −32768 to 32767.

**To negate a number:** flip all bits, then add 1.

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

| a | b | sum | carry |
|---|---|-----|-------|
| 0 | 0 | 0   | 0     |
| 0 | 1 | 1   | 0     |
| 1 | 0 | 1   | 0     |
| 1 | 1 | 0   | 1     |

**How I built it:** Sum is XOR(a, b). Carry is AND(a, b). Carry only fires when both inputs are 1.

---

#### FullAdder
`a, b, c → sum, carry`

| a | b | c | sum | carry |
|---|---|---|-----|-------|
| 0 | 0 | 0 | 0   | 0     |
| 0 | 0 | 1 | 1   | 0     |
| 0 | 1 | 1 | 0   | 1     |
| 1 | 1 | 0 | 0   | 1     |
| 1 | 1 | 1 | 1   | 1     |

**How I built it:** Chain two HalfAdders. First takes a and b → intermediate sum s1 and carry c1. Second takes s1 and carry-in c → final sum and carry c2. Final carry = OR(c1, c2).

---

#### Add16
`a[16], b[16] → out[16]`

**How I built it:** HalfAdder for bit 0 (no carry-in), then FullAdders for bits 1–15 in a ripple chain. Carry-out from bit 15 is discarded — overflow is ignored by design.

---

#### Inc16
`in[16] → out[16]`

**How I built it:** Add16 with the second input hardwired to 1 (bit 0 = true, all others = false).

---

### The Hack ALU

This was the most satisfying chip to build. Six control bits let it compute 18 different functions.

**Inputs:** `x[16]`, `y[16]`, `zx`, `nx`, `zy`, `ny`, `f`, `no`  
**Outputs:** `out[16]`, `zr` (out==0), `ng` (out<0)

#### Control Bits

| Bit | Effect |
|-----|--------|
| zx | if 1, force x = 0 |
| nx | if 1, negate x |
| zy | if 1, force y = 0 |
| ny | if 1, negate y |
| f  | if 1: compute x+y; if 0: compute x AND y |
| no | if 1, negate the output |

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

#### How I built the ALU

1. **Pre-process x:** If zx=1, zero it out. If nx=1, bitwise-negate it.
2. **Pre-process y:** Same — zero if zy=1, negate if ny=1.
3. **Compute:** If f=1, add x and y. If f=0, AND them.
4. **Post-process:** If no=1, negate the result.
5. **Status flags:** `zr` = OR all 16 output bits together, then NOT (1 if output is zero). `ng` = output bit 15 directly (1 if negative).

---

## Project 3 — Sequential Logic

Up to this point everything was combinational — no memory, no time. Project 3 introduced the clock and state.

### The DFF — D Flip-Flop (built-in primitive)

```
out(t) = in(t-1)
```

The DFF is the only chip that "remembers" — its output at time t is whatever its input was at t-1. Every memory chip I built in this project ultimately sits on top of DFFs.

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

> load=1: stores the input on the next tick. load=0: holds the current value.

| in | load | out(t+1) |
|----|------|----------|
| -  | 0    | no change |
| d  | 1    | d |

**How I built it:** A Mux selects between the DFF's current output (feedback, load=0) and the new input (load=1). The Mux output feeds into the DFF. The DFF output loops back to the Mux and also becomes the chip output.

---

#### Register — 16-bit
`in[16], load → out[16]`

**How I built it:** 16 Bit chips in parallel, all sharing the same load signal.

---

### RAM Chips

Each level is 8 instances of the previous, plus a DMux8Way to route the load signal and a Mux8Way16 to read back the right word.

```
Register  ×8 →  RAM8  ×8 →  RAM64  ×8 →  RAM512  ×8 →  RAM4K  ×4 →  RAM16K
(1 word)       (8w)        (64w)          (512w)         (4Kw)         (16Kw)
```

| Chip | Address bits | Size |
|------|-------------|------|
| RAM8 | 3 | 8 × Register |
| RAM64 | 6 | 8 × RAM8 |
| RAM512 | 9 | 8 × RAM64 |
| RAM4K | 12 | 8 × RAM512 |
| RAM16K | 14 | 4 × RAM4K |

#### How I built RAM8

1. DMux8Way routes the load signal to exactly one of 8 registers using the 3-bit address as selector.
2. All 8 Registers receive the same input bus; only the one with an active load will actually store anything.
3. Mux8Way16 selects the output of the addressed register using the same 3-bit address.

#### Address Slicing (RAM64 and up)

The same pattern repeats at every level: split the address in two. Upper bits select which sub-chip to activate; lower bits are passed down into that sub-chip as its own address. I found this recursive structure elegant once it clicked.

---

### Program Counter (PC)
`in[16], load, inc, reset → out[16]`

| Control | Effect |
|---------|--------|
| inc=1 | Advance to next instruction |
| load=1 | Jump to address in `in` |
| reset=1 | Restart from address 0 |

**Priority:** reset beats load beats inc.

**How I built it:**
1. Compute the incremented value with Inc16 on the register's current output.
2. Build a priority mux chain — start with the current value, layer in the incremented value (inc), then override with `in` (load), then override with 0 (reset). Later muxes win.
3. Feed the result into a Register with load permanently tied to true.

---

### Project 3 — Chips I Built

| Chip | Description |
|------|-------------|
| Bit | 1-bit register (DFF + Mux) |
| Register | 16-bit register (16 × Bit) |
| RAM8 | 8 registers, 3-bit address |
| RAM64 | 64 registers, 6-bit address |
| RAM512 | 512 registers, 9-bit address |
| RAM4K | 4K registers, 12-bit address |
| RAM16K | 16K registers, 14-bit address |
| PC | Program Counter: inc / load / reset |

---

## Project 4 — Machine Language

First project where I wrote software rather than hardware. I programmed the Hack computer directly in assembly — a good reality check on how low-level the machine actually is.

**Two programs I wrote:**

| Program | File | Goal |
|---------|------|------|
| Mult | `Mult.asm` | Multiply R0 × R1, store result in R2 |
| Fill | `Fill.asm` | Fill screen black on keypress, clear on release |

---

### The Hack Architecture

#### Registers

| Register | Description |
|----------|-------------|
| `D` | Data register — general-purpose 16-bit value |
| `A` | Address register — value store and memory pointer |
| `M` | Alias for `RAM[A]` — not a physical register |

#### Memory Map

| Address Range | Name | Purpose |
|---------------|------|---------|
| 0–15 | Virtual registers | `R0`–`R15` |
| 16–255 | Static variables | Compiler-allocated |
| 256–2047 | Stack | Call stack |
| 2048–16383 | Heap | Dynamic memory |
| 16384–24575 | Screen | Memory-mapped display (256×512 px) |
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

Two instruction types, nothing else.

#### A-Instruction — `@value`

Loads a constant, variable address, or label address into A.

**Encoding:** `0vvvvvvvvvvvvvvv` — bit 15 = 0, bits 14–0 = the 15-bit value.

---

#### C-Instruction — `dest = comp ; jump`

**Encoding:** `111accccccdddjjj`

| Field | Bits | Description |
|-------|------|-------------|
| `111` | 15–13 | C-instruction marker |
| `a` | 12 | Use A (0) or M (1) in comp |
| `cccccc` | 11–6 | ALU op |
| `ddd` | 5–3 | Destination |
| `jjj` | 2–0 | Jump condition |

##### comp field

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

`(LOOP)` declares a label pointing to the ROM address of the next instruction — no ROM space consumed.

Any `@symbol` that isn't predefined or a label gets automatically allocated a RAM address starting at 16, one per new symbol.

---

### Assembly Patterns I Used

**Conditional branch:** Load the target label into A, then `D;JEQ` (or whichever condition).

**Unconditional jump:** Load the target into A, then `0;JMP`.

**While loop:** Put the label before the body, unconditionally jump back at the end.

**Memory read/write:** Load the address into A, then `D=M` to read or `M=D` to write.

**Keyboard poll:** Load `KBD` into A, read M — 0 means no key is pressed.

**Screen write:** Load a screen address into A, write `M=-1` for black or `M=0` to clear.

---

### Mult.asm — My Approach

**Goal:** R2 = R0 × R1, using only the available instructions.

Since there's no multiply opcode I implemented it as repeated addition:

1. Initialise R2 = 0.
2. If R1 = 0, jump to END.
3. R2 += R0.
4. R1 -= 1.
5. Go to step 2.
6. END: infinite loop (Hack has no halt instruction).

---

### Fill.asm — My Approach

**Goal:** Poll the keyboard in a loop; fill the screen black when a key is held, clear it when released.

1. Read the keyboard register (24576) into D.
2. Set a `color` variable to −1 if a key is pressed, 0 if not.
3. Set an address pointer to the screen base (16384), counter to 8192.
4. Loop: write `color` to the current address, advance the pointer, decrement the counter.
5. When counter hits 0, go back to step 1.

> The screen is 256 rows × 512 pixels. Each memory word maps to 16 pixels, so the whole display is 256 × 32 = **8192 words**.

---

### Project 4 — Key Takeaways

| Concept | Detail |
|---------|--------|
| Two instruction types | A-instruction (`@`) and C-instruction (`dest=comp;jump`) |
| No multiply opcode | Implemented as repeated addition |
| No halt | Programs end by looping forever |
| Screen is memory-mapped | 8192 words at `SCREEN` (16384) |
| Keyboard is memory-mapped | Single word at `KBD` (24576); 0 = no key |
| Variables auto-allocated | From RAM[16] upward |
| Labels are free | `(LABEL)` takes no ROM space |

---

## Project 5 — Computer Architecture

This was the payoff project — wiring everything I'd built into a complete, working computer.

**Three chips I built:**

| Chip | Description |
|------|-------------|
| `Memory` | Unified data memory: RAM16K + Screen + Keyboard |
| `CPU` | Executes one Hack instruction per clock cycle |
| `Computer` | Top-level chip: ROM32K + CPU + Memory |

---

### The Hack Computer — Big Picture

```
          ┌─────────────────────────────────────────┐
          │              Computer                   │
          │                                         │
          │  ┌────────┐   instruction   ┌─────────┐ │
          │  │ ROM32K │ ─────────────►  │   CPU   │ │
          │  │ (ROM)  │ ◄─────────────  │         │ │
          │  └────────┘    pc (address) │         │ │
          │                             │         │ │
          │                  inM ──────►│         │ │
          │               writeM ◄──────│         │ │
          │               addressM◄─────│         │ │
          │                 outM ◄──────│         │ │
          │                             └─────────┘ │
          │                                 │  ▲    │
          │                         writeM  │  │inM │
          │                                 ▼  │    │
          │                          ┌──────────┐   │
          │                          │  Memory  │   │
          │                          │ RAM+Screen│  │
          │                          │ +Keyboard │  │
          │                          └──────────┘   │
          └─────────────────────────────────────────┘
```

---

### Memory Chip

`in[16], load, address[15] → out[16]`

#### Address Space

| Address Range | Device | Notes |
|---------------|--------|-------|
| 0–16383 | RAM16K | General-purpose data |
| 16384–24575 | Screen | Memory-mapped display |
| 24576 | Keyboard | Read-only |

> address[14]=0 → RAM · address[14]=1, address[13]=0 → Screen · address[14]=1, address[13]=1 → Keyboard

#### How I built it

1. DMux on `address[14]` routes the load signal — RAM if bit 14 is 0, Screen if 1. Keyboard never gets a load (read-only).
2. Instantiate RAM16K, Screen, and Keyboard with appropriate address slices.
3. Mux16 on `address[14]` picks between RAM and Screen output.
4. AND(address[14], address[13]) detects the keyboard; a second Mux16 overrides with keyboard output when true.

---

### CPU Chip

`inM[16], instruction[16], reset → outM[16], writeM, addressM[15], pc[15]`

#### Pins

| Pin | Direction | Description |
|-----|-----------|-------------|
| `inM[16]` | in | Value from RAM[A] |
| `instruction[16]` | in | Current instruction from ROM |
| `reset` | in | Restart from address 0 if 1 |
| `outM[16]` | out | Value to write to memory |
| `writeM` | out | High when a memory write should happen |
| `addressM[15]` | out | RAM address to read/write |
| `pc[15]` | out | Next instruction address (feeds ROM) |

#### Instruction Decoding

```
instruction[15] = 0  →  A-instruction
instruction[15] = 1  →  C-instruction
```

**C-instruction bit fields:**

| Bit(s) | Field | Role |
|--------|-------|------|
| 15 | opcode | 1 = C-instruction |
| 12 | a-bit | Use A (0) or M (1) |
| 11–6 | cccccc | ALU control |
| 5 | d1 | Write result to A |
| 4 | d2 | Write result to D |
| 3 | d3 | Write result to M |
| 2–0 | j1 j2 j3 | Jump condition |

#### How I built the CPU

1. **Decode:** Check bit 15 to distinguish A from C instructions.
2. **A register:** For an A-instruction, load the instruction value directly. For a C-instruction, load the ALU output only if d1 (bit 5) is set. A Mux16 selects between the two; OR of the two load conditions drives the register. A's output goes to both `addressM` and the ALU's y-input mux.
3. **D register:** Load ALU output only when it's a C-instruction AND d2 (bit 4) is set.
4. **ALU y-input:** Mux16 controlled by the a-bit (bit 12) selects between A and inM.
5. **ALU:** D is x, the mux output is y. Bits 11–6 wire directly to the six ALU control inputs. ALU produces `outM`, `zr`, and `ng`.
6. **writeM:** AND(C-instruction flag, d3).
7. **Jump logic:** Three conditions against the flags — j1 AND ng (negative), j2 AND zr (zero), j3 AND (NOT ng AND NOT zr) (positive). OR all three, AND with the C-instruction flag so A-instructions never jump.
8. **PC:** Jump condition met → load A. Otherwise increment. Reset overrides everything.

---

### Computer Chip

**How I built it:** Three instantiations and four wires. ROM32K feeds instructions into the CPU using the CPU's pc output as the address. The CPU drives Memory with outM, writeM, and addressM. Memory feeds inM back into the CPU. Reset passes straight through to the CPU.

Everything I built across projects 1–4 lives inside those three chips.

---

### Fetch-Execute Cycle

| Phase | What happens |
|-------|-------------|
| **Fetch** | ROM32K outputs the instruction at address pc |
| **Decode** | CPU reads bit 15; extracts comp/dest/jump fields |
| **Execute** | ALU computes; A, D, M updated per dest bits |
| **Advance PC** | Jump taken → PC ← A; else PC ← PC + 1 |

---

### Control Flow

| Scenario | Result |
|----------|--------|
| A-instruction (bit15=0) | Load value into A; PC++ |
| C-instruction, no jump | Compute; write to dest; PC++ |
| C-instruction, jump taken | Compute; write to dest; PC ← A |
| reset=1 | PC ← 0 |

---

### Project 5 — Chips I Built

| Chip | Inputs | Outputs | Description |
|------|--------|---------|-------------|
| Memory | in[16], load, address[15] | out[16] | Unified data memory |
| CPU | inM[16], instruction[16], reset | outM[16], writeM, addressM[15], pc[15] | Fetch/decode/execute |
| Computer | reset | — | Full Hack computer |

---

## Project 6 — Assembler

The first project written in a high-level language rather than HDL. I built an assembler that reads `.asm` files and outputs `.hack` binary — one 16-bit instruction per line.

**Input:** `Prog.asm`  
**Output:** `Prog.hack` — loadable directly into ROM32K

---

### What It Handles

| Input | Example | Output |
|-------|---------|--------|
| A-instruction (numeric) | `@42` | `0000000000101010` |
| A-instruction (symbol) | `@LOOP`, `@counter` | Look up / allocate address, then emit |
| C-instruction | `D=D+A;JGT` | Look up tables, emit `111...` |
| Label | `(LOOP)` | Record ROM address; emit nothing |
| Whitespace / comments | `// note` | Strip and ignore |

---

### Architecture

I split the assembler into four modules:

| Module | What it does |
|--------|-------------|
| **Parser** | Reads the file line by line; strips whitespace and comments; classifies and decomposes each instruction |
| **Code** | Translates comp, dest, and jump mnemonics into binary strings |
| **SymbolTable** | Hash map of symbol names to addresses; pre-loaded with predefined symbols |
| **Main** | Runs both passes; writes the output file |

---

### Symbol Table

Pre-loaded before the first pass:

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

Label symbols get added in Pass 1. Variable symbols get added in Pass 2 starting at RAM[16].

---

### Parser

**Instruction types:**

| Type | How to recognise | Example |
|------|-----------------|---------|
| `A_INSTRUCTION` | Starts with `@` | `@42`, `@counter` |
| `C_INSTRUCTION` | Contains `=` or `;` | `D=M`, `0;JMP` |
| `L_INSTRUCTION` | Wrapped in `( )` | `(LOOP)` |

**Per-line algorithm:**
1. Strip all whitespace; remove everything from `//` to end of line.
2. Skip if empty.
3. Starts with `@` → A_INSTRUCTION. Symbol = everything after `@`.
4. Starts with `(` → L_INSTRUCTION. Symbol = content between the parentheses.
5. Otherwise → C_INSTRUCTION. Extract dest (before `=`, or null), comp (between `=` and `;`), jump (after `;`, or null).

---

### Code Module

**dest → 3 bits**

| mnemonic | bits |
|----------|------|
| null | 000 |
| M | 001 |
| D | 010 |
| MD | 011 |
| A | 100 |
| AM | 101 |
| AD | 110 |
| AMD | 111 |

**jump → 3 bits**

| mnemonic | bits |
|----------|------|
| null | 000 |
| JGT | 001 |
| JEQ | 010 |
| JGE | 011 |
| JLT | 100 |
| JNE | 101 |
| JLE | 110 |
| JMP | 111 |

**comp → 7 bits (a-bit + 6 control bits)**

| comp | a | cccccc |
|------|---|--------|
| 0    | 0 | 101010 |
| 1    | 0 | 111111 |
| -1   | 0 | 111010 |
| D    | 0 | 001100 |
| A    | 0 | 110000 |
| !D   | 0 | 001101 |
| !A   | 0 | 110001 |
| -D   | 0 | 001111 |
| -A   | 0 | 110011 |
| D+1  | 0 | 011111 |
| A+1  | 0 | 110111 |
| D-1  | 0 | 001110 |
| A-1  | 0 | 110010 |
| D+A  | 0 | 000010 |
| D-A  | 0 | 010011 |
| A-D  | 0 | 000111 |
| D&A  | 0 | 000000 |
| D\|A | 0 | 010101 |
| M    | 1 | 110000 |
| !M   | 1 | 110001 |
| -M   | 1 | 110011 |
| M+1  | 1 | 110111 |
| M-1  | 1 | 110010 |
| D+M  | 1 | 000010 |
| D-M  | 1 | 010011 |
| M-D  | 1 | 000111 |
| D&M  | 1 | 000000 |
| D\|M | 1 | 010101 |

---

### Two-Pass Algorithm

A single pass isn't enough — a label can be referenced before it's declared (forward reference). Two passes solve this cleanly.

#### Pass 1 — Build the label table (no output written)

```
ROM counter = 0
For each line:
    Strip whitespace and comments; skip if empty
    L_INSTRUCTION → add symbol = ROM counter to table; do NOT increment counter
    A or C instruction → increment ROM counter
```

After Pass 1, every label points to the correct ROM address of its following instruction.

#### Pass 2 — Emit binary

```
RAM counter = 16
For each line:
    Strip whitespace and comments; skip if empty or L_INSTRUCTION

    A_INSTRUCTION:
        If numeric → value = integer
        If in symbol table → value = lookup
        Else → insert symbol at RAM counter; value = RAM counter; RAM counter++
        Emit: '0' + 15-bit binary of value

    C_INSTRUCTION:
        Extract dest, comp, jump
        Emit: '111' + Code(comp) + Code(dest) + Code(jump)
```

---

### Data Flow

```
  Prog.asm
     │
     ▼
┌─────────┐   Pass 1   ┌──────────────┐
│  Parser │ ─────────► │ Symbol Table │
│         │            │ (labels only)│
└─────────┘            └──────────────┘
     │
     ▼
┌─────────┐   Pass 2   ┌──────────────┐   ┌────────┐
│  Parser │ ─────────► │ Symbol Table │──►│  Code  │──► Prog.hack
│         │            │ + variables  │   │ module │
└─────────┘            └──────────────┘   └────────┘
```

---

### Project 6 — Key Takeaways

| Concept | Detail |
|---------|--------|
| Two passes required | Pass 1 resolves labels; Pass 2 generates binary |
| Forward references | Labels used before declaration — handled by Pass 1 |
| Variable allocation | Assigned from RAM[16] upward during Pass 2 |
| A-instruction output | `0` + 15-bit value |
| C-instruction output | `111` + 7-bit comp + 3-bit dest + 3-bit jump |
| L-instructions | Never emitted; only update the symbol table |
| Pre-loaded symbols | All predefined symbols inserted before Pass 1 |

---

## How It All Connects

```
Project 1  →  Gates (Not, And, Or, Mux, DMux, ...)
    ↓
Project 2  →  ALU  (HalfAdder, FullAdder, Add16, ALU)
    ↓
Project 3  →  Memory chips  (Bit, Register, RAM*, PC)
    ↓
Project 4  →  Assembly programs  (Mult.asm, Fill.asm)
    ↓
Project 5  →  Computer  (Memory + CPU + Computer chip)
    ↓
Project 6  →  Assembler  (.asm → .hack binary)
                    translates the programs from Project 4
                    into binary that runs on the computer
                    I built in Project 5
```

Every binary instruction that runs on this machine traces all the way back to NAND gates from Project 1.
