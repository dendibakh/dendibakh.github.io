example from [LWN article by Andi Kleen](https://lwn.net/Articles/648154/):
```
int main() {
  int i;
  for (i = 0; i < 10; i++) {
    if (i >= 8)
      *(int*)0 = 0; // crash
  }
  return 0;
}
```

```bash
[building gdb from sources](https://bugzilla.redhat.com/show_bug.cgi?id=1526617#c5)
sudo apt install texinfo bison flex
git clone git://sourceware.org/git/binutils-gdb.git
mkdir build && cd build
../binutils-gdb/configure --disable-binutils --disable-ld --disable-gold --disable-gas --disable-sim --disable-gprof
make
make install
```

```bash
https://github.com/intel/libipt
[building libipt](http://sourceware-org.1504.n7.nabble.com/Could-not-use-quot-record-btrace-quot-even-if-I-have-Intel-PT-hardware-feature-td412196.html)
git clone https://github.com/intel/libipt.git
mkdir build && cd build
cmake ../libipt
make
make install
```

```bash
$ gcc a.c -g
$ /usr/local/bin/gdb ./a.out 
GNU gdb (GDB) 8.3.50.20190822-git
Copyright (C) 2019 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "x86_64-pc-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ./a.out...
(gdb) start
Temporary breakpoint 1 at 0x5fe: file a.c, line 3.
Starting program: /home/dendibakh/workspace/intel_pt/debug/a.out 

Temporary breakpoint 1, main () at a.c:3
3	  for (i = 0; i < 10; i++) {
(gdb) record btrace pt
(gdb) c
Continuing.

Program received signal SIGSEGV, Segmentation fault.
0x0000555555554612 in main () at a.c:5
5	      *(int*)0 = 0; // crash
(gdb) record instruction-history /m -
38	   0x000055555555461c <main+34>:	cmpl   $0x9,-0x4(%rbp)
39	   0x0000555555554620 <main+38>:	jle    0x555555554607 <main+13>
a.c:4	    if (i >= 8)
40	   0x0000555555554607 <main+13>:	cmpl   $0x7,-0x4(%rbp)
41	   0x000055555555460b <main+17>:	jle    0x555555554618 <main+30>
a.c:3	  for (i = 0; i < 10; i++) {
42	   0x0000555555554618 <main+30>:	addl   $0x1,-0x4(%rbp)
43	   0x000055555555461c <main+34>:	cmpl   $0x9,-0x4(%rbp)
44	   0x0000555555554620 <main+38>:	jle    0x555555554607 <main+13>
a.c:4	    if (i >= 8)
45	   0x0000555555554607 <main+13>:	cmpl   $0x7,-0x4(%rbp)
46	   0x000055555555460b <main+17>:	jle    0x555555554618 <main+30>
a.c:5	      *(int*)0 = 0; // crash
47	   0x000055555555460d <main+19>:	mov    $0x0,%eax
(gdb) reverse-step
5	      *(int*)0 = 0; // crash
(gdb) p i
$1 = <unavailable>
```bash
