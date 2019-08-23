### Collecting PT while sampling

Perf analysis is much more fine-grained since all our traces are pricese. We didn't missed a sample. All the clocktick with PT analysis is precise.
In Vtune you can zoom in and filter-in. And you'll have much more detailed view on what was going on in that timeframe. In comparison with traditional sampling where you will have only few samples. You will not have representative distribution. Very usefull for glitches. 

For initial analysis one can start with traditional sampling and then zoom in using PT.
