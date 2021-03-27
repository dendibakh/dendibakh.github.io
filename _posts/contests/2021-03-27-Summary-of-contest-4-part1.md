---
layout: post
title: Summary for performance challenge &#35&#52. Part 1.
categories: challenge
author: Ivica Bogosavljevic
---

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

