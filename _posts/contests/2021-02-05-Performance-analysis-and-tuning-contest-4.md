---
layout: post
title: Performance analysis and tuning challenge &#35&#52.
categories: contest
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

[^1]: Unfortunately, neither Denis nor Ivica work closely with Windows, so sorry, we have limited support for Windows. At least we know that it is possible to compile the source code with the MSVC compiler (19.28.29335) from Visual Studio 2019. But you need to fix cmake or add the optimizations options to the VS project yourself. We highly encourage you to contribute your changes back to the benchmark, so that other people will benefit from it.
