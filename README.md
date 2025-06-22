# chibicc-6800-v1: A Small C Compiler for MC6800

This project is a fork of [@rui314](https://www.sigbus.info/)'s [chibicc](https://github.com/rui314/chibicc/), modified to create a C compiler for the Motorola MC6800 architecture.

This project was created as a tool for studying compilers for the MC6800, and while it includes extra code and comments that may not be essential, they reflect the learning process and experimentation involved in developing the compiler.

There are many aspects of the object code generation method and speed that cannot be understood without actually creating it. There may be unnecessary parts added for testing code generation.

However, it is a compiler that works reasonably well. I hope it will be helpful for you to create another fork.

Dhrystone & Whetstone works now. 

- https://github.com/z88dk/z88dk/tree/master/libsrc/\_DEVELOPMENT/EXAMPLES/benchmarks/dhrystone21
- https://github.com/z88dk/z88dk/tree/master/libsrc/_DEVELOPMENT/EXAMPLES/benchmarks/whetstone
----
# Topics

- **Data types:** `int` and pointers are 16-bit; `long` and `float` are 32-bit. `double` and `long long` (64-bit or more) are unsupported.
- **Structs/unions:** Passing/returning by value and bit fields are implemented.
- **Function parameters:** Only the first parameter is passed via registers (A/B/@long). If the first parameter is a struct/union, all parameters are passed via the stack.
- **Return values:** Struct/union return values pass their address as an implicit first argument in a register. all other normal arguments are passed on the stack.

The compiler passes basic tests, but there are still many issues remaining.

IEEE 754 32-bit floating-point arithmetic code is written in assembler, which is faster and also smaller in size compared to code written in C

It can handle subnormal, NaN and Inf values. It passes basic testing but is not well tested for precision.

The original chibicc compiles C source code using stack-based operations, but also makes use of the many registers available on x64. This is inefficient on the MC6800, which has few registers and limited stack support. chibicc-6800-v1 optimizes code generation for the MC6800.

Fuzix-BinTools is required to assemble and link compiled objects.
For testing, we use emu6800 from Fuzix-Compiler-Kit.

- https://github.com/EtchedPixels/Fuzix-Bintools
- https://github.com/EtchedPixels/Fuzix-Compiler-Kit

---

# Installation

Follow the steps below to set up the environment for chibicc-6800.

## 1. Install Fuzix-Bintools

First, install [Fuzix-Bintools](https://github.com/EtchedPixels/Fuzix-Bintools).  
Please refer to the instructions in the Fuzix-Bintools repository's README.md for details.

Make sure that the installed binaries (e.g., `as6800`, `ld6800`) are available in your `$PATH`.

## 2. Install Fuzix-Compiler-Kit

Next, install [Fuzix-Compiler-Kit](https://github.com/EtchedPixels/Fuzix-Compiler-Kit).  
Please follow the installation instructions in the Fuzix-Compiler-Kit repository's README.md.

## 3. Install `emu6800` Emulator

After installing the Fuzix-Compiler-Kit, you need to install the `emu6800` emulator for testing.

Copy the emulator binary to the appropriate directory:

```sh
cp test/emu6800 /opt/fcc/bin/
```

> **Note:**  
> Make sure that `/opt/fcc/bin/` is included in your `$PATH` environment variable.

## 4. Verify Installation

You can verify that the tools are installed correctly by running:

```sh
which chibicc
which emu6800
which as6800
which ld6800
```

All commands should return the path to the respective binaries.

---

## Example: Running Test Programs

You can run all test programs automatically using the provided onetest script:

```sh
cd ztest
./onetest 9018-asciiartf.c
```

To run all tests, use runtest.

```sh
cd ztest
./runall
```

Alternatively, you can manually compile and run a test program as follows:

```sh
cd ztest
chibicc -v -O -o 9018-asciiartf.bin 9018-asciiartf.c
emu6800 6800 9018-asciiartf.bin 9018-asciiartf.map
```

This will compile the source file and execute the resulting binary using the emulator.

---
# Performance

## Integer Operations
- **Dhrystone benchmark:** 234 seconds at 1MHz on MC6800, equivalent to approximately 0.05 DMIPS.
- **Source code:** [`z88dk/libsrc/_DEVELOPMENT/EXAMPLES/benchmarks
/dhrystone21/`](https://github.com/z88dk/z88dk/tree/master/libsrc/_DEVELOPMENT/EXAMPLES/benchmarks/dhrystone21)

## Floating-Point Operations
- **Mandelbrot ASCII renderer (`asciiartf`):** 266 seconds  
- **Source code:** [`ztest/9018-asciiartf.c`](https://github.com/zu2/chibicc-6800-v1/blob/main/ztest/9018-asciiartf.c)  

- **Whetstone benchmark:** 449.5355 seconds at 1MHz on MC6800, equivalent to approximately 2.2245 KWIPS, .0022245 MWIPS
-- Improve: 2.7544 KWIPS, 0.027544 MWIPS (2025/06/12)
- **Source code:** [`z88dk/libsrc/_DEVELOPMENT/EXAMPLES/benchmarks/whetstone/`](https://github.com/z88dk/z88dk/tree/master/libsrc/_DEVELOPMENT/EXAMPLES/benchmarks/whetstone)

## String and Memory Operations
- `strcmp`, `strcpy`, and `strcat` operate on two bytes at a time, resulting in high performance.
- Other `str*` and `mem*` functions are also implemented to minimize the number of comparisons and branches, further improving efficiency.

## Function Calls and Branching
- Despite the slow function prologue and epilogue, overall performance remains high.
- For `char` and `int`, direct branching is generated without relying on subroutines.
- For `long`, optimized subroutines are used for comparison, providing relatively fast execution.
- Recursive functions such as Ackermann ([`9005-ack.c`](https://github.com/zu2/chibicc-6800-v1/blob/main/ztest/9005-ack.c)) and Takeuchi's tarai ([`9100-tarai.c`](https://github.com/zu2/chibicc-6800-v1/blob/main/ztest/9100-tarai.c)) run efficiently, even with the overhead of function call setup and teardown.

## Details of Integer Arithmetic

- The MC6800 cannot directly transfer between AccAB and IX registers, and IX is required for stack data operations.
- A naive stack machine implementation would be slow, so chibicc-6800-v1 uses the IX register for address calculations whenever possible.
- For global variables and local variables with fixed offsets, AccAB is avoided as much as possible.
- Supports 32-bit `long`; does not support 64-bit `long long`.

## Details of Floating-Point Arithmetic

- Supports IEEE754 32-bit single-precision `float`.
- Only round-to-nearest mode is supported.
- Handles subnormal numbers.
- Does not support `double` or `long double`.
- Currently implements addition, subtraction, multiplication, division, `fabsf`, `fsqrtf`, `floorf`, and `ceilf`. Other functions may be supported in the future.

The IEEE754 float implementation has passed the Paranoia test for addition, subtraction, multiplication, division, and sqrtf. Other parts are still in progress.

- https://github.com/z88dk/z88dk/tree/master/libsrc/_DEVELOPMENT/EXAMPLES/benchmarks/paranoia
---
# Optimization Options

## Supported Optimization Flags

- The compiler supports the `-O` and `-Os` options for optimization.  
- You can also specify `-O0`, `-O1`, and `-O2`, but note that `-O2` is not yet implemented.  
- The default optimization level is `-O0`.

## Behavior of Each Option

- **`-O0`**:  
  No optimization is performed at this level.

- **`-O`, `-O1`, `-O2`**:  
  if `/opt/fcc/lib/copt` is available, chibicc will use it to perform peephole optimization

- **`-Os`**:  
  This option generates code aimed at minimizing the size of the object file. As a trade-off, the resulting code may run approximately 5%-25% slower.

---
# Memo

## Stack frame

This compiler, like the x86 version, uses the frame pointer (`@bp`) to access local variables and arguments.  
On the 6800, due to the limited number of registers, `@bp` is placed in the zero page.  
Unlike other 6800 compilers that obtain the stack position using `TSX`, chibicc-6800 uses `LDX @bp`.  
Although this makes the instruction one byte larger, the number of cycles required is the same.  
Furthermore, when arithmetic operations on the stack pointer are needed, omitting `STS` can offset the size difference.


### Key points:

- **Frame Pointer Usage:** Required for supporting `alloca` and Variable-Length Arrays (VLA).
- **Performance Tradeoff:** Functions with a frame pointer are slightly slower due to saving/restoring it during prologue/epilogue.
- **Comparison with Other Compilers:** CC68 and Fuzix CC use the stack pointer (SP) directly, which is more efficient but requires precise SP tracking.
- **Future Plans:** Support for `alloca`/VLA may be removed in favor of SP-only implementation for simplicity.

Since `alloca` and VLAs use stack space and the size isn't known in advance, accessing locals and arguments via the stack pointer becomes tricky after allocation.  
The frame pointer (@bp) helps by providing a stable reference point.  
VLAs are accessed through a pointer saved on the stack.  
When the function returns, the old frame pointer is used to restore the stack properly.

```
// @bp points old @bp,argument
//
// SP  -> stack top
//        alloca/VLA area
// @bp -> local var top
//             :
//        local var end
//        argment passed by register AB or @long (if any)
//        old @bp
//        return address
//        argment passed by stack
```

## Function Arguments Handling

In this implementation:

1. Function arguments are discarded by the caller (unlike CC68/Fuzix CC where this happens in the callee).
2. The caller performs stack adjustment using `INS`, which increases code size due to frequent calls.
3. The first argument of the function is passed in a register. If the first argument is 8-bit, it is passed in AccB. If it is 16-bit, it is passed in AccAB. If it is long/float, it is passed with 4byte @long area in zero page.

### Tradeoffs:

- **1 Argument:** Efficient; no `PUSH` needed.
- **2 Arguments:** Equivalent; one `PUSH` removed but requires an `INS`.
- **3+ Arguments:** Less efficient due to additional `INS`.

Register-based arguments are saved during the prologue and accessed as local variables within functions. Further optimizations in this area are possible.

### Function Arguments Handling

Adjusting the stack in the called function can make the return process trickier. Instead of just using `RTS`, you'll need extra instructions like `TSX/LDX 0,X/INS.../JMP 0,X`. However, if many `INS` instructions are required, the difference in how the callee restores the stack becomes less significant.

## float/long in zero page

long/float are handled as 4byte variables (@long) on the zero page.

This is different from the CC68 and Fuzix CC methods. They are handled using AccA/B and as 2-byte values on the zero page.

When Acc A/B are used together, long/int mixed operations are advantageous.

For example, sign-extend int to long.

```
    ldx #0
    tsta
    bpl L1
    dex
L1: stx @hireg
```

all bytes on zero page.

```
    ldx #0
    stab @long+3
    staa @long+2
    bpl L1
    dex
L1: stx @long
```

On the other hand, AccA/B are fragile, so they may need to be saved and restored when performing long/float operations.

In the chibicc-6800, everything is on the zero page for clarity.

`@long` is also used when passing function arguments via registers. If the first (most significant) argument of a function is a `long` or `float`, it is passed through `@long`. This avoids the need for a 4-byte long push and multiple `ins` instructions.

### Passing long/float Arguments

When calling a function with a long or float argument using register passing, a zero page area (@long) is used for the argument. If the return value of one function is passed directly as an argument to the next function, the call can be made without any additional cost.

For example:

```
#include "float.h"
#include "math.h"

int main(int argc, char **argv)
{
    float x=-2.0;
    float z;

    z = sqrtf(fabsf(x));
}
```

After storing the value of x into @long, we can call fabsf and then sqrtf in sequence. There is no need to restore the stack, making the operation efficient.

```
    ldab 7,x
    stab @long+3
    ldab 6,x
    stab @long+2
    ldab 5,x
    stab @long+1
    ldab 4,x
    stab @long
    jsr _fabsf
    jsr _sqrtf
```


## Conditional branch

8bit/16bit Integer's conditional branching is fast. It uses direct comparisons (not subroutines). 

For longs and floats, a subroutine is called for comparisons, but the code is written to reduce the number of comparisons as much as possible, so it is fast enough.

For example, take a look at some of the code generated by ztest/9100-tarai.c

```
int tarai(int x, int y, int z)
{
	if (x>y){
		return tarai(
			tarai(x-1,y,z),
			tarai(y-1,z,x),
			tarai(z-1,x,y));
	}
	return y;
}
```

Branches "if (x\>y)" are converted to jge (bge) instructions.

x\>y and x>=y have different branch costs. x-y>=0 requires one bge, but x-y>0 requires multiple branch instructions. The chibicc-6800 reduces the branch cost by treating the former as y-x\<0.

```
	ldab 9,x
	ldaa 8,x
	subb 3,x
	sbca 2,x
	jge L_end_5
	ldab 9,x
	ldaa 8,x
	pshb
	psha
	ldab 3,x
	ldaa 2,x
	pshb
	psha
	ldab 11,x
	ldaa 10,x
	subb #<1
	sbca #>1
	jsr _tarai
	ins
	ins
	ins
	ins
```

## Float

The `float` type follows the IEEE 754 standard for single-precision floating-point numbers, supporting both normalized and subnormal values.

IEEE 754 32-bit floating-point arithmetic code is written in assembler, which is faster and also smaller in size compared to code written in C

Currently, addition, subtraction, multiplication, division, comparison, absolute value (fabs), and square root (sqrtf) are implemented. Other functions are under consideration.

Float can handle subnormal, NaN and Inf values. It passes basic testing.


## Large size object / local area

IX addressing can only use offsets from 0 to 255, so there are limitations. If an offset greater than 255 is needed, calculations using AccAB are required, which leads to less efficient code.

- When returning a struct or union from a function, its size must be less than 256 bytes.
- When the total size of local variables and arguments is more than 252 bytes, certain limitations apply.
- Local area refers to temporary variables, including those allocated by assignment, alloca, or variable-length arrays (VLA).

## Bitfield Support

Bit fields enable the packing of multiple small integer values into a struct, thereby reducing memory usage. However, on the MC6800, manipulation of bit fields is generally inefficient. Accessing or modifying a bit field requires multiple shift and bitwise operations (such as AND and XOR) to isolate or update specific bits. This leads to an increased instruction count and slower execution compared to access of regular integer struct members.

- Bitfields are always packed into 16-bit (int) units, starting at the least significant bit (bit 0) of each word.
- When a non-bitfield member appears in a struct, bitfield packing ends, and any following bitfields start at the next 16-bit boundary.
- Zero-width fields (e.g., unsigned int : 0;) force alignment to the next 16-bit boundary.
- In unions, the lowest-addressed byte corresponds to the upper 8 bits of the bitfield word, and the highest-addressed byte to the lower 8 bits.
- The size of a struct or union containing bitfields is rounded up so that all bitfields fit into full 16-bit units.

### Example:

```
struct S {
  unsigned int a : 5;
  _Bool        b : 1;
  unsigned int c : 10;
  unsigned char d;
  unsigned int e : 12;
};
```

- a, b, and c are packed into the first 16-bit word.
- d is placed at the next byte.
- e starts at the next 16-bit word.

---
# Usage in Other Projects

This compiler is also used in `kwhr0`'s [bm2-baremetal-demo](https://github.com/kwhr0/bm2-baremetal-demo), [kwhr0/bm2-chibicc-demo](https://github.com/kwhr0/bm2-chibicc-demo) project. 

---
# Reference compilers

[@rui314](https://www.sigbus.info/)'s [chibicc](https://github.com/rui314/chibicc/)

- [rui314/chibicc: A small C compiler](https://github.com/rui314/chibicc/)

Other compilers that may be useful to study.

slimcc and widcc are forks of chibicc. They add features and clean up the code.

Fuzix C and CC6303 contain compilers for the MC6800. The former also has float.

acwj and mc09 are compilers for the MC6809.

- [fuhsnn/slimcc: C11 compiler with C23/C2y/GNU extensions for x86-64 Linux/BSD](https://github.com/fuhsnn/slimcc)
- [fuhsnn/widcc: Simple C compiler for x86-64 Linux able to build real-world projects including Curl, GCC, Git, PHP, Perl, Python, PostgreSQL etc](https://github.com/fuhsnn/widcc)
- [EtchedPixels/Fuzix-Compiler-Kit: Fuzix C Compiler Project](https://github.com/EtchedPixels/Fuzix-Compiler-Kit/)
- [zu2/CC6303: A C compiler for the 6800 series processors](https://github.com/zu2/CC6303)
- [acwj/64\_6809\_Target at master · DoctorWkt/acwj](https://github.com/DoctorWkt/acwj/tree/master/64_6809_Target)
- [sbc09/mc09 at os9lv2 · shinji-kono/sbc09](https://github.com/shinji-kono/sbc09/tree/os9lv2/mc09)

---
---
Rui's original README.md link.

- [chibicc/README.md at main · rui314/chibicc](https://github.com/rui314/chibicc/blob/main/README.md)
