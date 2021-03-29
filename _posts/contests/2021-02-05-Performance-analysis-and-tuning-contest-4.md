---
layout: post
title: Performance analysis and tuning challenge &#35&#52.
categories: challenge
author: Ivica Bogosavljevic
---

**Contents:**
* TOC
{:toc}

Welcome to the 4th edition of our performance analysis and tuning challenge. If you haven't participated in our challenges before, we highly encourage you to read the [introductory post]({{ site.url }}/blog/2019/02/02/Performance-optimization-contest) first. 

The fourth edition of the contest will be run by Ivica Bogosavljevic from [Johny's Software Lab](https://johnysswlab.com) blog. Ivica also writes about software performance, so feel free to go and check out his blog, there is a ton of useful content there.

The benchmark for the 4th edition is canny edge detection algorithm. The source code, compilation, and run script as well as the test image are available in Denis' github [repo](https://github.com/dendibakh/perf_challenge4).

Canny is an image edge detector algorithm that exists for a long time. You can find more information about it in the [Wikipedia article](https://en.wikipedia.org/wiki/Canny_edge_detector). They say an image is worth a thousand words, so here is the before and after image so you can get the impression of how it works.

![](/img/posts/PerfContest/4/221575-small.jpg){: .center-image-width-40-no-block } ![](/img/posts/PerfContest/4/22157-canny.png){: .center-image-width-40-no-block }

### Quickstart

The implementation used for the challenge is available [online](http://www.math.tau.ac.il/~turkel/notes/canny_source.c). The same version (with very few changes) is available in the repository.

To download and build canny do the following:
```bash
$ git clone https://github.com/dendibakh/perf_challenge4.git
$ cd perf_challenge4
$ cd canny_baseline
$ mkdir build
$ cd build
# cmake also honors the following env variables:
# export CC=/usr/bin/clang
# export CXX=/usr/bin/clang++
$ cmake .. -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
$ make
```

To run the benchmark:
```bash
$ ../run-and-compare.sh
```

If the program finished correctly, and the image it produced is good, you will see information about the runtime and a message `Validation successful`.

You may also find useful Denis' python [script](https://github.com/dendibakh/perf_challenge4/blob/master/run.py) for conducting multiple experiments. See the decription inside it.

The target configuration for this challenge is Skylake CPU (e.g. Intel Core i7-6700) + 64-bit Linux (e.g. Ubuntu 20.04) + Clang 10. Although you are free to use whatever environment you have access to. It's fine if you solve the challenge on Intel, AMD, or ARM CPU. Also, you can do your experiments on Windows[^1] or Mac since `cmake` is used for building the benchmark. The reason why we define the target configuration is to have a unified way to assess all the submissions. In the end, it is not about getting the best score, but about practicing performance optimizations.

### Couple of hints

Here is the workflow that I recommend: 

1. Collect the baseline. You can use `time`, my personal preference is `multitime` which available in the repositories.
2. Find the hotspot. The default choice would be `perf record`, but again, Intel's Advisor or Intel's VTune profiler are my go-to choices, especially for less experienced engineers who are still trying to get a feel on performance tuning.
3. Find performance headroom in the hot code:
  * Make sure the data is accessed sequentially in the innermost loop of the hot code. If the data is accessed with a constant stride, you can try different techniques like loop exchange (exchange inner and outer loop), loop tilling, etc. to move towards sequential accesses.
  * Make sure the innermost loop has a high trip count. This will allow compiler optimizations like loop unrolling, vectorization, etc. to speed up the code processing.
  * Move all loop-invariant code outside of the loop. Make the innermost loop as compact as possible. 
  * Run your code through the [TMA]({{ site.url }}/blog/2019/02/09/Top-Down-performance-analysis-methodology) or Intel's VTune profiler to understand which line of code causes the CPU to get stalled (due to cache misses, branch mispredictions, etc.)
4. Fix the issue, build the benchmark, run it and compare against the baseline.
5. Repeat steps 2-5.

Canny is a typical image processing algorithm that runs through the image, sometimes row-wise, sometimes column-wise, and processes pixels. Processing is done in several stages. Collecting the performance profile will help you focus on the right functions; collecting information about stalled cycles will help you understand why that code is slow.

I also have a few general hints:
- **Do not try to understand the whole algorithm**. For some people, it's crucial to understand how every piece of code works. For the purposes of optimizing it will be wasted effort. There are CPU benchmarks with thousands LOC (like [SPEC2017](http://spec.org/cpu2017/)) it's absolutely impossible to understand them in a reasonable time. What you need to familiarize yourself with, are hotspots. That's it. You most likely need to understand one function/loop which is not more than 100 LOC.
- **You have a specific workload for which you optimize the benchmark**. You don't need to optimize it for any other input/workload. The main principle behind [Data-oriented design](https://en.wikipedia.org/wiki/Data-oriented_design) is that you know the data of your application.

If you feel you're stuck, don't hesitate to ask questions or look for support elsewhere. I don't have much time to answer every question promptly, but I will do my best. You can send questions to me directly using the [contact form on my web site](https://johnysswlab.com/contact/) or to [Denis](https://easyperf.net/contact/).

__See the Q&A post about what optimizations are [allowed]({{ site.url }}/blog/2019/02/02/Performance-optimization-contest#q5-what-optimizations-are-allowed) and what [not]({{ site.url }}/blog/2019/02/02/Performance-optimization-contest#q6-whats-not-allowed).__

### Validation

If the produced image is correct it will print `Validation successful`. A slight tolerance between the reference output image and the image produced by your algorithm is allowed in order to fully exploit the hardware's resources.

### Submissions

> We will not use submissions for any commercial purposes. However, we can use the submissions for educational purposes.

The baseline we will be measuring against is Skylake client CPU (e.g. Intel Core i7-6700) with 64-bit Linux and Clang 10 compiler used with options `-ffast-math -O3 -march=core-avx2`.

We conduct performance challenges via Denis' mailing list, so it's a good idea to [subscribe](https://mailchi.mp/4eb73720aafe/easyperf) (if you haven't already) if you would like to submit your solution. The benchmark consists of a single file, so you can just send the modified `canny_source.c` source file via email to [Ivica](https://johnysswlab.com/contact/) or [Denis](https://easyperf.net/contact/). The general rules and guidelines for submissions are described [here]({{ site.url }}/blog/2019/02/02/Performance-optimization-contest#q7-how-should-the-submission-look-like). We also ask you to provide textual description of all the transformations you have made. It will be much easier for us to analyze your submission. 

**We are collecting submissions until 28th February 2021.**

### Spread the word

If you know someone who might be interested in participating in this challenge, please spread the word about it!

Good luck and have fun!

__P.S.__ I'm also open to your comments and suggestions. Especially if you have a proposal of a benchmark for the next edition of the challenge, please let me know. Finding a good benchmark isn't easy.

---

## _Updated 27th March 2021_

TODO(Write an introduction about the submission.)
Video version of the summary for the challenge is available on [youtube](https://www.youtube.com/watch?v=xvnFfl9LctM).

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
