g++ -O2 -std=c++11 -c main.c
g++ -O2 -std=c++11 -c switch.c
g++ -O2 main.o switch.o -o test
time -p ./test default
time -p ./test spec

perf stat -r 5 ./test default
perf stat -r 5 ./test spec

#perf record ./test default
#perf record ./test spec
