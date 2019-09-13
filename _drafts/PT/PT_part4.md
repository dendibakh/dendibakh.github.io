---
layout: post
title: Intel Processor Trace Part4. Better profiling experience.
categories: [tools]
---

**Contents:**
* TOC
{:toc}

------
**Subscribe to my [mailing list]({{ page.url }}#mc_embed_signup) and support me on [Patreon](https://www.patreon.com/dendibakh).**

------

This is the 4th part of my series about Intel Processor Traces (PT). If you read this article and are not familiar with what Intel PT is, I encourage you to take a few minutes and quickly grasp through the [part1]({{ site.url }}/blog/2019/08/23/Intel-Processor-Trace) of this series.

Here is the list of all parts:
1. Part1: [Introduction to Intel Processor Trace]({{ site.url }}/blog/2019/08/23/Intel-Processor-Trace).
2. Part2: [Better debugging experience with Intel PT]({{ site.url }}/blog/2019/08/30/Intel-PT-part2).
3. Part3: [Analyzing performance glitches with Intel PT]({{ site.url }}/blog/2019/09/06/Intel-PT-part3).
4. Part4: Introduction to Intel Processor Trace(this article).

Traditionally performance analysis is done through profiling which is the process of collecting statistics about the exectuion of an application. This process is implemented with periodically stopping the execution and recording where we are in the program right now. Going one level below, each stop is made by triggering an interrupt (PMI - Performance Monitoring Interrupt). And if we collect enough samples, we can get a nice distribution of the time spent in different part of the program. This is how we can identify the hotspots in our program, for example.

However, taking an interrupt itself comes with a cost. Imagine bursty application which sleeps almost most of the time. Profiling such an application might not show anything interesting at all. Since it can happen that all our interrupts occur during the time when the app was sleeping. This is the case when we would want to increase the frequency of our interrupts. The more interrupts, the more runtime overhead. In this article I will show how Intel PT can give you better accuracy with less runtime overhead.

If you've read [part3]({{ site.url }}/blog/2019/09/06/Intel-PT-part3) of this series, you would know how to collect the traces. If not, here is the command:

```bash
$ perf record -e intel_pt//u ./a.out
```

This command essentially dumps the processor traces into a file on a disk. Those are the traces for the whole runtime. What's interesting, is that you can view it as a collection of samples. And it is the exact collection in a sense that we didn't missed a sample. And now we can use those traces to do the same analysis we can do with interrupts, but much more precise.

To demonstrate what I wrote in the beginning, let me give you an example which I borrowed from [part3]({{ site.url }}/blog/2019/09/06/Intel-PT-part3). Suppose we have an application `a.out` that runs for 1.78s:

```bash
$ time -p ./a.out
real 1.78
user 1.78
sys 0.00
```

Collecting Processor Traces involves ~11% runtime overhead:

```bash
$ time -p perf record -e intel_pt/cyc=1/u ./a.out
[ perf record: Woken up 199 times to write data ]
[ perf record: Captured and wrote 397.617 MB perf.data ]
real 1.98
user 1.90
sys 0.26
```

Now let's do the same with traditional sampling at the highest possible accuracy:

```bash
$ echo 999999999 | sudo tee /proc/sys/kernel/perf_event_max_sample_rate
$ time -p perf record -F 999999999 -e cycles:u ./a.out
[ perf record: Woken up 39 times to write data ]
[ perf record: Captured and wrote 9.712 MB perf.data (253272 samples) ]
real 2.22
user 2.11
sys 0.05
$ perf report -i pmi.data -n --stdio --no-call-graph --itrace=i100ns
# Samples: 434K of event 'cycles:u'
# Overhead       Samples  Command  Shared Object  Symbol                                                                                                                         
# ........  ............  .......  .............  ..................................................
#
    88.53%        384801  a.out    a.out          [.] std::uniform_int_distribution<int>::operator()
     5.76%         25044  a.out    a.out          [.] goFastPath
     3.64%         15807  a.out    a.out          [.] main
     1.82%          7923  a.out    a.out          [.] goSlowPath
     0.25%          1067  a.out    ld-2.27.so     [.] _start
```

The maximum sample rate I was able to collect with interrupt-based sampling on my machine is around 100Mhz (100K samples/second). I.e. we have roughly 1 sample each 10 microseconds. Don't get me wrong, it's still pretty high precision and is sutable for most of the cases. However, the overhead goes from `1.87s` [^rate](default 1000Hz sample rate, `-F 1000`) to `2.22s` and now it is ~28%, which is much more than with Intel PT.

With Intel PT, easily we get 100x more samples:

```bash
$ perf record -e intel_pt/cyc=1/u -o pt.data ./a.out
$ perf report -i pt.data -n --stdio --no-call-graph --itrace=i100ns
# Samples: 32M of event 'instructions:u'
# Overhead       Samples  Command  Shared Object        Symbol                                                                                                                         
# ........  ............  .......  ...................  ..................................................
#
    76.10%      25030327  a.out    a.out                [.] std::uniform_int_distribution<int>::operator()
    10.57%       3523065  a.out    a.out                [.] main
     8.48%       2811222  a.out    a.out                [.] goFastPath
     4.82%        677681  a.out    a.out                [.] goSlowPath
```

Let me first explain the mechanics of what we just did. After executing `perf record` we have the traces in `pt.data`. After that we asked perf to build report by synthesizing one intruction sample per 100ns. We can identify the hotspots basically the same way we do with interrupts: skip 100ns of traces, see where we are in the program, skip 100ns of traces more and record the instruction again, and so on. See [part3]({{ site.url }}/blog/2019/09/06/Intel-PT-part3) for better understanding of how PT traces look like.

I'm not 100% sure why there is such a big difference between reports based on interrupts and PT. I tend to think this is the skid effect, which might make signigicant impact on small applications like ours. You can read more about the skid [here]({{ site.url }}/blog/2018/08/29/Understanding-performance-events-skid) and [here](https://travisdowns.github.io/blog/2019/08/20/interrupts.html). In the end, I tend to believe PT based report more, but even it is not 100% precise, since it doesn't analyze all instructions and simply takes one instruction per specified time period.

In general Intel PT allows you to get the precision you want. Just set `perf report --itrace=i1t` for the most accurate sample rate one would ever need. Or set `perf report --itrace=i100ms` for as low as 10 samples per second. What I like about this is that it moves the overhead from the runtime into the analysis time. You just collected PT traces once and then can do all sort of complicated analysis you may need. With interrupt based sampling you need to rerun the collection every time you need to modify the parameters.

There is one interesting limitation to profiling with PT. With interrupt-based profiling you can sample on different events, say cache misses. With PT it's only possible to synthesize instruction samples since information about cache misses is not encoded in the traces.

A nice bonus for profiling with PT is that we have call stacks for free: [^LBR_stack] LBR mechanism (`perf record --call-graph lbr`) also allows capturing call stacks with each sample. However, they are only ~2-3 functions deep and incur additional overhead.

```bash
$ perf report -i pt.data -n --stdio --no-children -G
# Samples: 31K of event 'instructions:u'
# Overhead       Samples  Command  Shared Object  Symbol                                                                                                                         
# ........  ............  .......  .............  ..................................................
#
    75.16%         24045  a.out    a.out          [.] std::uniform_int_distribution<int>::operator()
            |
            ---_start
               __libc_start_main
               main
               std::uniform_int_distribution<int>::operator()

    12.50%          4000  a.out    a.out          [.] main
            |
            ---_start
               __libc_start_main
               main

    10.12%          3191  a.out    a.out          [.] goFastPath
            |
            ---_start
               __libc_start_main
               main
               goFastPath

     2.20%           695  a.out    a.out          [.] goSlowPath
            |
            ---_start
               __libc_start_main
               main
               goSlowPath
```

As you have seen, Intel PT can provide great level of details. There are many strategies how to drive your analysis using PT. For initial analysis one can start with profiling based on traditional sampling and then collect the traces for particular region of code or time period. This allows you to "zoom in" at particular execution timeframe. 

For example, to limit collecting traces for only `goSlowPath` function you can use:
```bash
$ perf record -e intel_pt//u --filter 'filter goSlowPath @ a.out' ./a.out
$ perf script ...
```

To analyze traces only during particular time frame you can use:
```bash
$ perf record -e intel_pt//u ./a.out
$ perf script --ns --itrace=i1t -F +srcline,+srccode --time 253.555413140,253.555413520 > time_range.dump
```

See more examples of using PT with Linux perf in the [part3]({{ site.url }}/blog/2019/09/06/Intel-PT-part3) of this series and in the [cheat sheet](http://halobates.de/blog/p/410) for Intel Processor Trace.









------------------------------

Perf analysis is much more fine-grained since all our traces are pricese. All the clocktick with PT analysis is precise.
In Vtune you can zoom in and filter-in. And you'll have much more detailed view on what was going on in that timeframe. In comparison with traditional sampling where you will have only few samples. You will not have representative distribution. Very usefull for glitches. 

ToDo:
explain the mechanics of profiling using traces. You can control it with --trace option.

If we increase the interrupt frequency [^int]: on my system it is default. I tried to change it by doing `echo 1000000 > /proc/sys/kernel/perf_event_max_sample_rate` but the maximum I was able to achive was around `50000` samples which is around 100MHz (100K samples/second) looks like kernel isn't happy.

In practice it will not give you any advantage over traditional interrupt-based sampling. Since it is still sampling.
But it will effectively move your overhead from runtime to analysis time (which we can consider as almost free).

PT might help when you want to have really high precision!
