---
layout: post
title: HW and SW rules of thumb.
categories: [optimizations]
---

------
**Subscribe to my [newsletter](https://products.easyperf.net/newsletter), support me on [Patreon](https://www.patreon.com/dendibakh) or by PayPal [donation](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=TBM3NW8TKTT34&currency_code=USD&source=url).**

------

Here is my curated list of HW and SW rules of thumb, fun facts and quotes about performance of the programs. It is not an April fool's day joke, at least there was no intention to fool anybody :) .

- [Basic blocks](https://en.wikipedia.org/wiki/Basic_block) are usually 5 instructions on average. [^1]
- Branches occur every 6th instruction. [^1]
- Average [ILP](https://en.wikipedia.org/wiki/Instruction-level_parallelism) (Instruction Level Parallelism) in the general-purpose SW is 2. (by Ivan Godard in one of his [talks](https://www.youtube.com/channel/UCKdGg6hZoUYnjyRUb08Kjbg) about the Mill CPU architecture).
- 90% of execution runs on 25 opcodes (by Jim Keller in this [interview](https://www.youtube.com/watch?v=Nb2tebYAaOA) timestamp: 6:20)
- 90% of the execution time of a program is spent executing 10% of the code. (source: [wikipedia](https://en.wikipedia.org/wiki/Program_optimization#Bottlenecks))
- Before 2008 all instructions were executed on the same frequency. Now technologies like [Intel Turbo Boost](https://en.wikipedia.org/wiki/Intel_Turbo_Boost) and [AMD Turbo Core](https://en.wikipedia.org/wiki/AMD_Turbo_Core) allow to change frequency on the fly.
- If cache size is doubled, miss rate usually drops by a factor of `sqrt(2)`. [^1]
- Modern CPUs can correctly predict outcomes of 99% of the branches.
- The bigger the core count of CPU the lower its frequency.
- Modern superscalar CPU tries to find parallelism dynamically, but it is limited on what it can see by its branch predictor.
- “Flat profiles are dying. Already dead for most programs. Larger and larger fraction of code runs freezingly cold, while hot spots run hotter. ... By the late 1990s for even performance sensitive code, optimizing compilers exceeded the performance of human experts. ... We come so close to optimal on most architectures that we can’t do much more without using NP complete algorithms instead of heuristics. We can only try to get little niggles here and there where the heuristics get slightly wrong answers. ... We found ourselves always running up against the same problem: the compiler needs to be in a dialog with the programmer; it needs to know properties of the data, and whether certain cases can arise, etc. And we couldn’t think of a good language in which to have such a dialog." (from "[The death of optimizing compilers](http://cr.yp.to/talks/2015.04.16/slides-djb-20150416-a4.pdf)").
- 0.75% CPU utilization is not idle system and may wake up CPU. [^1]

Other interesting rules of thumbs and quotes:
- Rob Pike's [5 Rules of Programming](https://users.ece.utexas.edu/~adnan/pike.html).
- SW Engineering Candies: [Selected rules of thumb in SW engineering](https://www.sw-engineering-candies.com/blog-1/rules-of-thumb-in-software-engineering).
- Embedded Artistry: [Embedded Rules of Thumb](https://embeddedartistry.com/blog/2018/04/26/embedded-rules-of-thumb/).

Add your favorite rules of thumb and quotes in the comments.

------

[^1]: Don't remember the source.
