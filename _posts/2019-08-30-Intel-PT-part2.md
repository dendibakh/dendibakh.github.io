---
layout: post
title: Intel Processor Trace Part2. Better debugging experience.
categories: [tools]
---

**Contents:**
* TOC
{:toc}

------
**Subscribe to my [mailing list](https://mailchi.mp/4eb73720aafe/easyperf), support me on [Patreon](https://www.patreon.com/dendibakh) or by PayPal [donation](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=TBM3NW8TKTT34&currency_code=USD&source=url).**

------

In the [first part]({{ site.url }}/blog/2019/08/23/Intel-Processor-Trace) of my series about Intel Processor Traces (PT) I showed the underlying mechanics of this HW feature and talked a bit about its main use cases.

Here is the list of all parts in the series of posts about Intel PT:
1. Part1: [Introduction to Intel Processor Trace]({{ site.url }}/blog/2019/08/23/Intel-Processor-Trace).
2. Part2: Better debugging experience with Intel PT (this article).
3. Part3: [Analyzing performance glitches with Intel PT]({{ site.url }}/blog/2019/09/06/Intel-PT-part3).
4. Part4: [Better profiling experience with Intel PT]({{ site.url }}/blog/2019/09/13/Intel-PT-part4).

In this article I will go into one of areas where PT can provide additional value, which is debugging.

### Postmortem debugging 

Traditionally, in embedded world, issues that happen in production environment are being debugged by logging. But sometimes it's not enough. Suppose we have a function like this with a big switch inside:

```cpp
int foo(unsigned int arg) 
{
  int x = someComplexFunc(arg);
  log("Enter calculations");
  switch (arg)
  {
    case 7498536:
    {
        int y = 7498536 / x; // potential div by zero
        x = y - 42;
        break;
    }
    // <lots of other cases>
    default:
      x = 42;
      break;
  }
  log("result: x = %d\n", x);
  return x;
}
```

Customer reports that once in a while they see some of their machines crash. In the logs they provided the last lines are:
```
Time           message
19:25:13:0124: bar: calling foo
19:25:13:0134: foo: Enter calculations
```

Well, that's better than nothing, still doesn't give any clue what exactly the problem is. If the switch is big enough it will be hard to find where exactly is the issue.

Intel PT can provide a little bit more insights. I used [simple-pt](https://github.com/andikleen/simple-pt) to collect the traces [^1]. Please refer to `simple-pt` documentation[^2] for how to build and use it.

Source code for this example is available on my [github](https://github.com/dendibakh/dendibakh.github.io/tree/master/_posts/code/IntelPT/postmortem). I built the program like this:
```bash
$ gcc a.cpp -g -o app
```

The command below collects the traces. By default `simple-pt` saves the traces into 2MB circular buffer. This means new traces overwrite the old ones. So, even for long running applications we can have a trace of what was happening just before the crash:
```bash
$ sudo sptcmd -K --cyc 1 taskset -c 0 ./app
```

After we collected the traces we can decode them with: [^3]
```bash
$ sudo sptdecode -s ptout.sideband --pt ptout.0 -i -t | xed -F insn: -A -64 > dump.txt
```

If we now look into the `dump.txt`, right at the bottom we will see something like:

```
<...>
560a8ef2870d 0  call  callq  0x560a8ef2866allq
560a8ef2866b 0 other  mov %rsp, %rbp
560a8ef2866e 0 other  movl  %edi, -0x14(%rbp) 
560a8ef28671 0 other  movl  $0x0, -0x8(%rbp) 
560a8ef28678 0 other  movl  -0x14(%rbp), %eax 
560a8ef2867b 0 other  cmp $0x1d4a, %eax
560a8ef28680 0 cjump  jnz 0x560a8ef28699	// arg == 7498536
560a8ef28682 0 other  mov $0x1d4a, %eax 	// entering the block
560a8ef28687 0 other  cdq			// with div by zero
```

I was having some issues with emitting source code lines in the dumps. In `sptdecode` there is `-d` option that is supposed to print source code intermixed with the associated assembly code. After discussing the issue with Andi Kleen it looks like the problem is somewhere on my side.[^5]

In this example you can see how Intel PT can tell us the last instructions that were executed[^4]. **Intel PT can be used as an almost free[^6] addition to the  logging capabilities of your applications and often can provide a big chunk of useful information for postmortem debugging**.

Logs still are very useful because you can print some values in them. Until `PTWRITE` instruction came out there was no way of dumping data in processor traces. Traces were only useful for determining control flow. But in recent CPUs we have `PTWRITE` instruction that allows writing values into the PT packets[^7]. According to [Intel SD Manual](https://software.intel.com/en-us/articles/intel-sdm):

> This instruction reads data in the source operand and sends it to the Intel Processor Trace hardware to be encoded in a PTW packet.

I haven't used `PTWRITE` in practice, but I assume that every time you want to print something new you need to recompile the binary. Though you need to do the same when using logs.

My Intel Core i5-8259U doesn't have `PTWRITE` support. You can check whether your CPU has `PTWRITE` support with [ptfeature](http://halobates.de/spt-man/ptfeature.html) tool which is a part of `simple-pt`.

### Debugging stack corruption issues

Now let me show another case where Intel PT can be useful.

Let me jump right into the example with the program where the call stack is being corrupted:

```cpp
// a.c
void bar(); // implemented in assembly below

void foo()
{
  bar();
}

int main()
{
  foo();
}
```

```asm
// b.asm
GLOBAL bar

bar:
pop rdx       ; remove return address from the stack
xor rax, rax
ret
ud2
```

Source code for this example is available on my [github](https://github.com/dendibakh/dendibakh.github.io/tree/master/_posts/code/IntelPT/stack_corruption). Let's build the program and make sure it's crashing:

```bash
$ gcc a.c -c -g
$ nasm -f elf64 b.asm -g
$ gcc a.o b.o
$ ./a.out
Segmentation fault (core dumped)
```

When I run usual (pre-installed) version of gdb:

```bash
$ /usr/bin/gdb ./a.out
GNU gdb (Ubuntu 8.1-0ubuntu3) 8.1.0.20180409-git
(gdb) r
Starting program: /path/to/a.out
Program received signal SIGSEGV, Segmentation fault.
0x00007fffffffe336 in ?? ()
(gdb) bt
#0  0x00007fffffffe336 in ?? ()
#1  0x00007fffffffe320 in ?? ()
#2  0x00007fffffffe320 in ?? ()
#3  0x00007fffffffe320 in ?? ()
#4  0x0000555555554619 in main () at a.c:10
Backtrace stopped: frame did not save the PC
```
Stack is corrupted, so gdb is not able to unwind it. According to my experiments, [rr](https://rr-project.org/) tool cannot provide any additional value either.

Now let's try to run the same example using build-from-sources gdb and record traces. You can find particular instructions in the [appendix]({{ page.url }}#appendix-how-to-build-gdb-with-intel-pt-support) of this article.

```bash
$ /usr/local/bin/gdb ./a.out
GNU gdb (GDB) 8.3.50.20190822-git
(gdb) start
Starting program: /path/to/a.out
Temporary breakpoint 1, main () at a.c:10
10        foo();
(gdb) record btrace pt
(gdb) c
Continuing.
Program received signal SIGSEGV, Segmentation fault.
0x00007fffffffe336 in ?? ()
```
We have the crash, let's now see the call history:
```bash
(gdb) record function-call-history
1       main
2       foo
3       bar
4       ??
```
We can even pull the previous executed instructions.
```bash
(gdb) record instruction-history /m -
4          0x00005555555545fb <foo+1>:  mov    %rsp,%rbp
a.c:5     bar();
5          0x00005555555545fe <foo+4>:  mov    $0x0,%eax
6          0x0000555555554603 <foo+9>:  callq  0x555555554620 <bar>
7          0x0000555555554620 <bar+0>:  pop    %rdx
8          0x0000555555554621 <bar+1>:  xor    %rax,%rax
9          0x0000555555554624 <bar+4>:  retq
10         0x00007fffffffe330:  xor    %al,0x55(%rsi)
11         0x00007fffffffe333:  push   %rbp
12         0x00007fffffffe334:  push   %rbp
13         0x00007fffffffe335:  push   %rbp
```
Here `/m` switch is used for intermixing source code with assembly instructions. Because `bar` is implemented in assembly there is obviously no source line for it. But for `foo` function we see the corresponding source line (`a.c:5`).

After we know the exact place where the issue happened we can put normal breakpoint and restart debugging session as usual.

**As you see, Intel PT helps when debugging programs with corrupted stack**.

### Appendix: How to build gdb with Intel PT support

This [page](http://sourceware-org.1504.n7.nabble.com/Could-not-use-quot-record-btrace-quot-even-if-I-have-Intel-PT-hardware-feature-td412196.html) would probably be a good starting point. GDB uses [libipt](https://github.com/intel/libipt) for collecting PT, so we need to build it first:

```bash
git clone https://github.com/intel/libipt.git
mkdir build && cd build
cmake ../libipt
make
make install
```

Then we build gdb from sources as shown [here](https://bugzilla.redhat.com/show_bug.cgi?id=1526617#c5):

```bash
sudo apt install texinfo bison flex
git clone git://sourceware.org/git/binutils-gdb.git
mkdir build && cd build
../binutils-gdb/configure --disable-binutils --disable-ld --disable-gold --disable-gas --disable-sim --disable-gprof
make
make install
```

------
[^1]: Snapshot mode `-S` is also available in perf tool, see [documentation](https://github.com/torvalds/linux/blob/master/tools/perf/Documentation/intel-pt.txt#L437). But I wasn't able to make this working. Only in full mode I was able to see the traces.
[^2]: In my case I went through all the step of singing the kernel module before I was able to use `simple-pt`. Following links might help you to do this: [this](https://github.com/andikleen/simple-pt/issues/8), [this](https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7/html/Kernel_Administration_Guide/sect-signing-kernel-modules-for-secure-boot.html), [this](https://superuser.com/questions/1214116/no-openssl-sign-file-signing-key-pem-leads-to-error-while-loading-kernel-modules) and [this](https://wiki.ubuntu.com/UEFI/SecureBoot/DKMS).
[^3]: See this [link](https://github.com/andikleen/simple-pt#installation) for instructions how to build `xed`.
[^4]: Note however that the last instruction that caused the division exception is not shown in the trace.
[^5]: Although perf is able to emit source code location for the corresponding assembly code. Use: `perf record -e intel_pt/cyc=1/u ...` and then `perf script -F +srcline ... `.
[^6]: Check the [first part]({{ site.url }}/blog/2019/08/23/Intel-Processor-Trace) of this series for information about runtime overhead.
[^7]: See the [first part]({{ site.url }}/blog/2019/08/23/Intel-Processor-Trace) of this series for information about Intel PT packets.
