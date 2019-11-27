clang++ -O2 -std=c++11 default.cpp -o def
clang++ -O2 -std=c++11 spec.cpp -o spec
time -p ./def
time -p ./spec
perf stat ./def
perf stat ./spec
#perf record ./def
#perf record ./spec
