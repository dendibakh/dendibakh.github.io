Just to demonstrate it's power I created a small example in which I will calculate the [sum of the first N natural numbers](https://cseweb.ucsd.edu/groups/tatami/kumo/exs/sum/). There will intentionally be fast and slow path to do that. Here is the code:

```cpp
// a.cpp
#include <random>

int goFastPath(int* arr, int n);
int goSlowPath(int* arr, int n);

int main() {
  int arr[1000];
  for (int i = 0; i < 1000; i++) {
    arr[i] = i;
  }

  const int min = 0;
  const int max = 999;
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(min,max);

  // counting sum up to N
  for (int i = 0; i < 100000; i++) {
    int random_int = distribution(generator);
    if (random_int < 999)
      goFastPath(arr, random_int);
    else
	  goSlowPath(arr, random_int);
  }
  return 0;
}

// b.cpp
int goFastPath(int* arr, int n) {
  return (n * (n + 1)) / 2;
}

int goSlowPath(int* arr, int n) {
  int res = 0;
  for (int i = 0; i <= n; i++)
    res += arr[i];
  return res;
}
```
Building:
```bash
$ g++ a.cpp -c -O2
$ g++ b.cpp -c -O2
$ g++ a.o b.o
$ time -p ./a.out
real	0m0.007s
user	0m0.007s
sys	    0m0.000s
```

```bash
$ perf record -e intel_pt/cyc=1/u ./a.out
```

```bash
$ perf report -D > trace.dump
```

```
.  000073b3:  2d 98 8c                                        TIP 0x8c98
.  000073b6:  13                                              CYC 0x2
.  000073b7:  c0                                              TNT TNNNNN (6)
.  000073b8:  43                                              CYC 0x8
.  000073b9:  b6                                              TNT NTTNTT (6)
```

```bash
$ perf script --ns --itrace=i1t -F +insn | ../xed -F insn: -I -64 > script.xed
```

```
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27718 main+0x78 (example/a.out)               cmp eax, 0x3e6
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea2771d main+0x7d (example/a.out)               mov esi, eax
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea2771f main+0x7f (example/a.out)               mov rdi, r12
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27722 main+0x82 (example/a.out)               jle 0x5581dea27700
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27724 main+0x84 (example/a.out)               call 0x5581dea27a80
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a80 goSlowPath+0x0 (example/a.out)          test esi, esi
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a82 goSlowPath+0x2 (example/a.out)          js 0x5581dea27aa0
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a84 goSlowPath+0x4 (example/a.out)          movsxd rsi, esi
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a87 goSlowPath+0x7 (example/a.out)          xor eax, eax
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a89 goSlowPath+0x9 (example/a.out)          lea rdx, ptr [rdi+rsi*4+0x4]
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a8e goSlowPath+0xe (example/a.out)          data16 nop
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a90 goSlowPath+0x10 (example/a.out)         add eax, dword ptr [rdi]
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a92 goSlowPath+0x12 (example/a.out)         add rdi, 0x4
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a96 goSlowPath+0x16 (example/a.out)         cmp rdi, rdx
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a99 goSlowPath+0x19 (example/a.out)         jnz 0x5581dea27a90
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a90 goSlowPath+0x10 (example/a.out)         add eax, dword ptr [rdi]
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a92 goSlowPath+0x12 (example/a.out)         add rdi, 0x4
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a96 goSlowPath+0x16 (example/a.out)         cmp rdi, rdx
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a99 goSlowPath+0x19 (example/a.out)         jnz 0x5581dea27a90
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a90 goSlowPath+0x10 (example/a.out)         add eax, dword ptr [rdi]
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a92 goSlowPath+0x12 (example/a.out)         add rdi, 0x4
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a96 goSlowPath+0x16 (example/a.out)         cmp rdi, rdx
           a.out  7845 [007] 12024.856275988:          1   instructions:u:      5581dea27a99 goSlowPath+0x19 (example/a.out)         jnz 0x5581dea27a90
           a.out  7845 [007] 12024.856276039:          1   instructions:u:      5581dea27a90 goSlowPath+0x10 (example/a.out)         add eax, dword ptr [rdi]
           a.out  7845 [007] 12024.856276039:          1   instructions:u:      5581dea27a92 goSlowPath+0x12 (example/a.out)         add rdi, 0x4
           a.out  7845 [007] 12024.856276039:          1   instructions:u:      5581dea27a96 goSlowPath+0x16 (example/a.out)         cmp rdi, rdx
           a.out  7845 [007] 12024.856276039:          1   instructions:u:      5581dea27a99 goSlowPath+0x19 (example/a.out)         jnz 0x5581dea27a90
           a.out  7845 [007] 12024.856276039:          1   instructions:u:      5581dea27a90 goSlowPath+0x10 (example/a.out)         add eax, dword ptr [rdi]
           a.out  7845 [007] 12024.856276039:          1   instructions:u:      5581dea27a92 goSlowPath+0x12 (example/a.out)         add rdi, 0x4
           a.out  7845 [007] 12024.856276039:          1   instructions:u:      5581dea27a96 goSlowPath+0x16 (example/a.out)         cmp rdi, rdx
           a.out  7845 [007] 12024.856276039:          1   instructions:u:      5581dea27a99 goSlowPath+0x19 (example/a.out)         jnz 0x5581dea27a90

```

```bash
$ grep goSlowPath script.xed > slow.trace
$ grep ret slow.trace -c
93
```

```bash
# how to calculate average time
$ perf script --ns --itrace=i1t -F time,ip,sym,symoff,insn | ../xed -F insn: -I -64 | grep goSlowPath | grep "test esi" | cut -f1 -d":" > start.dump
$ perf script --ns --itrace=i1t -F time,ip,sym,symoff,insn | ../xed -F insn: -I -64 | grep goSlowPath | grep "ret" | cut -f1 -d":" > end.dump
$ paste -d- end.dump start.dump | bc -l > func.times
$ awk '{ total += $1; count++ } END { printf("%.9f",total/count)}' func.times
0.000001092
```

Try to expose srcline
