---
layout: post
title: Summary for performance challenge &#35&#52. Part 4.
categories: challenge
author: Ivica Bogosavljevic
---

**Contents:**
* TOC
{:toc}

Function `apply_hysteresis` consists of several loops, out of which only two have a measurable impact on performance and we will deal with them. 

### Replacing branches with arithmetics.

The first loop belongs to the class of _histogram computation loops_ and looks like this:

```cpp
for(r=0,pos=0;r<rows;r++){
  for(c=0;c<cols;c++,pos++){
    if(edge[pos] == POSSIBLE_EDGE) hist[mag[pos]]++;
  }
}
```

The above code will suffer from both a large data cache miss rate (because of the random access to array `hist`) and a large number of branch mispredictions. There isn't much we can do related to the data cache miss rate, but we can get rid of the branch misprediction very simply, by doing:

```cpp
hist[mag[pos]]+= (edge[pos] == POSSIBLE_EDGE);
```

The above code is equivalent to `if(edge[pos] == POSSIBLE_EDGE) hist[mag[pos]]++` but 1.5 times faster.

### Optimizing recursive function calls.

A second loop with a performance impact is following:

```cpp
for (r = 0, pos = 0; r<rows; r++){
  for (c = 0; c<cols; c++, pos++){
    if ((edge[pos] == POSSIBLE_EDGE) && (mag[pos] >= highthreshold)){
      edge[pos] = EDGE;
      follow_edges((edge + pos), (mag + pos), lowthreshold, cols);
    }
  }
}
```

The loop itself doesn't say a lot, so we have to look at the function `follow_edges`:

```cpp
void follow_edges(unsigned char *edgemapptr, short *edgemagptr, short lowval,
    int cols)
{
    short *tempmagptr;
    unsigned char *tempmapptr;
    int i;
    float thethresh;
    int x[8] = { 1, 1, 0, -1, -1, -1, 0, 1 },
        y[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };

    for (i = 0; i<8; i++){
        tempmapptr = edgemapptr - y[i] * cols + x[i];
        tempmagptr = edgemagptr - y[i] * cols + x[i];

        if ((*tempmapptr == POSSIBLE_EDGE) && (*tempmagptr > lowval)){
            *tempmapptr = (unsigned char)EDGE;
            follow_edges(tempmapptr, tempmagptr, lowval, cols);
        }
    }
}
```

The function is recursive. There are several optimizations that can be done on recursive functions:
* Decrease the number of parameters passed to the function
* Move read-only stack-allocated parameters to global memory
* Make the function tail-recursive (not possible in this case)

This function allocates arrays `x` and `y` in each invocation, but they are read-only. Changing the type from `int` to `static int` will decrease the size of the function's stack frame and save a few instructions.

In this code, we are changing all pixels with `POSSIBLE_EDGE` to `EDGE` if certain conditions are fulfilled. In the later loop (not shown here), all the pixels which are `POSSIBLE_EDGE` are converted into `NOEDGE`. We can do some preprocessing before we do `follow_edge` loop to decrease the number of parameters we need to pass to `follow_edge`. With the preprocessing, our two loops look like this:

```cpp
  for(r=0,pos=0;r<rows;r++){
    for(c=0;c<cols;c++,pos++){
      edge[pos] = (mag[pos] <= lowthreshold) & (edge[POS] == POSSIBLE_EDGE) ? NOEDGE : edge[pos];
    }
  }

   for(r=0,pos=0;r<rows;r++){
      for(c=0;c<cols;c++,pos++){
        if(edge[pos] == POSSIBLE_EDGE){
          edge[pos] = EDGE;
          follow_edges((edge+pos), cols);
        }
      }
   }
```

This decreases the work that needs to be done in `follow_edges` as well. 

Another idea is to convert `follow_edges` to a non-recursive function, but in this case, our contestants noticed that this didn't have any performance impact. 

A third possibility would be to inline `follow_edge` in itself once. Nobody did it so we didn't follow this through.

For this test, we used the CLANG compiler, but Intel's compiler has a nice feature where it can vectorize a function using OpenMP SIMD directive `#pragma omp declare simd`. It would be interesting to measure the performance impact of vectorization of this loop, but unfortunately, we didn't use it in our tests.

