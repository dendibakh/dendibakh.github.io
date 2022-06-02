---
layout: post
title: Performance analysis and tuning challenge &#35&#54.
categories: challenge
---

**Contents:**
* TOC
{:toc}

Welcome to the 6th edition of our performance analysis and tuning challenge. If you haven't participated in our challenges before, I highly encourage you to read the [introductory post]({{ site.url }}/blog/2019/02/02/Performance-optimization-contest) first. 

The benchmark for the 6th edition is `wordcount`, which was suggested to me by Karthik Tadinada. The task is very simple: you need to split a text and count each word's frequency, then print the list sorted by the frequency of each word. It doesn't sound hard, probably an "Easy" on Leetcode, but you can be creative with implementing it. And guess what? Performance could be way better than what you would typically do at Leetcode. 

This challenge was inspired by the following [repo](https://github.com/juditacs/wordcount) on Github. If you take a look at the tables on the front page of that repo, you will see that solution in C++ is in the 4th place, while Java is a leader. I think that C++ can do much better and can gain the lead back. Just to be fair to other languages, I'm sure solutions in other languages like Rust can be improved as well.

The [previous challenge]({{ site.url }}/blog/2021/07/16/Performance-analysis-and-tuning-contest-5) ended in one of the participants submitting our [best patch](https://github.com/kaldi-asr/kaldi/pull/4645) to the Kaldi repo, which provided 13.5% speedup. Let's keep the good performance engineering going and showcase its real power!

Without further ado, here is the link to the challenge: [https://github.com/dendibakh/perf-challenge6](https://github.com/dendibakh/perf-challenge6). There you can find more instructions on how to set up the environment and build and benchmark your solution.

## Prizes

Yay! I have prizes!

- 1st place - $50
- 2nd place - $30
- 3rd place - $20

Note that to receive your prize you must have a Paypal account. Crypto transaction in the same $ equivalent is also an option. For those who are interested, I fund it from the money I get from my [Patreon](https://www.patreon.com/dendibakh) and [Github](https://github.com/sponsors/dendibakh) sponsors.

Also, to everyone in the US who would be able to break the "100 seconds" mark, I will send a signed [book](https://amzn.to/3sYxx1P) "Performance Analysis and Tuning on Modern CPUs" that I've written. Currently, the baseline code runs at 164 seconds on the `Linux+Intel` target machine and at 141 seconds[^1] on `Windows+AMD`.

## Target configuration

Target configurations for this challenge are:

**Intel + Linux:**
- Intel Core i5-8259U CPU @ 2.30GHz Base (3.80GHz Turbo),  6MB L3-cache
- 16GB RAM
- 256GB NVME INTEL SSDPEKKW256G8
- 64-bit Ubuntu 20.04
- Clang C++ compiler version 14.0.

**AMD + Win:**
- AMD Ryzen 7 3700X 8Core @ 3.60GHz Base (4.40GHz Turbo), 32MB L3-cache
- 64GB RAM
- ADATA XPG SX8200 Pro 1TB 3D NAND NVMe SSD
- Windows 11 Version 21H2
- Clang C++ compiler version 14.0.

Thanks to Mansur for providing AMD machine for the challenge.

You don't have to have those exact configurations. Feel free to use whatever machine you have access to. It's fine if you solve the challenge on another Intel, AMD, or even ARM CPUs. You can do your experiments on Mac as well. The reason why I define the target configuration is to have a unified way to assess all the submissions.

## Submissions

**I'm waiting for your submissions until June 30th 2022.**

The best score will be determined as the lowest average time measured on the two target machines.

The general rules and guidelines for submissions are described [here]({{ site.url }}/blog/2019/02/02/Performance-optimization-contest#q7-how-should-the-submission-look-like). Send your patch(es) via email to [me](https://easyperf.net/contact/) with the subject "PerfChallenge6-Solution". Do not open pull requests as other participants will see your solution.

In the end, I will write a post summarizing all the submissions. By submitting your code you're giving your consent for me to share it if needed. I also ask you to provide a textual description of all the changes you've made if they are non-trivial. It will be much easier for me to analyze your code.

## Where to get help?

If you feel you're stuck, don't hesitate to ask questions or look for support on our [discord](https://discord.gg/8fz5jz9wXj) channel. I and other participants will do our best to answer your questions. Also, it's a good idea to subscribe to my [mailing list](https://mailchi.mp/4eb73720aafe/easyperf) (if you haven't already) to get updates about the challenge and more. I also run a monthly newsletter about SW and HW performance through my mailing list.

## P.S. Spread the word

If you know someone who might be interested in participating in this challenge, please spread the word about it. Good luck and have fun!

I'm open to your comments and suggestions. Especially if you have a proposal for a benchmark for the next edition of the challenge, please let us know.

Finally, if you like such puzzles you can also check out my free online course "Performance Ninja" [here](https://github.com/dendibakh/perf-ninja). We have many small lab assignments dedicated to certain low-level performance optimization.

[^1]: This measurement was done on Linux (AMD machine is in dual boot). There must be some issue with running it on Windows, which shows 220 seconds. We are looking into this.