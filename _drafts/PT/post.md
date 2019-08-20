Many ways to do performance analysis with Intel Processor Traces.

Describe how traditional sampling/instrumentation works.

For a long time I wanted to write this post. Today I want to present a revolutionary technology that allows doing performance analysis without interrupting the running SW. I must say that for most of my tasks Intel PT is rather an overkill and for major part of my daily work traditional approach works just fine. But I recall a number of cases where using Intel PT would expedite the analysis and exactly one case where I did use it. :)

So what is Intel PT? It is a CPU tracing feature which records the program execution by encoding packets in a very compressed format. It has extensive coverage and relatively small `~5%` overhead. It's main goals are to make postmortem analysis and root causing performance glitches. 

### Workflow

Similar to sampling techniques it does not require any modifications to the source code. All you need to collect traces is just to run the program under PT enabled tool. Once PT are enabled SW tools start writing packets to DRAM. Similar to [LBR](link), Intel PT works by recording branches. For example:

Create a picture:

push
mov
je
mov
add
cmp
je .label
mov
.label:
call (edx) // virtual function call

PT trace:
Not taken
Taken
Address

Info collected by SW decoder. IP and others. Decoder need binary file, in order to reconstruct the execution flow of the program. Very powerful for debugging.

Create a picture: Highlight executed instructions.

So we can reconstruct exact instruction flow.

Static events in the program (like an unconditional branches) are simply ignored, because it's obvious where the program will go. PT feature encodes conditional branches using 1 bit: 1taken/not taken`. In case of indirect branches or calls address is recorded.
Certain special mode changes are also captured (read more elsewhere).

Typically it's around 0.6 bytes per instruction. So if we have 1B instructions, we would have ...

Also we can filter IP ranges, so there are things we can opt-in and opt-out of tracing to limit the bandwith. This allows us to trace just a single function or a single loop.

### Timings

Not only flow of execution can be traced but also timings. Cycle count info. But also time stamp information which allows to align the event with some other event in the system and easy to compare to wall clock time. All timings are sent in a separate packets.

PT will bundle up to 6 conditional branches before it will emit a timing packet. As you can see instruction data is perfectly accurate but timing information is less accurate.

Can be configured to update timings every cycle. But likley it will not increase our accuracy greatly, since timings will be send only for conditional branches.

**picture**

### Usages

Here are some the cases when PT can be a freat use:

1. **Postmortem analysis**. Provides the call stack information which is always valid even if the stack is corrupted.
2. **Analyze performance glitches** which are not statistically significant.
3. **Having better accuracy when profiling**. PT can be a provider for profiling tools.
4. **Introspect execution of the programm**. For example:
* We can immideatly tell if some code path was never executed. Sort of prove theory quickly.
* How much time we spent waiting spinning on a lock attempt.

Be prepared to have at least 100 MB/s of encoded data. When decoded it might easily be 10 times more (~1GB/s). So typical use case might not be similar to profiling. But rather attaching just for the period of when the glitch happend. (circular buufer)

On BDW we had only time stamp information, but since Skylake & Goldmont every packet has also cycle count from the previous packet.

### Postmortem debugging 

**Come up with an example**

Can get an instruction history listing. Dump control flow trace and see what instructions lead you here.
Reverse step.

### Analyzing glitches

### Using PT for profiling

**compare perf record with -e intel_pt**

### Collecting PT while sampling

Perf analysis is much more fine-grained since all our traces are pricese. We didn't missed a sample. All the clocktick with PT analysis is precise.
In Vtune you can zoom in and filter-in. And you'll have much more detailed view on what was going on in that timeframe. In comparison with traditional sampling where you will have only few samples. You will not have representative distribution. Very usefull for glitches. 

For initial analysis you can start with traditional sampling and then zoom in using PT.

### Comparison with Intel LBR

We do not trace program on a BB basis. This would increase overhead much. BB are usually across 5 instructions.

LBR can only get timing for taken branches while PT encodes conditional branches regardlessly.

This technology does not depend on the interupts (PMI) which other PerfMon features (like PEBS and LBR) do. Much more useful for real-time systems, because you don't need to interupt the CPU to get out the data. This is definetly a shift from using PMI tracing into something new.

Power event trace.

### References and links

There is similar kind of technique used in Windows ETW traces which can be used for analyzing performance glitches. Bruce Dawson has a lot of interesting articles on his [blog](https://randomascii.wordpress.com/). He has ETW traces turned on `24/7` on his machines which allows him to capture what was going on in the last minute or so. By analyzing those postmortem traces he was able to root cause many interesting bugs including [this](https://randomascii.wordpress.com/2012/09/04/windows-slowdown-investigated-and-identified/), [this](https://randomascii.wordpress.com/2016/03/08/power-wastage-on-an-idle-laptop/) or [this](https://randomascii.wordpress.com/2018/08/16/24-core-cpu-and-i-cant-type-an-email-part-one/).



To add:

Overhead for compute-bound application might come from lots of branches -> more data to log. Overhead for memory-bound application might come from the fact that PT pushes a lot of data to DRAM.

It is great for analyzing cold paths.

The only things that is missing is printf style debugging. This is probably the only significant reason why you would want to instrument your code manually.

Put the link for library for collecting traces. Will be useful in Windows context.
