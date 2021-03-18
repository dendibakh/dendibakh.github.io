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

## _Updated TODO(Add date)_

TODO(Write an introduction about the submission. Put the link to the youtube channel)

### Observations

The profiler showed four functions dominating the profile: `gaussian_smooth`, `derivative_x_y`, `non_max_supp` and `apply_hysteresis`. Let's dig into the function, see what they are doing and what our contestants did to make them faster.

**gaussian_smooth**

The function `gaussian_smooth` (TODO: Add code to the baseline in the repository) consists of two loop nests. Here is the source code of the first loop nest:

```cpp
for(r=0;r<rows;r++){
  for(c=0;c<cols;c++){
      dot = 0.0;
      sum = 0.0;
      for(cc=(-center);cc<=center;cc++){
        if(((c+cc) >= 0) && ((c+cc) < cols)){
            dot += (float)image[r*cols+(c+cc)] * kernel[center+cc];
            sum += kernel[center+cc];
        }
      }
      tempim[r*cols+c] = dot/sum;
  }
}
```

The access pattern for the arrays in the innermost loop is good, it is always the sequential pattern. Everytime the loop counter `cc` in the innermost loop increases by one, the index into the `image` and `kernel` increases by one.

The problem is the trip count of the innermost loop: it is low. Vectorization doesn't pay off in that case.

One of the proposed solution was to multiversion a loop. In our case, `center` had value 2. We can create two versions of the loop nest, one where the value of `center` is known at compile time, and another one where it isn't. The compilers are very efficient at unrolling the loop with compile-time known trip count. Here is the solution:

```cpp
if (center == 2) {
  for(r=0;r<rows;r++){
    for(c=0;c<cols;c++){
        dot = 0.0;
        sum = 0.0;
        for(cc=(-2);cc<=2;cc++){
          if(((c+cc) >= 0) && ((c+cc) < cols)){
              dot += (float)image[r*cols+(c+cc)] * kernel[center+cc];
              sum += kernel[center+cc];
          }
        }
        tempim[r*cols+c] = dot/sum;
    }
  }
} else {
  for(r=0;r<rows;r++){
    for(c=0;c<cols;c++){
        dot = 0.0;
        sum = 0.0;
        for(cc=(-center);cc<=center;cc++){
          if(((c+cc) >= 0) && ((c+cc) < cols)){
              dot += (float)image[r*cols+(c+cc)] * kernel[center+cc];
              sum += kernel[center+cc];
          }
        }
        tempim[r*cols+c] = dot/sum;
    }
  }
}
```

The above approach is quite crude, we could have used C macros to achieve similar things with less copying. BUt the basic idea is there.

Another approach is to do the loop interchange. If we could exchange the loop over `cc` and loop over `c`, we would get the innermost loop with a high trip count. Loop interchange is possible if two loops are perfectly nested, which is not our case. However, with a trick they can become perfectly nested.

```cpp
for(r=0;r<rows;r++){
  for(c=0;c<cols;c++){
      dot[c] = 0.0;
      sum[c] = 0.0;
      for(cc=(-2);cc<=2;cc++){
        if(((c+cc) >= 0) && ((c+cc) < cols)){
            dot[c] += (float)image[r*cols+(c+cc)] * kernel[center+cc];
            sum[c] += kernel[center+cc];
        }
      }
      tempim[r*cols+c] = dot[c]/sum[c];
  }
}
```

We converted temporary values `dot` and `sum` into arrays. By doing this, their values are preserved accross loop iterations. This allows us to break the loop over `c` into two loops:

```cpp
for(r=0;r<rows;r++){
  for(c=0;c<cols;c++){
      dot[c] = 0.0;
      sum[c] = 0.0;
  }
  for(c=0;c<cols;c++){
      for(cc=(-2);cc<=2;cc++){
        if(((c+cc) >= 0) && ((c+cc) < cols)){
            dot[c] += (float)image[r*cols+(c+cc)] * kernel[center+cc];
            sum[c] += kernel[center+cc];
        }
      }
  }
  for(c=0;c<cols;c++){
      tempim[r*cols+c] = dot[c]/sum[c];
  }
}
```

Loop over `c` and loop over `cc` are now perfectly nested and they can be interchanged.

The second loop nest looks similar to the first one:

```cpp
for(c=0;c<cols;c++){
  for(r=0;r<rows;r++){
      sum = 0.0;
      dot = 0.0;
      for(rr=(-center);rr<=center;rr++){
        if(((r+rr) >= 0) && ((r+rr) < rows)){
            dot += tempim[(r+rr)*cols+c] * kernel[center+rr];
            sum += kernel[center+rr];
        }
      }
      (*smoothedim)[r*cols+c] = (short int)(dot*BOOSTBLURFACTOR/sum + 0.5);
  }
}
```

The access pattern in the innermost loop is bad this time. Access to the array `tempim` is with a stride `cols`. Everytime the value of `rr` increases by one we are accessing the elements which is `cols` places away from the previous element. This access pattern is bad from the performance point of view. To get rid of it, we would need to do a complex loop interchange to move the loop over `c` to the innermost position.

To do this, we first interchange the loop over `c` and the loop over `r`. Then we interchange the loop over `c` and loop over `rr`. The final solution looks like this:

```cpp
for (r = 0; r<rows; r++){
  for(c=0;c<cols;c++){
      dot[c] = 0.0;
      sum[c] = 0.0;
  }
  for (rr = (-center); rr <= center; rr++){
    if (((r + rr) >= 0) ＆＆ ((r + rr) < rows)){
      for (c = 0; c<cols; c++){
        dot[c] += tempim[(r + rr) * cols + c] * kernel[center + rr];
        sum[c] += kernel[center + rr];
      }
    }
  }
  for (c = 0; c < cols; c++) {
    (*smoothedim)[r*cols + c] = (short int)(dot[c]*BOOSTBLURFACTOR / sum[c] + 0.5);
  }
}
```

One of the contestants also noted that the `sum[c]` has a fixed value `1` for all the pixels except the pixels at the edge of the image. This would allow us to get rid of the expensive division when `sum[c]` is `1` in the calculation of values for `smoothedim`.

And finally, note that we are allocating a temporary image `tempim` for our calculation. One of the contestants managed to merge the above two loops nests into one loop nest, thus avoiding the need to allocate a huge buffer.

**derivative_x_y**

Function `derivative_x_y` also consists of two loops that are processing the image. The first loop nest processes the image row-wise and the second column-wise.

As far as performance is concerned, the first loop nest has a sequential memory access pattern and from the performance point of view there is nothing that needs to be done there. CLANG auto-vectorized the loop nest, and from the performance point of view, nothing needs to be done there. You can click here TODO(Add link) to see the source code of the first loop nest.

The second loop nest is more problematic. We are giving its source code here:

```cpp
for(c=0;c<cols;c++){
  pos = c;
  (*delta_y)[pos] = smoothedim[pos+cols] - smoothedim[pos];
  pos += cols;
  for(r=1;r<(rows-1);r++,pos+=cols){
      (*delta_y)[pos] = smoothedim[pos+cols] - smoothedim[pos-cols];
  }
  (*delta_y)[pos] = smoothedim[pos] - smoothedim[pos-cols];
}
```

In the innermost loop (over r) you can see the expression `(*delta_y)[pos] = smoothedim[pos+cols] - smoothedim[pos-cols]`. The variable `pos` is increased by `cols`, which represents inefficient strided memory access.

We could try loop interchange here is well, by interchanging the loop over `c` and loop over `r`. But to do that, first we need to get rid of induction variable `pos`. We want to be able to calculate index into arrays for each iteration that depends only on `c` and `r`. Here is how the rewrite goes:

```cpp
int index;
for(c=0;c<cols;c++){
  (*delta_y)[c] = smoothedim[c+cols] - smoothedim[c];
  for(r=1;r<(rows-1);r++,pos+=cols){
      index = c + r * cols;
      (*delta_y)[index] = smoothedim[index+cols] - smoothedim[index-cols];
  }
  index = c + (rows - 1) * cols
  (*delta_y)[index] = smoothedim[index] - smoothedim[index];
}
```

To make the loop over `c` and loop over `r` perfectly nested, we have get rid of the lines that are preventing the nesting. In this case, all the iterations of the loop are independent, and this can be achieved easily. We move the epilogue and prologue of the loop ver `r` into separate loops. The transformed code looks like this:

```cpp
int index;
for(c=0;c<cols;c++){
  (*delta_y)[c] = smoothedim[c+cols] - smoothedim[c];
  index = c + (rows - 1) * cols
  (*delta_y)[index] = smoothedim[index] - smoothedim[index];
}
for(c=0;c<cols;c++){
  for(r=1;r<(rows-1);r++,pos+=cols){
      index = c + r * cols;
      (*delta_y)[index] = smoothedim[index+cols] - smoothedim[index-cols];
  }
}
```

The last step is to interchange the loops over `c` and loops over `r` and with this, the accesses to arrays `delta_y` and `smoothedim` becomes sequential.

One contestant noticed that it is convenient to keep the arrays `delta_x` and `delta_y` as positive values, since this simplifies the processing in the function `non_max_supp`. A simple `abs(smoothedim[index+cols] - smoothedim[index-cols])` while processing the data in the loop does the trick.

**non_max_supp**

TODO
* Remove useless code (replace division by multiplication)
* Move to completely branchless

**apply_hysteresis**

TODO:
* Break dependency chain in histogram computation
* Do some preprocessing in follow_edges to make it more lightweight
* Decrease the number of parameters passed in follow edge
* Ideas not tested: move to non-recursive follow-edges, inline follow_edges two times, rewrite using intrinsics

**Other ideas**

TODO:
* Reuse buffers
* Use mmap to allocate large chunks of memory and preallocate the memory using MAP_RESERVE
* Memory-mapped files
---

[^1]: Unfortunately, neither Denis nor Ivica work closely with Windows, so sorry, we have limited support for Windows. At least we know that it is possible to compile the source code with the MSVC compiler (19.28.29335) from Visual Studio 2019. But you need to fix cmake or add the optimizations options to the VS project yourself. We highly encourage you to contribute your changes back to the benchmark, so that other people will benefit from it.
