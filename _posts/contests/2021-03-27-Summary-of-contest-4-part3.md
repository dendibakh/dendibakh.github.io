---
layout: post
title: Summary for performance challenge &#35&#52. Part 3.
categories: challenge
author: Ivica Bogosavljevic
---

**Contents:**
* TOC
{:toc}

The function `non_max_supp` was the most interesting one, at least according to me. Click [here](https://github.com/dendibakh/perf_challenge4/blob/master/canny_baseline/canny_source.c#L719) to see the full source code of the critical loop.

The loop iterates over pixels row-wise. Inside the loop, for each pixel, the code performs the same operations. But the input values for the operation depend on the complex derivative values calculated in the previous step. Depending on the value of the derivatives, the code will perform the action on one of the eight neighboring pixels. Here is the full source code of the critical loop:

```cpp
for(rowcount=1,magrowptr=mag+ncols+1,gxrowptr=gradx+ncols+1,
  gyrowptr=grady+ncols+1,resultrowptr=result+ncols+1;
  rowcount<nrows-2; 
  rowcount++,magrowptr+=ncols,gyrowptr+=ncols,gxrowptr+=ncols,
  resultrowptr+=ncols){   
  for(colcount=1,magptr=magrowptr,gxptr=gxrowptr,gyptr=gyrowptr,
      resultptr=resultrowptr;colcount<ncols-2; 
      colcount++,magptr++,gxptr++,gyptr++,resultptr++){   
      m00 = *magptr;
      if(m00 == 0){
        *resultptr = (unsigned char) NOEDGE;
      }
      else{
        xperp = -(gx = *gxptr)/((float)m00);
        yperp = (gy = *gyptr)/((float)m00);
      }

      if(gx >= 0) {
        if(gy >= 0){
          if (gx >= gy){  
            // calculate mag1 and mag2
          } else {    
            // calculate mag1 and mag2
          }
        } else {
          if (gx >= -gy){
            // calculate mag1 and mag2
          } else {    
            // calculate mag1 and mag2
          }
        }
      } else {
        if ((gy = *gyptr) >= 0) {
          if (-gx >= gy) { 
            // calculate mag1 and mag2
          } else {
            // calculate mag1 and mag2
          }
        } else {
          if (-gx > -gy) {
            // calculate mag1 and mag2
          } else {
            // calculate mag1 and mag2
          }
        }
      } 

        /* Now determine if the current point is a maximum point */
        if ((mag1 > 0.0) || (mag2 > 0.0))
        {
            *resultptr = (unsigned char) NOEDGE;
        }
        else
        {    
            if (mag2 == 0.0)
                *resultptr = (unsigned char) NOEDGE;
            else
                *resultptr = (unsigned char) POSSIBLE_EDGE;
        }
    }
  }
```

The above code is quite verbose but can be basically divided into three parts. The first part loads the common values `m0`, `gx`, `gy`, `xperp`, and `yperp`. The second part on lines [736-871](https://github.com/dendibakh/perf_challenge4/blob/master/canny_baseline/canny_source.c#L736) performs the calculation depending on the values of `gx` and `gy`. The third part writes into the resulting array depending on the calculated values `mag1` and `mag2`.

This nested loop suffers from a large number of branch mispredictions, which can be confirmed with `perf`. Any attempt in reducing the number of branches will result in better performance. Branches in this case prevent auto-vectorization since compilers do not know how to deal with them.

### Reducing the number of branches

One of the way to reduce number of branches in case of nested branches is to replace nested branches with conditionally joined expression. E.g:

```cpp
if (gx > 0) {
  if (gy > 0) {
    if (gx >= 0) {
      ...
    }
  }
}
```

Can be replaced with:

```cpp
if (gx > 0 & gy > 0 & gx >= 0) {
  ...
}
```

The above three branches are not equivalent. The first example, condition `gy > 0` will be evaluated only if `gx > 0`. In the optimized version, all three conditions are evaluated before the branch is evaluated as a whole. When condition evaluation is cheap (which is normally the case with simple arithmetic conditions), the second solution will be faster. Please note that we are joining the condition with arithmetic `&`, not `&&`, since only when they are joined with `&` will they be evaluated altogether.

The above approach will decrease the number of branches, but not eliminate them. 

### Replacing branches with arithmetics.

We can go completely branchless by replacing them with arithmetics. Consider this part of the code:

```cpp
#define NOEDGE 255
#define POSSIBLE_EDGE 128

if ((mag1 > 0.0) || (mag2 > 0.0))
{
    *resultptr = (unsigned char) NOEDGE;
}
else
{    
    if (mag2 == 0.0)
        *resultptr = (unsigned char) NOEDGE;
    else
        *resultptr = (unsigned char) POSSIBLE_EDGE;
}
```

We conditionally store to `*resultptr` depending on the set of conditions. We store `NOEDGE` if condition `((mag > 0.0) || (mag2 >= 0.0))`, otherwise we store `POSSIBLE_EDGE`. Or to say it the other way around, we store `POSSIBLE_EDGE` if condition `((mag <= 0.0) & (mag2 < 0.0))` is 1. So we can write:

```cpp
`*resultptr` = POSSIBLE_EDGE *((mag <= 0.0) & (mag2 < 0.0));
```

This completely avoids branches. If we analyze the large set of nested `if`s  (link to the [source](https://github.com/dendibakh/perf_challenge4/blob/master/canny_baseline/canny_source.c#L736)), we notice a pattern. The computations are always the same, there is only a difference in which of the neighboring pixels we are processing.

The processing in each of the ifs body looks something like this:

```cpp
z1 = *(magptr - A);
z2 = *(magptr - B);

mag1 = (X1 * m00 + Y1 * z1 + Z1 * z2)*xperp + (X2 * m00 + Y2 * z1 + Z2 * z2)*yperp;

/* Right point */
z1 = *(magptr + A);
z2 = *(magptr + B);

mag1 = (X1 * m00 + Y1 * z1 + Z1 * z2)*xperp + (X2 * m00 + Y2 * z1 + Z2 * z2)*yperp;
```

Where constants `A`, `B`, `X1`, `X2`, `Y1`, `Y2`, `Z1` and `Z2` depend on conditions `gx > 0`, `gy > 0` and `gx > gy`. 

One of the ways to go branchless is to select values for the constants using a lookup table that indexes with conditions `gx > 0`, `gy > 0` or `gx > gy`. The example solutions can be found [here](https://github.com/dendibakh/perf_challenge4/blob/master/Peter_Coffman/canny_source.c#L693) or [here](https://github.com/dendibakh/perf_challenge4/blob/master/Andrey_Evstyukhin/canny_source.c#L661).

### Substitute division with multiplication

The last optimization in this function concerns the strength reduction. Here is an example:

```cpp
xperp = -(gx = *gxptr)/((float)m00);
yperp = (gy = *gyptr)/((float)m00);
...
mag1 = (m00 - z1)*xperp + (z2 - z1)*yperp;
...
mag2 = (m00 - z1)*xperp + (z2 - z1)*yperp;
...
bool condition = ((mag1 > 0.0) | (mag2 > 0.0));
```

We first divide `gx` and `gy` by `m00` to get `xperp` and `yperp` and then we calculate `mag1` and `mag2` by using `xperp` and `yperp`. Finally, we only check the sign of `mag1` and `mag2`, we are not interested in its actual value.

We can substitute the division with multiplication like this:

```cpp
mag1 = (m00 - z1)*xperp + (z2 - z1)*yperp;
// Multiply the whole expression by m00
mag1 * m00 = (m00 - z1) * xperp * m00 + (z2 - z1) * yperp * m00
mag1 * m00 = (z1 - m00) * gx + (z2 - z1) * gy 
```

From this point, our condition becomes a bit more complex:

```cpp
bool condition = (m00 < 0) ^ ((mag1 > 0.0) | (mag2 > 0.0))
```

Nevertheless, an evaluation of the condition and exclusive xor is much cheaper than the division operation.
