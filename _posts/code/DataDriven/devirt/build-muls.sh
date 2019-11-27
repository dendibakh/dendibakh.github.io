clang++ -O2 -std=c++11 default_muls.cpp -o def_muls
clang++ -O2 -std=c++11 spec_muls.cpp -o spec_muls
time -p ./def_muls
time -p ./spec_muls
#perf stat ./def_muls
#perf stat ./spec_muls
#perf record ./def_muls
#perf record ./spec_muls
