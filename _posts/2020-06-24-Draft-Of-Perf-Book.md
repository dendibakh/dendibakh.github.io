---
layout: post
title: Draft of my perf book is ready!
categories: [performance analysis]
---

------
**Subscribe to my [mailing list](https://mailchi.mp/4eb73720aafe/easyperf), support me on [Patreon](https://www.patreon.com/dendibakh) or by PayPal [donation](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=TBM3NW8TKTT34&currency_code=USD&source=url).**

------

**It has been a long journey!** I was silent for a while, haven't posted regularly on my blog. But don't worry, I'm fine. Instead, I took this situation around coronavirus and focused on writing a book "*Performance Analysis and Tuning on Modern CPU*". I started writing this book almost a year ago, so I'm happy I finally can show something to the people. Right now, the **early draft is ready and I'm welcoming everybody to review the book and maybe even add something to it**. I know a lot of people are struggling right now, so I decided to make the book __FREE__ for all. Eventually, everyone will be able to download PDF version of it.

__Why I started it?__ I started this book with one simple goal in mind: educate developers to better understand performance of their applications running on modern HW. Most of developers are used to look no further than the source code of their application without trying to understand performance implications of the changes they make. I know for many of us what happens under the hood is sort of black matter. I know it's hard, but hopefully with this book, performance world of modern HW will become more accessible.

__What is the book about?__ Have you ever debated with a coworker about performance of a certain piece of code? Then you probably know how hard it is to predict which version is going to work best. With so many moving parts inside the modern processors even a small tweak to the code can trigger significant performance change. Besides all its complexity, HW has many features that support performance analysis. That's right, CPUs are capable of telling us what performance bottlenecks are and where they occur. Thus, the book focuses on how your code looks like from the CPU perspective and provides recipes for HW specific optimizations. The core of the book is centered around how to find the right place in the code to improve performance.

Below I present the sweetest parts of the book contents:
```
1. Introduction
2. Measuring Performance
3. CPU Microarchitecture 101
4. Terminology And Metrics In Performance Analysis
5. Performance Analysis Approaches
  5.1. Code Instrumentation
  5.2. Tracing
  5.3. Workload Characterization
  5.4. Sampling
  5.5. Static Performance Analysis
  5.6. Compiler Optimization Reports
6. CPU Features For Performance Analysis
  6.1. Top-Down Microarchitecture Analysis (TMAM)
  6.2. Last Branch Record (LBR)
  6.3. Processor Event-Based Sampling (PEBS)
  6.4. Intel Processor Traces (PT)
7. Source Code Tuning For CPU
  7.1. Data-Driven Optimizations
  7.2. CPU Front-End Optimizations
  7.3. CPU Back-End Optimizations
  7.4. Optimizing Bad Speculation
  7.5. Other Tuning Areas
8. Optimizing Multithreaded Applications
```

__Who is this book for?__ Primarily, this book is written for developers working in performance critical projects. This includes the following areas: High performance computing (HPC), High Frequency Trading (HFT), GameDev, Data Centers (Facebook, Google, etc), and other areas. But I hope it will be useful for any C++ developer: you can use this book to learn performance analysis which will stack up nicely with other skills you might have. All examples in the book are written in C/C++, but to the large extent they are applicable to any native programming language (like C, C++, Rust, Go and even Fortran). While this book is fairly low-level, I hope, it will be accessible even to developers that are just starting performance related work.

I gave this book all my knowledge and experience and I'm eager to share it with community. I decided to make this book open to everyone and **I welcome everyone to review and contribute to the book**. I hope, this way we can combine best performance practices and expertise from people in different areas. 

**Shoot me an email or leave a comment if you are interested.**

