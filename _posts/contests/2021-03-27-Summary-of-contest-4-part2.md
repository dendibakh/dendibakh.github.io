---
layout: post
title: Summary for performance challenge &#35&#52. Part 2.
categories: challenge
author: Ivica Bogosavljevic from johnysswlab.com
---

**Contents:**
* TOC
{:toc}

Function `derivative_x_y` also consists of two loops that are processing the image. The first loop nest processes the image row-wise and the second column-wise.

As far as performance is concerned, the first loop nest has a sequential memory access pattern and from the performance point of view, there is nothing that needs to be done there. CLANG auto-vectorized the loop nest, and from the performance point of view, nothing needs to be done there. You can click [here](https://github.com/dendibakh/perf_challenge4/blob/master/canny_baseline/canny_source.c#L389) to see the source code of the first loop nest.

### Loop interchange for achieving sequential memory accesses.

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

The last step is to interchange the loops over `c` and loops over `r` and with this, the accesses to arrays `delta_y` and `smoothedim` become sequential.

One contestant noticed that it is convenient to keep the arrays `delta_x` and `delta_y` as positive values, since this simplifies the processing in the function `non_max_supp`. A simple `abs(smoothedim[index+cols] - smoothedim[index-cols])` while processing the data in the loop does the trick.
