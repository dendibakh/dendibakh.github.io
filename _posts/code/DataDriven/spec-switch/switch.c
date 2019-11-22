// The code is taken from
// https://github.com/mknejp/computed-goto/blob/master/benchmarks/trivial.cpp

#include <algorithm>
#include <cstdint>
#include <random>
#include <vector>

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

  float run_with_switch_loop(bytecode const* instructions)
  {
    for(auto value = 0.0;;)
    {
      bytecode instr = *instructions;
      instructions++;
      switch(instr)
      {
        case bytecode::add1:
          value += 1.0;
          break;
        case bytecode::add2:
          value += 2.0;
          break;
        case bytecode::add3:
          value += 3.0;
          break;
        case bytecode::add5:
          value += 5.0;
          break;
        case bytecode::add7:
          value += 7.0;
          break;
        case bytecode::sub1:
          value -= 1.0;
          break;
        case bytecode::sub2:
          value -= 2.0;
          break;
        case bytecode::sub3:
          value -= 3.0;
          break;
        case bytecode::sub5:
          value -= 5.0;
          break;
        case bytecode::sub7:
          value -= 7.0;
          break;
        case bytecode::mul2:
          value *= 2.0;
          break;
        case bytecode::mul3:
          value *= 3.0;
          break;
        case bytecode::mul5:
          value *= 5.0;
          break;
        case bytecode::div2:
          value /= 2.0;
          break;
        case bytecode::div3:
          value /= 3.0;
          break;
        case bytecode::div5:
          value /= 5.0;
          break;
        case bytecode::halt:
          return value;
      }
    }
  }

  float run_specialized_switch_loop(bytecode const* instructions)
  {
    for(auto value = 0.0;;)
    {
      bytecode instr = *instructions;
      instructions++;
      if (instr == bytecode::add7) {
        value += 7.0;
      } else {
        switch(instr)
        {
          case bytecode::add1:
          value += 1.0;
          break;
        case bytecode::add2:
          value += 2.0;
          break;
        case bytecode::add3:
          value += 3.0;
          break;
        case bytecode::add5:
          value += 5.0;
          break;
        case bytecode::add7:
          value += 7.0;
          break;
        case bytecode::sub1:
          value -= 1.0;
          break;
        case bytecode::sub2:
          value -= 2.0;
          break;
        case bytecode::sub3:
          value -= 3.0;
          break;
        case bytecode::sub5:
          value -= 5.0;
          break;
        case bytecode::sub7:
          value -= 7.0;
          break;
        case bytecode::mul2:
          value *= 2.0;
          break;
        case bytecode::mul3:
          value *= 3.0;
          break;
        case bytecode::mul5:
          value *= 5.0;
          break;
        case bytecode::div2:
          value /= 2.0;
          break;
        case bytecode::div3:
          value /= 3.0;
          break;
        case bytecode::div5:
          value /= 5.0;
          break;
        case bytecode::halt:
          return value;
      }
     }
    }
  }

  std::vector<bytecode> create_instructions(std::size_t count, std::uint_fast64_t seed = std::mt19937_64::default_seed)
  {
    auto instructions = std::vector<bytecode>();
    instructions.reserve(count);

    std::size_t p10 = count / 10;  // 10%
    std::size_t p90 = count - p10; // 90%
    auto urng = std::mt19937_64(seed);
    auto dist = std::uniform_int_distribution<int>(0, static_cast<int>(bytecode::halt) - 1);
    std::generate_n(std::back_inserter(instructions), p10, [&] { return static_cast<bytecode>(dist(urng)); });
    std::generate_n(std::back_inserter(instructions), p90 - 1, [&] { return bytecode::add7; });
    std::shuffle(std::begin(instructions), std::end(instructions), urng);
    instructions.push_back(bytecode::halt);
    return instructions;
  }

