---
layout: post
title: Performance analysis and tuning challenge &#35&#53.
categories: challenge
author: Ivica Bogosavljevic from johnysswlab.com
---

**Contents:**
* TOC
{:toc}

Welcome to the 5th edition of our performance analysis and tuning challenge. If you haven't participated in our challenges before, we highly encourage you to read the [introductory post]({{ site.url }}/blog/2019/02/02/Performance-optimization-contest) first. 

The fifth edition of the contest will be run by Ivica Bogosavljevic from [Johny's Software Lab](https://johnysswlab.com) blog. Ivica also writes about software performance, so feel free to go and check out his blog, there is a ton of useful content there.

The benchmark for the 5th edition is KALDI. Kaldi is an open-source toolkit for speech recognition written in C++ intended to be used by researchers.

In the essence, Kaldi takes an input model and recorded speech and then it converts it to a textual representation. Speech recognition is a complex topic, but as always, we focus on hardware efficiency and general data processing efficiency, not on the speech recognition algorithm itself.

## Quickstart

Here are instructions on how to build Kaldi on Linux. To download and build kaldi for the first time do the following:

```bash
$ git clone https://github.com/kaldi-asr/kaldi.git
$ cd kaldi
$ git checkout ca6d133262aa183b23f6daba48995bd7576fb572
$ cd tools
$ extras/check_dependencies.sh
$ make -j8
$ extras/install_openblas.sh
$ cd OpenBLAS
$ make PREFIX=`pwd` install
$ cd ../../src/
$ ./configure --shared --openblas-root=../tools/OpenBLAS/
```
The configure script generates the file called `kaldi.mk`. Open the file, find the line that starts with `CXXFLAGS`, and replace default optimization level `-O1` with `-O3`,

```bash
$ make clean -j8
$ make depend -j8
$ make -j8
```

The first time you build kaldi it can take a lot of time to compile everything, so it is a good idea to let it compile and go do something else. Later, when doing incremental builds, just run `make -j8`. If you added or removed headers (changed dependencies), you should also run `make depend -j8` before `make -j8`.

## Downloading and running the benchmark

Under the assumption you are in `kaldi/src` directory, to download the test you will do the following;

```bash
$ wget https://johnysswlab.com/downloads/test-speed.zip
$ unzip test-speed.zip
```

To run the benchmark, execute:

```bash
$ cd test-speed
../online2bin/online2-wav-nnet3-latgen-faster --word-symbol-table=graph/words.txt --config=conf/model.conf am/final.mdl graph/HCLG.fst ark:test.utt2spk scp:test.scp ark:/dev/null
```

To verify the result of the change:

```bash
TODO
```

## Rules of the game

When you profile the example, you will notice that a lof time is spent in `sgemm_kernel` and `sgemm_copy` functions. Unfortunately, these functions do not belong to kaldi, they belong to OpenBLAS library and they are not the goal of optimization in this contest.

To limit the scope of the changes, you are allowed to:
* Modify only files: `lattice-faster-decoder.cc` and `lattice-faster-decoder.h`, files which are part of kaldi
* You are allowed to introduce new `.h` files, but not new `.cc` files (you will need this if you want to introduce a custom allocator or custom data structures)

You are not allowed to modify the compilation flags, modify configuration files, etc. Please note that this rule is not written in stone, if you believe that for good performance it is necessary to change other files as well, let us know and we can agree to change this rule.

This task is tough and you will want to cooperate with other participants. We created a DISCORD channel to facilitate cooperation, and we will be answering all questions there. Join the channel if you wish to cooperate. TODO Add link.

The target configuration for this challenge is TODO (Denis: let's take a newer CPU and also include for example ARM on Raspberry PI) Skylake CPU (e.g. Intel Core i7-6700) + 64-bit Linux (e.g. Ubuntu 20.04) + Clang 10. Although you are free to use whatever environment you have access to. It's fine if you solve the challenge on Intel, AMD, or ARM CPU. Also, you can do your experiments on Windows[^1] or Mac since `cmake` is used for building the benchmark. The reason why we define the target configuration is to have a unified way to assess all the submissions. In the end, it is not about getting the best score, but about practicing performance optimizations.

## Couple of hints

### The tools

To measure the program's runtime, you can prefix the command with `time` or `multitime`, like this:

```bash
multitime -n 5 ../online2bin/online2-wav-nnet3-latgen-faster --word-symbol-table=graph/words.txt --config=conf/model.conf am/final.mdl graph/HCLG.fst ark:test.utt2spk scp:test.scp ark:/dev/null
```

You compare the runtime of your modification against the runtime of the original code. The faster your program, the better you are at optimizing.

To find the hotspots (i.e. functions that take the most time), you will need to use the profiler. There are many profilers available: `perf` which is very common on Linux or Intel's VTune which works on Intel's chips only. Intel's VTune is a GUI, it is very useful and convenient both for beginners and experts. Alternatively, you can use `perf` in combination with `speedscope`, as described [here](https://johnysswlab.com/speedscope-visualize-what-your-program-is-doing-and-where-it-is-spending-time/).

After having found the functions that are performance bottlenecks, the fun starts. One part of your work is to make sure that the CPU doesn't do unnecessary work, or think of ways to skip unnecessary work. This automatically translates to better performance since the number of instructions is lower.

Another important way of making code run faster is to perform CPU-specific optimizations. The profiler tells you that the function is slow, but it doesn't tell you why. Are these data cache misses, branch prediction misses, instruction cache misses? To find out, you will need to use a special profiler that can read hardware performance counters and tell you that.

Profilers that can read this kind of data are CPU-specific since each vendor and CPU family has a different set of counters. On Intel, you can use Intel's VTune Microarchitectural Analysis to get this information. Also, `pmu-tools` is another alternative (excellent information in Denis' [book](https://book.easyperf.net/perf_book)). On AMD, there is a profiler called uProf.

Alternatively, you can use `likwid-perfctr` to read information from the hardware performance timers. This tool is open source and works with many types of CPUs from different vendors. Link to more information about `likwid-perfctr` is [here](https://johnysswlab.com/hardware-performance-counters-the-easy-way-quickstart-likwid-perfctr/).

### What to measure?

The tools can read many data from the hardware performance counters, but most of it is not interesting to you.

As you will see later, the code we are investigating mostly suffers from data cache misses. Important measurements about this type of code include:
* Cycles count: number of cycles CPU used to execute your code. This corresponds to execution time. You want to see a decrease in cycle count for your program because the program then runs faster. 
* Instructions count: number of executed instructions. You will see, when you do your changes, this number can go up or down. But, sometimes even with an increase in instruction count, your program may still run faster.
* Cycles per Instructions (Instructions per Cycle): modern CPUs can execute more than one instruction per cycle. In the case of a memory-bound program, CPI will typically be high, because the CPU often needs to wait for the data from the main memory and the CPU is not doing anything useful at that time. Pay attention to CPI: the smaller the number, the more efficient the code.

Additional metrics:
* Data cache misses: smaller the miss count, the better the performance. There are L1, L2, and L3 data caches, you should focus on L3 first since data cache misses are most expensive there.
* Memory accesses: the number of times the CPU had to do memory access is the number of times it didn't find the data it needed in the data caches.

Please note that these metrics are correlated with one another in strange ways. Every time you do some modifications to the code, the instruction count goes up or down. If the change is useful, data cache misses might go down together with cycles count, or instructions count might go down together with cycles count.

### The memory allocation problem

One of the things that will keep popping up in the profiler is the amount of time spent in `malloc` and `free`. Kaldi code extensively uses linked lists, for each node in the linked list there is a call to `malloc` and this puts huge pressure on the system allocator.

One way to resolve this is to replace the system allocator with a faster one. You can try it to see how it goes (you can use `LD_PRELOAD` to replace the system allocator as explained [here](https://johnysswlab.com/the-price-of-dynamic-memory-allocation/)). The consequence is that the program runs faster since (a) allocation is faster so it spends less time allocating memory and (b) data locality is better so there is a smaller amount of data cache misses.

Nevertheless, we are not going to replace the system allocator since Kaldi is many times used as a library and libraries should not change the system allocator.

### Custom allocator

As you will see later, two classes called `Token` and `ForwardingLink` get allocated a lot. You can use a custom allocator to allocate the instances of those classes. An example of a custom allocator can be found [here](https://github.com/moya-lang/Allocator).

Custom allocator has for one type has several benefits: it decreases data fragmentation and increases data locality. You can find more information about custom allocators [here](https://johnysswlab.com/the-price-of-dynamic-memory-allocation/).

### Increasing data locality

An important task you need to solve as part of this challenge is to increase data locality and therefore decrease data cache miss rate. Increasing data locality (which you can measure by observing the data cache hit rate) will result in a performance increase, under the assumption you are not executing much more instructions to achieve better data locality.

### Decrease structure size

In the case of our classes `Token` and `ForwardingLink`, the smaller the size of the class, the better the performance of the program. There are several ways to decrease the size of a class, here are a few ideas:
* Remove rarely used data fields to other classes.
* Get rid of pointers. Many memory-bound programs run faster if the program is compiled for a 32-bit architecture because these architecture use smaller pointers. A way to decrease the size of a pointer on a 64-bit machine is not to use them at all, instead, you can allocate from an array and keep an index into an array instead of a pointer. For example, you can use `std::vector` for memory allocation and keep the index in the array instead of a pointer.
* Use smaller types.

### Use a custom linked list

The main problem with traversing a linked list is data cache misses. Every usage of the `->` operator typically results in a data cache miss. 

Linked lists are very flexible data structures, but this flexibility comes at the price of slow access. Traversing a vector can be an order of magnitude faster than traversing a linked list of the same size. So most of the optimizations related to linked lists actually revolve around implementing them using a vector.

If you are ready to give up on some of the flexibility offered by a linked list, you can benefit from a huge speed improvement.

* Linked list where you push elements only at the beginning or end of the list can be efficiently implemented using vectors (example: `std::dequeue`)
* Linked list where you are pushing elements only at the beginning or end of the list, but you are removing random elements can also be implemented using vectors. In this case, when deleting, you mark the deleted elements of the linked list in the vector, and you skip over them during iteration. You will also want to implement a "compact" operation on such a data structure, which moves used elements of the vector to empty positions and restores an ordering as in a vector. After the "compact" operation, iteration through a linked list becomes as fast as an iteration through a vector.
* [Unrolled linked lists](https://en.wikipedia.org/wiki/Unrolled_linked_list): linked lists that store more than one element in a node is much more cache-friendly than linked lists that store only one. In one of our [earlier experiments](https://johnysswlab.com/make-your-programs-run-faster-by-better-using-the-data-cache/#linkedlist), traversing a linked list with two elements in a node was two times faster than traversing a regular linked list.
* Specialize for your data: if most of the linked lists in your program are really small, you can specialize for it using [small size optimizations](https://johnysswlab.com/the-price-of-dynamic-memory-allocation/#small-size-optimizations).

On the internet, you can find several implementations of linked lists that claim to be faster than regular linked lists. However, bear in mind that although this can be true for a general case, it is not necessarily so for your specific case. For example, there is an [implementation of STL](https://github.com/electronicarts/EASTL) that focuses on performance done by game developer EA. They claim that their implementation is faster than regular STL implementation, if this is the case it is up to you to discover.

### Don't process unused data

If you are traversing a linked list, and you come across a piece of data that doesn't require processing, your program lost time. The idea is not to access unused data at all.

Linked list node consists of a piece of data, and a pointer to a next node, typically called `next`. You can add another pointer, called `skip`, that you can use to skip over unused nodes. When you need the whole list, you iterate over `next`, when you need just to modify relevant parts of the list, you iterate over `skip` pointers.

### Copy data

When accessing data indirectly, using the `->` operator, you are likely to have a data cache miss. One of the ways to fix this is to keep a copy of data in the same class, in order to avoid using the `->` operator and additional cost. 

But bear in mind that if you have two copies of the data, and the data is changed in one place, you will need to update copies everywhere. Therefore, this approach works for data that once set, doesn't change.

### Prefetch data

An important technique for speeding up accesses is data prefetching. Many CPUs have prefetch instructions, which you can use to tell the CPU to load the data from the main memory to the cache memory.

If you know that you are going to need a certain piece of the data, you can use `__builtin_prefetch` on GCC and CLANG to request it, so it is already in the cache when you access it. Denis gave an explanation of this technique in his [book](https://book.easyperf.net/perf_book). Prefetching can be used to implement very fast data structures, as explained [here](https://johnysswlab.com/use-explicit-data-prefetching-to-faster-process-your-data-structure/).

### Reuse data in the data cache

Every piece of data accessed by your program will be loaded to the data cache and it will stay there for some time. If you need to access the same data again, it is better to do it sooner than later.

For example, if you have N-linked lists that you are accessing M times, you can try different access patterns: access them in groups of 5 lists 5 times, and then move on to the next list. When you are processing data in this way, the data needs to be loaded to the data cache only the first time, the access for the remaining four times is much faster. 

### General Recommendations

I also have a few general hints:
- **Do not try to understand the whole algorithm**. For some people, it's crucial to understand how every piece of code works. For the purposes of optimizing it will be wasted effort. There are CPU benchmarks with thousands LOC (like [SPEC2017](http://spec.org/cpu2017/)) it's absolutely impossible to understand them in a reasonable time. What you need to familiarize yourself with, are hotspots. That's it. You most likely need to understand one function/loop which is not more than 100 LOC.
- **You have a specific workload for which you optimize the benchmark**. You don't need to optimize it for any other input/workload. The main principle behind [Data-oriented design](https://en.wikipedia.org/wiki/Data-oriented_design) is that you know the data of your application.

If you feel you're stuck, don't hesitate to ask questions or look for support on our discord (TODO link). Me and other participants will do our best to answer your questions.

### Validation

The results must match the baseline. There shouldn't be any memory leaks (check with gperftools). To compare to the baseline, run:

TODO

### Submissions

> We will not use submissions for any commercial purposes. However, a good and maintainable solution can be merged back to Kaldi source tree.

The baseline we will be measuring against is TODO (Which CPU. Maybe add ARM Raspberry Pi as well) with 64-bit Linux and Clang 10 compiler used with options `-ffast-math -O3 -march=core-avx2`.

We conduct performance challenges via Denis' mailing list, so it's a good idea to [subscribe](https://mailchi.mp/4eb73720aafe/easyperf) (if you haven't already) if you would like to submit your solution. The benchmark consists of a single file, so you can just send the modified `canny_source.c` source file via email to [Ivica](https://johnysswlab.com/contact/) or [Denis](https://easyperf.net/contact/). The general rules and guidelines for submissions are described [here]({{ site.url }}/blog/2019/02/02/Performance-optimization-contest#q7-how-should-the-submission-look-like). We also ask you to provide a textual description of all the transformations you have made. It will be much easier for us to analyze your submission. 

**We are collecting submissions until 2nd August 2021.**

### Spread the word

If you know someone who might be interested in participating in this challenge, please spread the word about it!

Good luck and have fun!

__P.S.__ I'm also open to your comments and suggestions. Especially if you have a proposal of a benchmark for the next edition of the challenge, please let me know. Finding a good benchmark isn't easy.

[^1]: Haven't yet tested it on Windows.
