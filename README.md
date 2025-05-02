# chibicc-6800-v1: A Small C Compiler for MC6800

This project is a fork of [@rui314](https://www.sigbus.info/)'s [chibicc](https://github.com/rui314/chibicc/), modified to create a C compiler for the Motorola MC6800 architecture.

This project was created as a tool for studying compilers for the MC6800, and while it includes extra code and comments that may not be essential, they reflect the learning process and experimentation involved in developing the compiler.

There are many aspects of the object code generation method and speed that cannot be understood without actually creating it. There may be unnecessary parts added for testing code generation.

However, it is a compiler that works reasonably well. I hope it will be helpful for you to create another fork.

Dhrystone works now. 

- https://github.com/z88dk/z88dk/tree/master/libsrc/\_DEVELOPMENT/EXAMPLES/benchmarks/dhrystone21
----
# Topics

- **Data types:** `int` and pointers are 16-bit; `long` and `float` are 32-bit. `double` and `long long` (64-bit or more) are unsupported.
- **Structs/unions:** Passing/returning by value and bit fields are implemented.
- **Function parameters:** Only the first parameter is passed via registers (A/B/@long). If the first parameter is a struct/union, all parameters are passed via the stack.
- **Return values:** Struct/union return values pass their address as an implicit first argument in a register. all other normal arguments are passed on the stack.
- **Limitations:** Handling of large local variable areas (>255 bytes) is poorly tested and coded.

IEEE 754 32-bit floating-point arithmetic code is written in assembler, which is faster and also smaller in size compared to code written in C

It can handle subnormal, NaN and Inf values. It passes basic testing but is not well tested for precision.

The compiler passes basic tests, but there are still many issues remaining.

The original chibicc compiles C source code using stack-based operations, but also makes use of the many registers available on x64. This is inefficient on the MC6800, which has few registers and limited stack support. chibicc-6800-v1 optimizes code generation for the MC6800.

Fuzix-BinTools is required to assemble and link compiled objects.
For testing, we use emu6800 from Fuzix-Compiler-Kit.

- https://github.com/EtchedPixels/Fuzix-Bintools
- https://github.com/EtchedPixels/Fuzix-Compiler-Kit

---
# Installation

Follow these steps to set up the compiler:

1. Install **Fuzix-BinTools** and **Fuzix-Compiler-Kit**.
2. Clone this repository:
```
   git clone https://github.com/zu2/chibicc-6800-v1.git
```
3. Build and install:
```
   cd chibicc-6800-v1
   make
   make install
```
4. Add the following paths to your `$PATH`:
```
   export PATH="/opt/chibicc/bin:/opt/fcc/bin:$PATH"
```
5. test compile a C file using:
```
   cd ztest
   ./onetest 9018-asciiartf.c
```
6. To run the test programs
```
    cd ztest; ./runall
    cd ztest; ./onetest some-test-program.c
```

---
# Memo

## Stack frame

This compiler, like the x86 version, uses the frame pointer (`@bp`) to access local variables and arguments.  
On the 6800, due to the limited number of registers, `@bp` is placed in the zero page.  
Unlike other compilers that obtain the stack position using `TSX`, chibicc-6800 uses `LDX @bp`.  
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


## Conditional branch

Integer's conditional branching is fast. It uses direct comparisons (not subroutines). 

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

Float can handle subnormal, NaN and Inf values. It passes basic testing but is not well tested for precision.

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
Below is Rui's original README.md.

# chibicc: A Small C Compiler

(The old master has moved to
[historical/old](https://github.com/rui314/chibicc/tree/historical/old)
branch. This is a new one uploaded in September 2020.)

chibicc is yet another small C compiler that implements most C11
features. Even though it still probably falls into the "toy compilers"
category just like other small compilers do, chibicc can compile
several real-world programs, including [Git](https://git-scm.com/),
[SQLite](https://sqlite.org),
[libpng](http://www.libpng.org/pub/png/libpng.html) and chibicc
itself, without making modifications to the compiled programs.
Generated executables of these programs pass their corresponding test
suites. So, chibicc actually supports a wide variety of C11 features
and is able to compile hundreds of thousands of lines of real-world C
code correctly.

chibicc is developed as the reference implementation for a book I'm
currently writing about the C compiler and the low-level programming.
The book covers the vast topic with an incremental approach; in the first
chapter, readers will implement a "compiler" that accepts just a single
number as a "language", which will then gain one feature at a time in each
section of the book until the language that the compiler accepts matches
what the C11 spec specifies. I took this incremental approach from [the
paper](http://scheme2006.cs.uchicago.edu/11-ghuloum.pdf) by Abdulaziz
Ghuloum.

Each commit of this project corresponds to a section of the book. For this
purpose, not only the final state of the project but each commit was
carefully written with readability in mind. Readers should be able to learn
how a C language feature can be implemented just by reading one or a few
commits of this project. For example, this is how
[while](https://github.com/rui314/chibicc/commit/773115ab2a9c4b96f804311b95b20e9771f0190a),
[[]](https://github.com/rui314/chibicc/commit/75fbd3dd6efde12eac8225d8b5723093836170a5),
[?:](https://github.com/rui314/chibicc/commit/1d0e942fd567a35d296d0f10b7693e98b3dd037c),
and [thread-local
variable](https://github.com/rui314/chibicc/commit/79644e54cc1805e54428cde68b20d6d493b76d34)
are implemented. If you have plenty of spare time, it might be fun to read
it from the [first
commit](https://github.com/rui314/chibicc/commit/0522e2d77e3ab82d3b80a5be8dbbdc8d4180561c).

If you like this project, please consider purchasing a copy of the book
when it becomes available! 😀 I publish the source code here to give people
early access to it, because I was planing to do that anyway with a
permissive open-source license after publishing the book. If I don't charge
for the source code, it doesn't make much sense to me to keep it private. I
hope to publish the book in 2021.
You can sign up [here](https://forms.gle/sgrMWHGeGjeeEJcX7) to receive a
notification when a free chapter is available online or the book is published.

I pronounce chibicc as _chee bee cee cee_. "chibi" means "mini" or
"small" in Japanese. "cc" stands for C compiler.

## Status

chibicc supports almost all mandatory features and most optional
features of C11 as well as a few GCC language extensions.

Features that are often missing in a small compiler but supported by
chibicc include (but not limited to):

- Preprocessor
- float, double and long double (x87 80-bit floating point numbers)
- Bit-fields
- alloca()
- Variable-length arrays
- Compound literals
- Thread-local variables
- Atomic variables
- Common symbols
- Designated initializers
- L, u, U and u8 string literals
- Functions that take or return structs as values, as specified by the
  x86-64 SystemV ABI

chibicc does not support complex numbers, K&R-style function prototypes
and GCC-style inline assembly. Digraphs and trigraphs are intentionally
left out.

chibicc outputs a simple but nice error message when it finds an error in
source code.

There's no optimization pass. chibicc emits terrible code which is probably
twice or more slower than GCC's output. I have a plan to add an
optimization pass once the frontend is done.

I'm using Ubuntu 20.04 for x86-64 as a development platform. I made a
few small changes so that chibicc works on Ubuntu 18.04, Fedora 32 and
Gentoo 2.6, but portability is not my goal at this moment. It may or
may not work on systems other than Ubuntu 20.04.

## Internals

chibicc consists of the following stages:

- Tokenize: A tokenizer takes a string as an input, breaks it into a list
  of tokens and returns them.

- Preprocess: A preprocessor takes as an input a list of tokens and output
  a new list of macro-expanded tokens. It interprets preprocessor
  directives while expanding macros.

- Parse: A recursive descendent parser constructs abstract syntax trees
  from the output of the preprocessor. It also adds a type to each AST
  node.

- Codegen: A code generator emits an assembly text for given AST nodes.

## Contributing

When I find a bug in this compiler, I go back to the original commit that
introduced the bug and rewrite the commit history as if there were no such
bug from the beginning. This is an unusual way of fixing bugs, but as a
part of a book, it is important to keep every commit bug-free.

Thus, I do not take pull requests in this repo. You can send me a pull
request if you find a bug, but it is very likely that I will read your
patch and then apply that to my previous commits by rewriting history. I'll
credit your name somewhere, but your changes will be rewritten by me before
submitted to this repository.

Also, please assume that I will occasionally force-push my local repository
to this public one to rewrite history. If you clone this project and make
local commits on top of it, your changes will have to be rebased by hand
when I force-push new commits.

## Design principles

chibicc's core value is its simplicity and the reability of its source
code. To achieve this goal, I was careful not to be too clever when
writing code. Let me explain what that means.

Oftentimes, as you get used to the code base, you are tempted to
_improve_ the code using more abstractions and clever tricks.
But that kind of _improvements_ don't always improve readability for
first-time readers and can actually hurts it. I tried to avoid the
pitfall as much as possible. I wrote this code not for me but for
first-time readers.

If you take a look at the source code, you'll find a couple of
dumb-looking pieces of code. These are written intentionally that way
(but at some places I might be actually missing something,
though). Here is a few notable examples:

- The recursive descendent parser contains many similar-looking functions
  for similar-looking generative grammar rules. You might be tempted
  to _improve_ it to reduce the duplication using higher-order functions
  or macros, but I thought that that's too complicated. It's better to
  allow small duplications instead.

- chibicc doesn't try too hard to save memory. An entire input source
  file is read to memory first before the tokenizer kicks in, for example.

- Slow algorithms are fine if we know that n isn't too big.
  For example, we use a linked list as a set in the preprocessor, so
  the membership check takes O(n) where n is the size of the set.  But
  that's fine because we know n is usually very small.
  And even if n can be very big, I stick with a simple slow algorithm
  until it is proved by benchmarks that that's a bottleneck.

- Each AST node type uses only a few members of the `Node` struct members.
  Other unused `Node` members are just a waste of memory at runtime.
  We could save memory using unions, but I decided to simply put everything
  in the same struct instead. I believe the inefficiency is negligible.
  Even if it matters, we can always change the code to use unions
  at any time. I wanted to avoid premature optimization.

- chibicc always allocates heap memory using `calloc`, which is a
  variant of `malloc` that clears memory with zero. `calloc` is
  slightly slower than `malloc`, but that should be neligible.

- Last but not least, chibicc allocates memory using `calloc` but never
  calls `free`. Allocated heap memory is not freed until the process exits.
  I'm sure that this memory management policy (or lack thereof) looks
  very odd, but it makes sense for short-lived programs such as compilers.
  DMD, a compiler for the D programming language, uses the same memory
  management scheme for the same reason, for example [1].

## About the Author

I'm Rui Ueyama. I'm the creator of [8cc](https://github.com/rui314/8cc),
which is a hobby C compiler, and also the original creator of the current
version of [LLVM lld](https://lld.llvm.org) linker, which is a
production-quality linker used by various operating systems and large-scale
build systems.

## References

- [tcc](https://bellard.org/tcc/): A small C compiler written by Fabrice
  Bellard. I learned a lot from this compiler, but the design of tcc and
  chibicc are different. In particular, tcc is a one-pass compiler, while
  chibicc is a multi-pass one.

- [lcc](https://github.com/drh/lcc): Another small C compiler. The creators
  wrote a [book](https://sites.google.com/site/lccretargetablecompiler/)
  about the internals of lcc, which I found a good resource to see how a
  compiler is implemented.

- [An Incremental Approach to Compiler
  Construction](http://scheme2006.cs.uchicago.edu/11-ghuloum.pdf)

- [Rob Pike's 5 Rules of Programming](https://users.ece.utexas.edu/~adnan/pike.html)

[1] https://www.drdobbs.com/cpp/increasing-compiler-speed-by-over-75/240158941

> DMD does memory allocation in a bit of a sneaky way. Since compilers
> are short-lived programs, and speed is of the essence, DMD just
> mallocs away, and never frees.
