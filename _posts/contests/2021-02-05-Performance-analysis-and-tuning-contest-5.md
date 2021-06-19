---
layout: post
title: Performance analysis and tuning challenge &#35&#52.
categories: challenge
author: Ivica Bogosavljevic from johnysswlab.com
---

**Contents:**
* TOC
{:toc}

Welcome to the 5th edition of our performance analysis and tuning challenge. If you haven't participated in our challenges before, we highly encourage you to read the [introductory post]({{ site.url }}/blog/2019/02/02/Performance-optimization-contest) first. 

The fifth edition of the contest will be run by Ivica Bogosavljevic from [Johny's Software Lab](https://johnysswlab.com) blog. Ivica also writes about software performance, so feel free to go and check out his blog, there is a ton of useful content there.

The benchmark for the 5th edition is KALDI. Kaldi is an open-source toolkit for speech recognition written in C++ intended to be used by speech recognition researchers. Kaldi is intended for use by speech recognition researchers.

In the essence, Kaldi takes as an input a model and recorded speech and then it converts it to textual information. Speech recognition is a complex topic, but as always, we focus on hardware efficiency and general data processing efficiency, not on speech recognition algorithms.

## Quickstart

The instructions here are instructions on how to build and run test of Kaldi on Linux.


To download and build kaldi for the first time do the following:
```bash
$ git clone https://github.com/kaldi-asr/kaldi.git
4 cd kaldi
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

The first time you build kaldi it can take a lot of time to compile everything. Later, when doing incremental builds, just run `make -j8`. If your added or removed headers (changed dependencies), you should also run `make depend -j8` before `make -j8`.

## Downloading and running the test

Under the assumption you are in `kaldi/src` directory, to download the test you will do the following;

```bash
wget https://johnysswlab.com/downloads/test-speed.zip
unzip test-speed.zip
cd test-speed
TODO finish
```

To run the test, execute:

```bash
TODO Add
```

TO verify the result of the change:

```bash
TODO
```

## Rules of the game

When you profile the example, you will notice that a lof time is spent in `sgemm_kernel` and `sgemm_copy` functions. Unfortunatelly, these functions do not belong to kaldi, they belong to OpenBLAS library and they are not the goal of optimization in this contest.

To limit the scope of the changes, you are allowed to:
* Modify only files: `lattice-faster-decoder.cc` and `lattice-faster-decoder.h`, files which are part of kaldi
* You are allowed to introduce new `.h` files, but not new `.cc` files (you will need this if you want to introduce a custom allocator or custom data structures)

You are not allowed to modify the compilation flags, modify configuration files, etc. Please note that this rule is not written in stone, if you believe that for good performance it is necessary to change other files as well, let us know and we can agree to change this rule.


The target configuration for this challenge is TODO Skylake CPU (e.g. Intel Core i7-6700) + 64-bit Linux (e.g. Ubuntu 20.04) + Clang 10. Although you are free to use whatever environment you have access to. It's fine if you solve the challenge on Intel, AMD, or ARM CPU. Also, you can do your experiments on Windows[^1] or Mac since `cmake` is used for building the benchmark. The reason why we define the target configuration is to have a unified way to assess all the submissions. In the end, it is not about getting the best score, but about practicing performance optimizations.

## Couple of hints

### The tools

1. Collect the baseline. You can use `time`, my personal preference is `multitime` which available in the repositories.
2. Find the hotspot. The default choice would be `perf record`, but again, Intel's Advisor or Intel's VTune profiler are my go-to choices, especially for less experienced engineers who are still trying to get a feel on performance tuning. In Denis' book "Performance Tuning on Modern CPUs" there is a really nice explanation on how to use `pmu-tools`.
TODO finish talk about the tools


### The memory allocation problem

One of the things that will keep popping up in the profiler is the amount of time spent in `malloc` and `free`. Kaldi code extensively uses linked lists, for each node in the linked list there is a call to `malloc` and this puts a huge pressure on the system allocator.

One way to resolve this is to replace the system allocator with a faster one. You can try it to see how it goes (you can use `LD_PRELOAD` to replace the system allocator as explained [here](https://johnysswlab.com/the-price-of-dynamic-memory-allocation/)). The consequence is that the program runs faster since (a) allocation is faster so it spends less time allocating memory and (b) data locality is better so there is a smaller amount of data cache misses.

Nevertheless, we are not going to replace the system allocator since Kaldi is many times used as a library and libraries should not change the system allocator.

### Custom allocator

As you will see later, two classes called `Token` and `ForwardingLink` get allocated a lot. You can use a custom allocator to allocate the instances of those classes. An example of a custom allocator can be found [here](https://github.com/moya-lang/Allocator).

Custom allocator has for one type has several benefits: it decreases data fragmentation and increases data locality. You can find more information about custom allocators [here](https://johnysswlab.com/the-price-of-dynamic-memory-allocation/).

### Increasing data locality

An important task you need to solve as part of this challenge is to increase data locality and therefore decrease data cache miss rate. Increasing data locality (which you can measure by observing the data cache hit rate) will result in performance increase, under the assumption you are not executing much more instructions to achieve better data locality.

### Decrease structure size

In case of our classes `Token` and `ForwardingLink`, the smaller the size of the class, the better the performance of the program. There are several ways to decrease the size of a class, here are a few ideas:
* Remove rarely used data fields to other classes.
* Get rid of pointers. Many memory bound programs run faster if the program is compiled for a 32 bit architecture, because these architecture use smaller pointers. A way to decrease the size of a pointer on a 64 bit machine is not to used them at all, instead you can allocate from an array and keep an index into array instead of pointer. For example, you can use `std::vector` for memory allocation and keep the index in the array instead of pointer.
* Use smaller types.

### Use custom linked list

The main problem with traversing a linked list are data cache misses. Every usage of `->` operator typically results in a data cache miss. 

Linked lists are very flexible data structures, but this flexibility comes at the price of slow access. Traversing a vector can be an order of a magnitude faster than traversing a linked list of the same size. So most of the optimizations related to linked lists actually revolve around implementing them using a vector.

If you are ready to give up on some of the flexibility offered by a linked list, you can benefit from a huge speed improvements.

* Linked list where you push elements only at the beginning or end of the list can be efficiently implemented using vectors (example: `std::dequeue`)
* Linked list where you are pushing elements only at the beginning or end of the list, but you are removing random elements can also be implemented using vectors. In this case, when deleting, you mark the deleted elements of the linked list in the vactor, and you skip over them during iteration. You will also want to implement a "compact" operation on such a data structure, which moves used elements of the vector to empty positions and restores an ordering as in a vector. After "compact" operation, iteration through linked list becomes as fast as iteration through a vector.
* [Unrolled linked lists](https://en.wikipedia.org/wiki/Unrolled_linked_list): linked lists that store more than one element in a node are much more cache friendly that linked lists that store only one. In one of our [earlier experiments](https://johnysswlab.com/make-your-programs-run-faster-by-better-using-the-data-cache/#linkedlist), traversing a linked list with two elements in a node was two times faster than traversing a regular linked list.
* Specialize for your data: if most of the linked lists in your program are really small, you can specialize for it using [small size optimizations](https://johnysswlab.com/the-price-of-dynamic-memory-allocation/#small-size-optimizations).

On the internet you can find several implementations of linked lists that claim to be faster than regular linked lists. However, bear in mind that although this can be true for a general case, it is not necessarily so four your specific case. For example, there is an [implementation of STL](https://github.com/electronicarts/EASTL) that focuses on performance done by game  developer EA. They claim that their implementation is faster than regular STL implementation, if this is the case it is up to you to discover.

### Don't process unused data

If you are traversing a linked list, and you come accross a piece of data that doesn't require processing, your program lost time. The idea is not to access unused data at all.

Linked list node consists of a piece of data, and a pointer to a next node, typicall called `next`. You can add another pointer, called `skip`, that you can use to skip over unused nodes. When you need the whole list, you iterate over `next`, when you need just to modify relevant parts of the list, you iterate over `skip` pointers.

### Copy data

When accessing data indirectly, using `->` operator, you are likely to have a data cache miss. One of the ways to fiy this is to keep a copy a data in the same class, in order to avoid using the `->` operator and additional cost. 

But bear in mind that if you have two copies of the data, and the data is changed in one place, you will need to update copies everywhere. Therefore, this approach works for data that once set, doesn't change.

### Prefetch data

An important technique for speeding up accesses is data prefetching. Many CPUs have prefetch instruction, which you can use to tell the CPU to load the data from the main memory to the cache memory.

If you know that you are going to need a certain piece of the data, you can use `__builtin_prefetch` on GCC and CLANG to request it, so it is already in the cache when you access it. Denis gave an explanation of this technique in his book (TODO: Add link). Prefetching can be used to implement very fast data structures, as explained [here](https://johnysswlab.com/use-explicit-data-prefetching-to-faster-process-your-data-structure/).

### General Recommendations

I also have a few general hints:
- **Do not try to understand the whole algorithm**. For some people, it's crucial to understand how every piece of code works. For the purposes of optimizing it will be wasted effort. There are CPU benchmarks with thousands LOC (like [SPEC2017](http://spec.org/cpu2017/)) it's absolutely impossible to understand them in a reasonable time. What you need to familiarize yourself with, are hotspots. That's it. You most likely need to understand one function/loop which is not more than 100 LOC.
- **You have a specific workload for which you optimize the benchmark**. You don't need to optimize it for any other input/workload. The main principle behind [Data-oriented design](https://en.wikipedia.org/wiki/Data-oriented_design) is that you know the data of your application.

If you feel you're stuck, don't hesitate to ask questions or look for support elsewhere. I don't have much time to answer every question promptly, but I will do my best. You can send questions to me directly using the [contact form on my web site](https://johnysswlab.com/contact/) or to [Denis](https://easyperf.net/contact/).

__See the Q&A post about what optimizations are [allowed]({{ site.url }}/blog/2019/02/02/Performance-optimization-contest#q5-what-optimizations-are-allowed) and what [not]({{ site.url }}/blog/2019/02/02/Performance-optimization-contest#q6-whats-not-allowed).__

### Validation

TODO

### Submissions

> We will not use submissions for any commercial purposes. However, a good and maintainable solution can be merged back to Kaldi source tree.

The baseline we will be measuring against is TODO (Which CPU. Maybe add ARM Raspberry Pi as well) with 64-bit Linux and Clang 10 compiler used with options `-ffast-math -O3 -march=core-avx2`.

We conduct performance challenges via Denis' mailing list, so it's a good idea to [subscribe](https://mailchi.mp/4eb73720aafe/easyperf) (if you haven't already) if you would like to submit your solution. The benchmark consists of a single file, so you can just send the modified `canny_source.c` source file via email to [Ivica](https://johnysswlab.com/contact/) or [Denis](https://easyperf.net/contact/). The general rules and guidelines for submissions are described [here]({{ site.url }}/blog/2019/02/02/Performance-optimization-contest#q7-how-should-the-submission-look-like). We also ask you to provide textual description of all the transformations you have made. It will be much easier for us to analyze your submission. 

**We are collecting submissions until 28th February 2021.**

### Spread the word

If you know someone who might be interested in participating in this challenge, please spread the word about it!

Good luck and have fun!

__P.S.__ I'm also open to your comments and suggestions. Especially if you have a proposal of a benchmark for the next edition of the challenge, please let me know. Finding a good benchmark isn't easy.

---

## _Updated 27th March 2021_

Our contestants have really put quite an effort in the challenge. After the challenge we organized a follow-up sessions, where we discussed the changes contestants made, their experiences and the performance gains. Video version of the summary for the challenge is available on [youtube](https://www.youtube.com/watch?v=xvnFfl9LctM).

### Observations

The profiler showed four functions dominating the profile: `gaussian_smooth`, `derivative_x_y`, `non_max_supp` and `apply_hysteresis`. It is possible to speed up each one of them. We break down our summary into 4 parts which dig into a particular function and show what it is doing and what our contestants did to make them faster.
- [Part 1]({{ site.url }}/blog/2021/03/27/Summary-of-contest-4-part1): `gaussian_smooth`: loop multiversioning, loop interchange to achieve vectorization and sequential memory accesses.
- [Part 2]({{ site.url }}/blog/2021/03/27/Summary-of-contest-4-part2): `derivative_x_y`: loop interchange for achieving sequential memory accesses.
- [Part 3]({{ site.url }}/blog/2021/03/27/Summary-of-contest-4-part3): `non_max_supp`: reducing the number of branches, replace branches with arithmetics.
- [Part 4]({{ site.url }}/blog/2021/03/27/Summary-of-contest-4-part4): `apply_hysteresis`: replacing branches with arithmetics, optimizing recursive function calls.

**Other ideas**

There were a few other ideas that didn't bring large performance improvements, nevertheless, we mention them here for completeness:
* **Buffer reuse**: the algorithm was allocating some buffers where it can reuse some of the old buffers. Buffer reuse is a clever choice, it saves memory and this is good for performance.
* **Use `mmap` to allocate large chunks of memory and preallocate the memory using MAP_POPULATE**: when you allocate memory with `malloc` or `new`, the operating system doesn't allocate all the memory in advance, because many programs allocating a lot of memory never actually use it. When your program first writes to the unallocated memory page, a pagefault happens and the operating system allocates the page that is missing. If you are allocating a large block of memory that you will write to only a few times but you are sure you will write each byte in the block, you can use `mmap` with `MAP_POPULATE` to allocate all the memory in advance, so no pagefaults happen when you are accessing the memory.

### Score Table

Here is the table that summarizes submissions that we received:
```
time(s)  submission             timings for 10 consecutive runs (s)                            speedup
([0.15, 'Andrey_Evstyukhin',    [0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.2 ]], ' + 1260.0%')
([0.29, 'Adam_Folwarczny',      [0.29, 0.29, 0.29, 0.29, 0.29, 0.3,  0.3,  0.3,  0.3,  0.33]], ' + 603.45%')
([0.4,  'Peter_Coffman',        [0.4,  0.4,  0.4,  0.4,  0.4,  0.4,  0.4,  0.4,  0.41, 0.42]], ' + 410.0%')
([0.52, 'Yiannis_Papadopoulos', [0.52, 0.52, 0.52, 0.52, 0.52, 0.52, 0.52, 0.52, 0.52, 0.56]], ' + 292.31%')
([0.58, 'Goran_Mitrovic',       [0.58, 0.58, 0.58, 0.58, 0.58, 0.58, 0.58, 0.58, 0.59, 0.62]], ' + 251.72%')
([0.62, 'Sasha_Krassovsky',     [0.62, 0.63, 0.63, 0.63, 0.63, 0.63, 0.63, 0.63, 0.63, 0.67]], ' + 229.03%')
([0.64, 'Tomas_Hudziec',        [0.64, 0.64, 0.64, 0.64, 0.64, 0.64, 0.65, 0.65, 0.65, 0.65]], ' + 218.75%')
([0.84, 'Andrey_Pechkurov',     [0.84, 0.85, 0.85, 0.85, 0.85, 0.85, 0.85, 0.85, 0.85, 0.85]], ' + 202.38%')
([1.4,  'Pradeep_Kumar',        [1.4,  1.45, 1.45, 1.46, 1.46, 1.47, 1.47, 1.48, 1.48, 1.51]], ' + 45.71%')
([2.04, 'canny_baseline',       [2.04, 2.04, 2.05, 2.06, 2.06, 2.06, 2.06, 2.06, 2.12, 2.12]], ' + 0.0%')
```

---

[^1]: Unfortunately, neither Denis nor Ivica work closely with Windows, so sorry, we have limited support for Windows. At least we know that it is possible to compile the source code with the MSVC compiler (19.28.29335) from Visual Studio 2019. But you need to fix cmake or add the optimizations options to the VS project yourself. We highly encourage you to contribute your changes back to the benchmark, so that other people will benefit from it.
