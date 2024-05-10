---
layout: post
title: Thread Count Scaling Part 1. Introduction
categories: [performance analysis, book chapters]
---

------

**Subscribe to my [newsletter](https://products.easyperf.net/newsletter), support me on [Patreon](https://www.patreon.com/dendibakh), [Github](https://github.com/sponsors/dendibakh), or by PayPal [donation](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=TBM3NW8TKTT34&currency_code=USD&source=url).**

------

<span style="background-color: #fff9ae">
*I would love to hear your feedback!*

<span style="background-color: #fff9ae">
*I wrote this blog series for the second edition of my book titled "Performance Analysis and Tuning on Modern CPUs". It is open-sourced on Github: [perf-book](https://github.com/dendibakh/perf-book). The book primarily targets mainstream C and C++ developers who want to learn low-level performance engineering, but devs in other languages may also find a lot of useful information.*
</span>

<span style="background-color: #fff9ae">
*After you finish, let me know which parts you find useful/boring/complicated, which parts need better explanation, etc. Write a comment or send me an email, which you can find [here]({{ site.url }}/about_me).*
</span>

<span style="background-color: #fff9ae">
*Please keep in mind that it is an excerpt from the book, so some phrases may sound too formal. Also, in the original chapter, there is a preface to this content, where I talk about Amdahl's law, Universal Scalability Law, parallel efficiency metrics, etc. But I'm sure you guys don't need that. :)*
</span>

<br/>

Estimated reading time: 25 mins. TLDR; jump straight to the [summary]({{ site.url }}/blog/2024/05/10/Thread-Count-Scaling-Part5).

- Part 1: Introduction (this article).
- Part 2: [Blender and Clang]({{ site.url }}/blog/2024/05/10/Thread-Count-Scaling-Part2).
- Part 3: [Zstd]({{ site.url }}/blog/2024/05/10/Thread-Count-Scaling-Part3).
- Part 4: [CloverLeaf and CPython]({{ site.url }}/blog/2024/05/10/Thread-Count-Scaling-Part4).
- Part 5: [Summary]({{ site.url }}/blog/2024/05/10/Thread-Count-Scaling-Part5).

## Thread Count Scaling Case Study

Thread count scaling is perhaps the most valuable analysis you can perform on a multithreaded application. It shows how well the application can utilize modern multicore systems. As you will see, there is a ton of information you can learn along the way. Without further introduction, let's get started. 

In this case study, we will analyze the thread count scaling of the following benchmarks, some of which should be already familiar to you from the previous chapters:

1. [Blender 3.4](https://download.blender.org/release), an open-source 3D creation and modeling software project. This test is of Blender's Cycles performance with the BMW27 blend file. Command line: `./blender -b bmw27_cpu.blend -noaudio --enable-autoexec -o output.test -x 1 -F JPEG -f 1 -t N`, where `N` is the number of threads. 
2. [Clang 17](https://www.llvm.org) self-build, this test uses clang 17 to build the clang 17 compiler from sources. Command line: `ninja -jN clang`, where `N` is the number of threads.
3. [Zstandard v1.5.5](https://github.com/facebook/zstd), a fast lossless compression algorithm. A dataset used for compression: [silesia.tar](http://wanos.co/assets/silesia.tar). Command line: `./zstd -TN -3 -f -- silesia.tar`, where `N` is the number of compression worker threads.
4. [CloverLeaf 2018](http://uk-mac.github.io/CloverLeaf), a Lagrangian-Eulerian hydrodynamics benchmark. This test uses the input file `clover_bm.in`. Command line: `export OMP_NUM_THREADS=N; ./clover_leaf`, where `N` is the number of threads.
5. [CPython 3.12](https://github.com/python/cpython), a reference implementation of the Python programming language. We run a simple multithreaded binary search script written in Python, which searches `10'000` random numbers (needles) in a sorted list of `1'000'000` elements (haystack). Command line: `./python3 binary_search.py N`, where `N` is the number of threads. Needles are divided equally between threads.

The benchmarks were executed on a machine with the configuration shown below:

* 12th Gen Alderlake Intel(R) Core(TM) i7-1260P CPU @ 2.10GHz (4.70GHz Turbo), 4P+8E cores, 18MB L3-cache.
* 16 GB RAM, DDR4 @ 2400 MT/s.
* Clang 15 compiler with the following options: `-O3 -march=core-avx2`.
* 256GB NVMe PCIe M.2 SSD.
* 64-bit Ubuntu 22.04.1 LTS (Jammy Jellyfish, Linux kernel 6.5).

This is clearly not the top-of-the-line hardware setup, but rather a mainstream computer, not necessarily designed to handle media, developer, or HPC workloads. However, for our case study, it is an excellent platform to demonstrate the various effects of thread count scaling. Because of the limited resources, applications start to hit performance roadblocks even with a small number of threads. Keep in mind, that on better hardware, the scaling results will be different.

Our processor has four P-cores and eight E-cores. P-cores are SMT-enabled, which means the total number of threads on this platform is sixteen. By default, the Linux scheduler will first try to use idle physical P-cores. The first four threads will utilize four threads on four idle P-cores. When they are fully utilized, it will start to schedule threads on E-cores. So, the next eight threads will be scheduled on eight E-cores. Finally, the remaining four threads will be scheduled on the 4 sibling SMT threads of P-cores. We've also run the benchmarks while affinitizing threads using the aforementioned scheme, except `Zstd` and `CPython`. Running without affinity does a better job of representing real-world scenarios, however, thread affinity makes thread count scaling analysis cleaner. Since performance numbers were very similar, in this case study we present the results when thread affinity is used.

The benchmarks do a fixed amount of work. The number of retired instructions is almost identical regardless of the thread count. In all of them, the largest portion of an algorithm is implemented using a divide-and-conquer paradigm, where work is split into equal parts, and each part can be processed independently. In theory, this allows applications to scale well with the number of cores. However, in practice, the scaling is often far from optimal. 

Figure 1 shows the thread count scalability of the selected benchmarks. The x-axis represents the number of threads, and the y-axis shows the speedup relative to the single-threaded execution. The speedup is calculated as the execution time of the single-threaded execution divided by the execution time of the multi-threaded execution. The higher the speedup, the better the application scales with the number of threads. 

*I suggest to open this image in a separate tab as we will get back to it several times.*

<br/>

| [![](/img/posts/ThreadCountScaling/ScalabilityMainChart.png){: .center-image-width-100 }]({{ site.url }}/img/posts/ThreadCountScaling/ScalabilityMainChart.png) | 
|:--:| 
| *Figure 1. Thread Count Scalability chart for five selected benchmarks. (clickable)* |

<br/>

As you can see, most of them are very far from the linear scaling, which is quite disappointing. The benchmark with the best scaling in this case study, Blender, achieves only 6x speedup while using 16x threads. CPython, for example, enjoys no thread count scaling at all. Performance of Clang and Zstd suddenly degrades when the number of threads goes beyond 11. To understand this and other issues, let's dive into the details of each benchmark.

`->` [part 2]({{ site.url }}/blog/2024/05/10/Thread-Count-Scaling-Part2)