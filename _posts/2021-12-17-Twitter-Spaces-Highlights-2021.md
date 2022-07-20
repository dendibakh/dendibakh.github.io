---
layout: post
title: Highlights from Twitter Spaces discussions 2021.
categories: [twitter-spaces]
---

------
**Subscribe to my [newsletter](https://products.easyperf.net/newsletter), support me on [Patreon](https://www.patreon.com/dendibakh) or by PayPal [donation](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=TBM3NW8TKTT34&currency_code=USD&source=url).**

------

In this short blog post, I decided to capture the most important highlights (for me) from all the Twitter Spaces conversations that I had during the year 2021. Some of those are not exact quotes, but rather my interpretation of their thoughts (I hope they call me out if I skrewed it up). Everything in this post is in chronological order as episodes were coming out. Recordings of all the episodes are available on my [youtube](https://www.youtube.com/playlist?list=PLRWO2AL1QAV4WK6O6cxyjWVJvbUVVLrQk) channel (sorry about the mediocre audio quality).

**Daniel Lemire:**
- *"Benchmarking should become as common as unit tests."*
- Think about SW performance from the first principles. Ask yourself: *"how fast my code COULD have run"*.

**Nadav Rotem:**
- *"Right now LLVM compiler optimizes C++ as if it was C (pointers all over the place)."* We could benefit from capturing high-level language semantics as well.

**Mark Dawson:**
- Low-latency industries (high-frequence trading, real-time robotics) fight hard the fact that most HW and SW components are built around maximizing throughput, not latency. 

**Ivica Bogosavljevic:**
- *"Memory bottlenecks are the most common performance issue in modern SW."*

**Thomas Dullien:**
- *"I predict that performance optimization and efficiency in the cloud is going to be way more important 5-10 years down the line to the point where we will eventually have the equivalent of the C-level executive in organizations that’s responsible for the efficient delivery of digital goods."*

**Tomer Morad:**
- *"The world doesn't like to do SW performance tuning"*. Systems should be able to tune themselves automatically.

**James Reinders:**
- *"Parallelism is in data."*
- A better algorithm is better than a better HW.
- *"SW libraries should be heterogeneous from the start".*
- *"We all will be heterogeneous devs in 10 years from now".*

**Arnaldo Carvalho De Melo:**
- *"eBPF is eating the world".*

**Bryan Cantrill:**
- *"People serious about SW do their own HW."*
- The future is in HW/SW co-designed open platforms.

**Andrey Akinshin:**
- There is no universal benchmarking method/approach that can be applied everywhere. Benchmarking very much depends on your business goals.

**Some of the projects that we were talking about:**

* [simdjson](https://github.com/simdjson/simdjson) - one of the fastest JSON libraries capable of parsing gigabytes of JSON per second.
* [Prodfiler](https://prodfiler.com/) - whole-system profiler that allows you to continuously monitor performance of your cloud applications.
* [Concertio](https://optimizer.concertio.com/performance/) - static and dynamic AI-powered infrastructure optimizer.
* [DPCPP](https://www.intel.com/content/www/us/en/developer/tools/oneapi/dpc-compiler.html) - standard C++ and SYCL LLVM-based cross-architecture compiler.
* [OxideComputer](https://oxide.computer/) - a startup that focuses on building hardware, with the software baked in, for running infrastructure at scale.

If you were my guest and you don't see yourself on this list, sorry, I haven't saved the notes for every episode, so it's hard for me to recover everything. Sorry about that.

---
