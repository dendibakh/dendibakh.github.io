---
layout: post
title: Thread Count Scaling Part 5. Summary
categories: [performance analysis, book chapters]
---

------

**Subscribe to my [newsletter](https://products.easyperf.net/newsletter), support me on [Patreon](https://www.patreon.com/dendibakh), [Github](https://github.com/sponsors/dendibakh), or by PayPal [donation](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=TBM3NW8TKTT34&currency_code=USD&source=url).**

------

<span style="background-color: #fff9ae">
*This blog is an excerpt from the book. More details in the introduction.*
</span>

- Part 1: [Introduction]({{ site.url }}/blog/2024/05/10/Thread-Count-Scaling-Part1).
- Part 2: [Blender and Clang]({{ site.url }}/blog/2024/05/10/Thread-Count-Scaling-Part2).
- Part 3: [Zstandard]({{ site.url }}/blog/2024/05/10/Thread-Count-Scaling-Part3).
- Part 4: [CloverLeaf and CPython]({{ site.url }}/blog/2024/05/10/Thread-Count-Scaling-Part4).
- Part 5: Summary (this article).

### Summary 

In the case study, we have analyzed several throughput-oriented applications with varying thread count scaling characteristics. Here is a quick summary of our findings:

* Frequency throttling is a major roadblock to achieving good thread count scaling. This affects all the benchmarks that we've analyzed. In fact, any application that makes use of multiple hardware threads suffers from frequency drop due to thermal limits. Platforms that have processors with higher TDP (Thermal Design Power) and advanced liquid cooling solutions are less prone to frequency throttling.
* Thread count scaling on hybrid processors (with performant and energy-efficient cores) is penalized because E-cores are less performant than P-cores. Once E-cores start being used, performance scaling is slowing down. Sibling SMT threads also don't provide good performance scaling. We observed these effects in Blender and Clang.
* Worker threads in a throughput-oriented workload share a common set of resources, which may become a bottleneck. As we saw in the CloverLeaf example, performance doesn't scale because of the memory bandwidth limitation. This is a common problem for many HPC and AI workloads. Once you hit that limitation, everything else becomes less important, including code optimizations and even CPU frequency. Another shared resource that often becomes a bottleneck is the L3 cache.
* Finally, performance of a concurrent application may be limited by the synchronization between threads as we saw in Zstd and CPython examples. Some programs have very complex interactions between threads, so it is very useful to visualize worker threads on a timeline. Also, you should know how to find contested locks using performance profiling tools.

To confirm that suboptimal scaling is a common case, rather than an exception, let's look at the SPEC CPU 2017 suite of benchmarks. In the *rate* part of the suite, each hardware thread runs its own single-threaded workload, so there are no slowdowns caused by thread synchronization. According to one of the MICRO 2023 keynotes[^1], benchmarks that have integer code (regular general-purpose programs) have a thread count scaling in the range of `40% - 70%`, while benchmarks that have floating-point code (scientific, media, and engineering programs) have a scaling in the range of `20% - 65%`. Those numbers represent inefficiencies caused just by the hardware platform. Inefficiencies caused by thread synchronization in multithreaded programs further degrade performance scaling.

In a latency-oriented application, you typically have a few performance-critical threads and the rest do background work that doesn't necessarily have to be fast. Many issues that we've discussed apply to latency-oriented applications as well. We covered some low-latency tuning techniques in Section 12.2.

---

[^1]: Debbie Marr, "Architecting for Power-Efficiency in General-Purpose Computing", [https://youtu.be/IktNjMxJYPE?t=2599](https://youtu.be/IktNjMxJYPE?t=2599)