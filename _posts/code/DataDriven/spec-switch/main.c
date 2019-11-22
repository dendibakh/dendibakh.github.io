// The code is taken from
// https://github.com/mknejp/computed-goto/blob/master/benchmarks/trivial.cpp

#include <algorithm>
#include <cstdint>
#include <random>
#include <vector>
#include <string>

  enum bytecode : int8_t
  {
    add1,
    add2,
    add3,
    add5,
    add7,

    sub1,
    sub2,
    sub3,
    sub5,
    sub7,

    mul2,
    mul3,
    mul5,

    div2,
    div3,
    div5,

    halt,
  };

  // benchmarked functions
  float run_with_switch_loop(bytecode const* instructions);
  float run_specialized_switch_loop(bytecode const* instructions);

  // create the load for the benchmark
  std::vector<bytecode> create_instructions(std::size_t count, std::uint_fast64_t seed = std::mt19937_64::default_seed);

// benchmark driver
int main(int argc, char** argv) {
  auto const instructions = create_instructions(static_cast<std::size_t>(10000));   
  if (argc == 2) {
    if (std::string(argv[1]) == "default")
      for (int i = 0; i < 100000; ++i)
        run_with_switch_loop(instructions.data());
    else if (std::string(argv[1]) == "spec")
      for (int i = 0; i < 100000; ++i)
        run_specialized_switch_loop(instructions.data());
  }
}
