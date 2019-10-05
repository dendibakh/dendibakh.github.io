set term pngcairo fontscale 2
set out "utilization.png"
set ylabel "CPU utilization"
set xlabel "number of threads"
set xrange [1:8]
set yrange [1:8]

set grid xtics
set grid ytics

#set key center
plot "utilization.txt" using 1:2 with lines lw 5 ti "utilization"
