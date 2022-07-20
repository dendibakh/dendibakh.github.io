---
layout: post
title: Computing industry at the end of 2020 as I see it.
categories: [research]
---

------
**Subscribe to my [newsletter](https://mailchi.mp/4eb73720aafe/easyperf), support me on [Patreon](https://www.patreon.com/dendibakh) or by PayPal [donation](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=TBM3NW8TKTT34&currency_code=USD&source=url).**

------

> Disclaimer: Everything written in this post is my personal opinion and does not reflect the official position of my employer (Intel). This post does not advertise any software, hardware, or any other product.

We live in a very interesting time! I'm curious to see where the computing industry will go in the upcoming years. And this is extremely hard to predict. Tech giants collaborate with and against each other in many directions. For instance, Intel and AMD compete with each other for the best CPU, but at the same time, together they defend the X86 ecosystem against ARM that is backed by big players like Apple, Amazon, Samsung, and others. One thing I know is that we are at the beginning of the new computing era. The world progressed from the PC era to the cloud era, but now computing becomes even more distributed and heterogeneous. In this short post, I will share my humble opinion on this.

Software programmers have had an "easy ride" for decades, thanks to Moore’s law. Unfortunately, single-threaded performance growth is slowing down, which is so critical for general-purpose computing. John Hennessy said in his Google I/O 2018 [talk](https://youtu.be/Azt8Nc-mtKM): "That's the end of the general-purpose processor performance as we know it". Now it is the world of specialized HW for a particular domain. I talk a lot about that in the first chapter of my [book](https://book.easyperf.net/perf_book), so read it for an expanded discussion. According to the popular paper "There’s plenty of room at the top" by Leiserson et al., streamlined HW is the future we will face, where computing devices would be heavily customized for the work they are designed to do. Everything will look like a computer. Even simple earphones will employ a noise-canceling streaming processor, that will be crafted to do just noise-canceling and nothing else.

Something more complex than the earbuds will have multiple accelerators integrated into the single SOC. I believe, our computing world moves from the traditional CPU + GPU design towards an XPU design, where multiple types of HW are integrated into a single system. In a cloud environment, heterogeneity is already a norm, with CPU, GPU, FPGA, and other accelerators hosted under the same rack. But we also see this trend for user devices. For example, the Apple iPhone chip has a CPU, a GPU, and an AI chip integrated into a single SOC. I think this trend can be observed in the actions of the industry's major players. HW companies are broadening their portfolio to provide a wide range of HW architectures. NVidia is going after the CPU market by buying ARM, AMD acquired Xilinx, one of the major players in the FPGA world, and so on. 

There is one problem associated with it. It will be extremely hard for a casual developer to target this insane number of architectures and getting a good level of performance out of them. One of the existing solutions is CUDA which is proprietary to NVidia and only targets their HW. For developers, to be able to program for a variety of different HW architectures, we need an open standard supported by all the vendors.

Sorry for the shameless plug, but here is where Intel's [OneAPI](https://www.oneapi.com/) comes into play. This is the project that I'm working on, so I'm happy to talk about it. OneAPI is a bold idea that all the HW devices can be programmed using a single API, just like OpenCL, but much easier. OneAPI is built upon the modern C++ and Khronos SYCL standards. The core of OneAPI is the Data Parallel C++ (DPCPP) compiler, which is available on [github](https://github.com/intel/llvm). BTW, the whole project is free, you can download it and give it a try. The best part about this programming model is that it does all the heavy lifting for you, so you can focus on the algorithm, rather than on writing OpenCL calls. This post is not about OneAPI, however, let me know if you are interested in it. Maybe I can write more about the DPCPP compiler. I started working on the DPCPP compiler at the beginning of 2020, so I'm not the biggest expert here. But the whole initiative looks very promising to me.

The variety of different hardware begs for a unified programming model. Sooner or later, cloud service providers would come up with something similar, given the need to program all the different machine types in their data centers. In that sense OneAPI project is forward-looking and provides an open standard, that can be picked up by all the players. 

Speaking of data centers. Right now we see many services operate in the cloud environment. Those computations are owned by the data centers. It is very much profitable for them to know what is being done on their servers since they can make a customized solution that will be better tuned for the needs of their customers. We already see this trend with Amazon developing their Graviton processor family. Microsoft recently [announced](https://www.bloomberg.com/news/articles/2020-12-18/microsoft-is-designing-its-own-chips-for-servers-surface-pcs) that they will start making their own server chips for their data centers.

However, it's not yet clear to me where the computations would be done in the future. The current trend is to do everything in the cloud, but things can turn in the opposite direction. We could see more and more computations done "on the edge", i.e. on the user devices. The primary driving factors for that could be privacy and security concerns. But also, a simple fact that moving the data back and forth between the device and the cloud is not cheap. The growth of 5G may reduce the "cost" of data transfers, but still, we cannot do better than the speed of light.

It will be very interesting to see where the computing industry will move in the upcoming years. I would love to hear what you think. Share your thoughts in the comments.

---
