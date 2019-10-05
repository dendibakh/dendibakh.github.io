set term pngcairo fontscale 2
set out "scaling.png"
set ylabel "speedup factor"
set xlabel "number of threads"
set xrange [1:8]
set yrange [1:8]

set grid xtics
set grid ytics

#set key center
plot "scaling.txt" using 1:2 w lines lw 5 ti "scaling"
