---
layout: post
title: Performance analysis and tuning contest &#45&#51.
categories: contest
---

**Contents:**
* TOC
{:toc}

Welcome to the 4th edition of our performance analysis and tuning contest. If you see this post and haven't read my initial post about the [contest]({{ site.url }}/blog/2019/02/02/Performance-optimization-contest), I encourage you to read it first. 

The fourth edition of the contest will be ran by Ivica Bogosavljevic from [Johny's Software Lab](https://johnysswlab.com) blog. Ivica also writes about software performance, so feel free to go and check out his blog, there is a ton of useful content there.

------
**Subscribe to my [mailing list](https://mailchi.mp/4eb73720aafe/easyperf), support me on [Patreon](https://www.patreon.com/dendibakh) or by PayPal [donation](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=TBM3NW8TKTT34&currency_code=USD&source=url).**

------

Guest writer: [Ivica Bogosavljevic](https://johnysswlab.com/author/ibogi/)

The benchmark for the 4th edition is canny edge detection algorithm. The source code, compilation and run script as well as test image is available in the [code repository](TODO: Add link to the repository).

Canny is an image edge detector algorithm that exists for a long time. You can find more information about it in the [Wikipedia article](https://en.wikipedia.org/wiki/Canny_edge_detector), but they say an image is worth a thousand words, so here is before and after image so you can get the impression of how it works.

![](/img/posts/PerfContent/4/221575-small.jpg){: .center-image-width-40-no-block }
![](/img/posts/PerfContent/4/22157-canny.png{: .center-image-width-40-no-block }

TODO(verify image)

### Quickstart


The implementation used for this contest is available [online](http://www.math.tau.ac.il/~turkel/notes/canny_source.c). The same version (with very few changes) is available in the repository.

To download and build canny do the following:
```bash
$ TODO(how to download a single directory from github?)
$ cd canny_baseline
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_C_COMPILER=clang
$ make
```

To run the benchmark:
```bash
$ ../run-and-compare.sh
```

If the program finished correctly, and the image it produced is good, you will see information about the runtime and a message `Validation successful`.

You may also find useful Denis' python [script](https://github.com/dendibakh/dendibakh.github.io/tree/master/_posts/code/PerfContest/4/run.py) for conducting multiple experiments. See decription inside it.

We use CLANG for this experiment since it produced faster binary in my tests.

Target machine for this edition of the contest is again Skylake CPU with 64-bit Linux. Although you can do your experiments on Windows since `cmake` is used for building the benchmark. If you choose Windows as a platform, here is the article that might be helpful: [How to collect CPU performance counters on Windows?]({{ site.url }}/blog/2019/02/23/How-to-collect-performance-counters-on-Windows).

### Couple of hints

Here is the workflow that I recommend: 

1. Collect the baseline. You can use `time`, my personal preference is `multitime` which available in the repositories.
2. Find the hotspot. The default choice would be `perf record`, but again, Intel's Advisor or Intel's VTUNE profiler are my go-to choices, especially for less experienced engineers who are still trying to get a feel on performance tuning.
3. Find performance headroom in the hot code:
  * Make sure the data is accessed sequentially in the innermost loop of the hot code. If the data is accessed with a constant stride, you can try different techniques like loop exchange (exchange inner and outer loop), loop tilling, etc. to move towards sequential accesses.
  * Make sure innermost loop has the high trip count. This will allow compiler optimizations like loop unrolling, vectorization, etc. to speed up the code processing.
  * Move all loop-invariant code outside of the loop. Make innermost loop as compact as possible. 
  * Run your code through [TMAM]({{ site.url }}/blog/2019/02/09/ or Intel's VTUNE profiler to understand which line cause the CPU to get stalled (due to cache misses, branch misspredictions, etc.)
4. Fix the issue, build the benchmark, run it and compare against baseline.
5. Repeat steps 2-5.

Canny is a typical image processing algorithm that runs through the image, sometimes row-wise, sometimes column-wise, and processes pixels. Processing is done in several stages. Collecting the performance profile will help you focus on the right functions; collecting information about stalled cycles will help you understand why that code is slow.

I also have a few general advises:
- **Do not try to understand the whole algorithm**. For some people it's crucial to understand how every peace of code works. For the purposes of optimizing it will be wasted effort. There are CPU benchmarks with thousands LOC (like [SPEC2017](http://spec.org/cpu2017/)) it's absolutely impossible to understand them in a reasonable time. What you need to familiarize yourself with are hotspots. That's it. You most likely need to understand one function/loop which is not more than 100 LOC.
- **You have specific workload for which you optimize the benchmark**. You don't need to optimize it for any other input/workload. The main principle behind [Data-oriented design](https://en.wikipedia.org/wiki/Data-oriented_design) is that you know the data of your application.

If you feel you're stuck, don't hesitate to ask questions or look for support elsewhere. I don't have much time to answer every question promptly, but I will do my best. You can send questions to me directly using the [contact form on my web site](https://johnysswlab.com/contact/) or to Denis (TODO: Add link)

__See the Q&A post about what optimizations are [allowed]({{ site.url }}/blog/2019/02/02/Performance-optimization-contest#q5-what-optimizations-are-allowed) and what [not]({{ site.url }}/blog/2019/02/02/Performance-optimization-contest#q6-whats-not-allowed).__

### Validation

If the produced image is correct it will print `Validation successful`. A slight tolerance between the reference output image and the image produced by your algorithm is allowed in order to fully exploit the hardaware's resources.

### Submissions

> __Disclaimer__: We will not use submissions for any commercial purposes. However, we can use the submissions for educational purposes.

The baseline that I will be measuring against is 'clang -ffast-math -O3 -march=core-avx2').

If you're willing to submit your work __subscribe to Denis' [mailing list]({{ page.url }}#mc_embed_signup)__ and then send all that you have via email.

__See the rules and guidelines for submissions [here]({{ site.url }}/blog/2019/02/02/Performance-optimization-contest#q7-how-should-the-submission-look-like).__

If you are in a position of writing article with description of your findings, I highly encourage you to do so. It will be much better to have the author describe the finding in comparison with me interpreting your submission. 

**I'm collecting all your submissions until 28th February 2021.**

### Spread the word

If you know someone who might be interesting in participating in this contest, please spread the word about it!

Good luck and have fun!

__P.S.__ I'm also open to your comments and suggestions. Especially if you have an proposal of a benchmark for the next edition of the contest, please let me know. Finding a good benchmark ain't easy.
