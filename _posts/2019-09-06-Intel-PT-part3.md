---
layout: post
title: Intel Processor Trace Part3. Analyzing performance glitches.
categories: [tools]
---

**Contents:**
* TOC
{:toc}

------
**Subscribe to my [mailing list](https://mailchi.mp/4eb73720aafe/easyperf), support me on [Patreon](https://www.patreon.com/dendibakh) or by PayPal [donation](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=TBM3NW8TKTT34&currency_code=USD&source=url).**

------

This is the third part of my series about Intel Processor Traces (PT). In the [first part]({{ site.url }}/blog/2019/08/23/Intel-Processor-Trace) of this series I showed the underlying mechanics of this HW feature. In the [second part]({{ site.url }}/blog/2019/08/30/Intel-PT-part2) I showed 2 ways how PT can enhance your debugging experience. 

Here is the list of all parts in the series of posts about Intel PT:
1. Part1: [Introduction to Intel Processor Trace]({{ site.url }}/blog/2019/08/23/Intel-Processor-Trace).
2. Part2: [Better debugging experience with Intel PT]({{ site.url }}/blog/2019/08/30/Intel-PT-part2).
3. Part3: Analyzing performance glitches with Intel PT (this article).
4. Part4: [Better profiling experience with Intel PT]({{ site.url }}/blog/2019/09/13/Intel-PT-part4).

In this article we will focus more on the performance side of this CPU feature.

### Analyzing performance glitches

Imagine a situation when you're a developer of an application. Everything works smoothly in general, besides one little thing. Users of the application claim that sometimes they observe glitches in the application performance. Often times this is quite hard to fix, since it won't be exposed as a hotspot when you profile the app. Also, it's hard to isolate and extract it to the standalone benchmark, since performance glitches happen rarely, and the root cause may be not obvious. [^1]

To demonstrate how Intel PT can help in this case, I created a small example in which I calculate the [sum of the first N natural numbers](https://cseweb.ucsd.edu/groups/tatami/kumo/exs/sum/). There is intentionally fast and slow path of the algorithm. Here is the code:

a.cpp:
{% highlight cpp linenos %}
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
{% endhighlight %}

b.cpp:
{% highlight cpp linenos %}
int goFastPath(int* arr, int n) {
  return (n * (n + 1)) / 2;
}

int goSlowPath(int* arr, int n) {
  int res = 0;
  for (int i = 0; i <= n; i++)
    res += arr[i];
  return res;
}
{% endhighlight %}

The idea is that 99.9% of the time we are using the fast path which only takes a few CPU cycles. But in 0.1% of the time we will fall back to the slow path which can take as much as 1000 cycles. [^2]

Here is how I build the executable: [^3]
```bash
$ g++ a.cpp -c -O2 -g
$ g++ b.cpp -c -O2 -g
$ g++ a.o b.o
$ time ./a.out
real	0m0.007s
user	0m0.007s
sys	0m0.000s
```

Now let's collect the traces:
```bash
$ perf record -e intel_pt/cyc=1/u ./a.out
[ perf record: Woken up 1 times to write data ]
[ perf record: Captured and wrote 0.548 MB perf.data ]
```
Notice that I used `cyc=1` to have the finest granularity available. This option asks the CPU to produce timing packets every cycle. [^4]

To decode the traces into human readable form use the command below:
```bash
$ perf script --ns --itrace=i1t -F +srcline,+srccode > decoded.dump
```

Before going into the output, let me explain what those options mean. Option `--ns` will show the timestamps in nanoseconds. Option `-F +srcline,+srccode` will add source line and source code for each decoded assembly instruction. Option `--trace=i1t` is a little more difficult to explain. This option specifies a period on which decoder will sample our traces. In my case I asked to sample every clocktick, i.e. it will output each instruction separately and put a timestamp for it. See `man perf script` for more details and footnotes[^5] for more examples.

Here is what the decoded output (in `decoded.dump`) might look like: [^6]

```
proc   PID   CPU    timestamp       sym+off          srcline   srccode
...
a.out  3172 [007]   253.555413143:  std::uniform_int_distribution<...> >+0xc2  uniform_int_dist.h:281
a.out  3172 [007]   253.555413143:  main+0x78        a.cpp:21  if (random_int < 999)
a.out  3172 [007]   253.555413143:  main+0x7d        a.cpp:22    goFastPath(arr, random_int);
a.out  3172 [007]   253.555413143:  main+0x7f        a.cpp:22
a.out  3172 [007]   253.555413143:  main+0x82        a.cpp:21  if (random_int < 999)
a.out  3172 [007]   253.555413143:  main+0x84        a.cpp:24    goSlowPath(arr, random_int);
a.out  3172 [007]   253.555413143:  goSlowPath+0x0   b.cpp:7   for (int i = 0; i <= n; i++)
a.out  3172 [007]   253.555413143:  goSlowPath+0x2   b.cpp:7
a.out  3172 [007]   253.555413143:  goSlowPath+0x4   b.cpp:7
a.out  3172 [007]   253.555413143:  goSlowPath+0x7   b.cpp:6   int res = 0;
a.out  3172 [007]   253.555413143:  goSlowPath+0x9   b.cpp:6
a.out  3172 [007]   253.555413143:  goSlowPath+0xe   b.cpp:6
a.out  3172 [007]   253.555413143:  goSlowPath+0x10  b.cpp:8     res += arr[i];
a.out  3172 [007]   253.555413143:  goSlowPath+0x12  b.cpp:8
a.out  3172 [007]   253.555413143:  goSlowPath+0x16  b.cpp:7   for (int i = 0; i <= n; i++)
a.out  3172 [007]   253.555413143:  goSlowPath+0x19  b.cpp:7
a.out  3172 [007]   253.555413143:  goSlowPath+0x10  b.cpp:8   res += arr[i];
a.out  3172 [007]   253.555413143:  goSlowPath+0x12  b.cpp:8
a.out  3172 [007]   253.555413143:  goSlowPath+0x16  b.cpp:7   for (int i = 0; i <= n; i++)
a.out  3172 [007]   253.555413143:  goSlowPath+0x19  b.cpp:7
<loop iterations>
a.out  3172 [007]   253.555413508:  goSlowPath+0x19  b.cpp:7
a.out  3172 [007]   253.555413508:  goSlowPath+0x1b  b.cpp:7
a.out  3172 [007]   253.555413508:  main+0x89        a.cpp:19  for (int i = 0; i < 100000; i++) {
...
```

**Having such a trace is very helpful for any kind of analysis**. It allows you to zoom in the exact invocation where the glitch happened and analyze it in isolation. You may further use the timestamp to align it with the other event that was happening at the same moment in the system. Also, you can compare two different invocations of the same function: the one that was fast and the one that was slow. You may then go back in time and compare what was happening before the glitch in both cases and hopefully will find the thing that was different.

Decoded processor traces won't be able to show the values of different variables [^7] but you will get precise control flow. With this information you will know exactly what lead you here and hopefully it will sched some light on what might be causing performance issues. I'm not showing the history here since in our case it's obvious what caused the glitch of 365ns.

But... There is a caveat. When adding either `+srcline` or `+srccode` to output, decoding gets extremely slow. For this workload that runs for only 7ns and generated traces of less than 1MB, decoding took more than a day! [^8] I know this is nuts for most of the users. I'm not entirely sure why that happens, but I suspect the implementation might not be optimal.

The good news is that there is a workaround. You don't have to decode the whole trace in advance and can do it lazily. First you can decode without adding `+srcline` or `+srccode` to the `perf script` command. Then look at the time range that you care about and then only decode this time range using `--time start,stop` option. For example:

```bash
$ perf script --ns --itrace=i1t -F +srcline,+srccode --time 253.555413140,253.555413520 > time_range.dump
```

If you're also interested in assembly instructions, you can do this with the command:

```bash
$ perf script --ns --itrace=i1t -F +insn | ../xed -F insn: -I -64 > decoded_insn.dump
```

This type of decoding is much faster. Note that I also used `xed` [^9] to decode the raw bytes into instructions. Here is what the decoded output (in `decoded_insn.dump`) might look like:

```
proc   PID   CPU    timestamp       sym+off          instruction
...
a.out  3172 [007]   253.555413143:  main+0x78        cmp eax, 0x3e6
a.out  3172 [007]   253.555413143:  main+0x7d        mov esi, eax
a.out  3172 [007]   253.555413143:  main+0x7f        mov rdi, r12
a.out  3172 [007]   253.555413143:  main+0x82        jle 0x556830247700
a.out  3172 [007]   253.555413143:  main+0x84        call 0x556830247a80
a.out  3172 [007]   253.555413143:  goSlowPath+0x0   test esi, esi		<== enter goSlowPath()
a.out  3172 [007]   253.555413143:  goSlowPath+0x2   js 0x556830247aa0
a.out  3172 [007]   253.555413143:  goSlowPath+0x4   movsxd rsi, esi
a.out  3172 [007]   253.555413143:  goSlowPath+0x7   xor eax, eax
a.out  3172 [007]   253.555413143:  goSlowPath+0x9   lea rdx, ptr [rdi+rsi*4+0x4]
a.out  3172 [007]   253.555413143:  goSlowPath+0xe   data16 nop
a.out  3172 [007]   253.555413143:  goSlowPath+0x10  add eax, dword ptr [rdi]
a.out  3172 [007]   253.555413143:  goSlowPath+0x12  add rdi, 0x4
a.out  3172 [007]   253.555413143:  goSlowPath+0x16  cmp rdi, rdx
a.out  3172 [007]   253.555413143:  goSlowPath+0x19  jnz 0x556830247a90
a.out  3172 [007]   253.555413143:  goSlowPath+0x10  add eax, dword ptr [rdi]
a.out  3172 [007]   253.555413143:  goSlowPath+0x12  add rdi, 0x4
a.out  3172 [007]   253.555413143:  goSlowPath+0x16  cmp rdi, rdx
a.out  3172 [007]   253.555413143:  goSlowPath+0x19  jnz 0x556830247a90
<loop iterations>
a.out  3172 [007]   253.555413508:  goSlowPath+0x16  cmp rdi, rdx
a.out  3172 [007]   253.555413508:  goSlowPath+0x19  jnz 0x556830247a90
a.out  3172 [007]   253.555413508:  goSlowPath+0x1b  ret			<== exit goSlowPath()
a.out  3172 [007]   253.555413508:  main+0x89        sub ebx, 0x1
...
```

If you want combined output, use:
```bash
$ perf script --ns --itrace=i1t -F comm,tid,pid,time,cpu,ip,sym,symoff,srcline,srccode,insn | ../xed -F insn: -I -64 > decoded_combined.dump
```
This is the output that you might see:
```
proc   PID   CPU    timestamp       sym+off          srcline   instruction      srccode
...
a.out  3172 [007]   253.555413143:  main+0x84        a.cpp:24  call 0x35c       goSlowPath(arr, random_int);
a.out  3172 [007]   253.555413143:  goSlowPath+0x0   b.cpp:7   test esi, esi    for (int i = 0; i <= n; i++)
a.out  3172 [007]   253.555413143:  goSlowPath+0x2   b.cpp:7   js 0x1e
a.out  3172 [007]   253.555413143:  goSlowPath+0x4   b.cpp:7   movsxd rsi, esi
...
```

Finally, we can also calculate interesting statistics like "How much times we called goSlowPath function?":

```bash
$ grep goSlowPath decoded_insn.dump > slow.trace
$ grep ret slow.trace -c
93
```

Note that this is **exact** number of calls! This is not a wild guess. We know precisely that `goSlowPath` was called 93 times.

We can calculate average time for one invocation of `goSlowPath` function:

```bash
$ grep "test esi" slow.trace | cut -f1 -d":" > start.dump
$ grep "ret" slow.trace | cut -f1 -d":" > end.dump
$ paste -d- end.dump start.dump | bc -l > func.times
$ awk '{ total += $1; count++ } END { printf("%.9f",total/count)}' func.times
0.000001092
```

And there are many other ways how to extract useful information from the Intel PT traces.

That's it. Hope that you find this article useful and maybe will apply it in your analysis someday. I agree that there are not many tools that support Intel PT, and even those tools don't perfectly handle all the cases. Anyway, I hope it will get better in the future.

------
[^1]: Bruce Dawson has a lot of interesting articles on his [blog](https://randomascii.wordpress.com/) with examples how he uses Windows ETW traces to debug performance glitches. He has ETW traces turned on `24/7` on his machines which allows him to capture what was going on in the last minute or so. By analyzing those postmortem traces, he was able to root cause many interesting bugs including [this](https://randomascii.wordpress.com/2012/09/04/windows-slowdown-investigated-and-identified/), [this](https://randomascii.wordpress.com/2016/03/08/power-wastage-on-an-idle-laptop/) or [this](https://randomascii.wordpress.com/2018/08/16/24-core-cpu-and-i-cant-type-an-email-part-one/).
[^2]: If no vectorization/unrolling happened. It is the case in my example.
[^3]: Note that I implemented functions in different files in order to eliminate compiler's interprocedural optimizations. For that same reason [LTO](https://en.wikipedia.org/wiki/Interprocedural_optimization) (`-flto`) is not used. Also, I compiled with debug information `-g` in order to have source line information available in the decoded traces.
[^4]: However, they are not sent every cycle. Read more about the timing packets in the [first part]({{ site.url }}/blog/2019/08/23/Intel-Processor-Trace) of this series and in Linux kernel [documentation](https://github.com/torvalds/linux/blob/master/tools/perf/Documentation/intel-pt.txt#L355).
[^5]: For example, if you specify `--trace=i100ns` the decoder will synthesize one instruction roughly each 100ns. It will output only 1 sample but will show near it how much instructions were omitted. It's a way to go for initial analysis of big traces, since increasing period will allow the decoder work much faster. Read more in Linux kernel [documentation](https://github.com/torvalds/linux/blob/master/tools/perf/Documentation/intel-pt.txt#L814).
[^6]: I formatted the output and omitted several columns. You can add and remove data from the output as you want with `-F` option. Check `man perf script` for more details.
[^7]: Although that is also possible with `PTWRITE` assembly instruction that allows writing values into the PT packets. Check [Intel SD Manual](https://software.intel.com/en-us/articles/intel-sdm) for more information about `PTWRITE` and the [first part]({{ site.url }}/blog/2019/08/23/Intel-Processor-Trace) of this series for information about Intel PT packets.
[^8]: This is only the case for `+srcline` and `+srccode`. Decoding source lines only (`+srcline`) takes 6 hours and still is nuts. According to `time -p` decoder spends 70% of the time in kernel space which indicates that implementation can be significantly improved by caching. If you do not specify `+srcline` or `+srccode` decoder usually runs in the orders of minutes (not hours).
[^9]: Xed is "x86 encoder decoder" and it's available on [github](https://github.com/intelxed/xed).
