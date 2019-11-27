#include <random>
#include <array>

// sort of a weighted sum of surrounding neighbors.
unsigned foo(std::array<unsigned, 10000>::iterator I, unsigned coeff)  { 
  unsigned weight = 0;
  const unsigned N = 16;
  std::prev(I, N/2);
  #pragma clang vectorize(disable) unroll(enable)
  for (unsigned i = 0; i < N; ++i) {
    weight += *I * coeff;
    ++I;
  }
  return weight;
}

unsigned foo1(std::array<unsigned, 10000>::iterator I)  { return foo(I, 113); }
unsigned foo2(std::array<unsigned, 10000>::iterator I)  { return foo(I, 117); }
unsigned foo3(std::array<unsigned, 10000>::iterator I)  { return foo(I, 119); }
unsigned foo4(std::array<unsigned, 10000>::iterator I)  { return foo(I, 123); }
unsigned foo5(std::array<unsigned, 10000>::iterator I)  { return foo(I, 129); }
unsigned foo6(std::array<unsigned, 10000>::iterator I)  { return foo(I, 131); }
unsigned foo7(std::array<unsigned, 10000>::iterator I)  { return foo(I, 133); }
unsigned foo8(std::array<unsigned, 10000>::iterator I)  { return foo(I, 137); }
unsigned foo9(std::array<unsigned, 10000>::iterator I)  { return foo(I, 143); }
unsigned foo10(std::array<unsigned, 10000>::iterator I) { return foo(I, 147); }

typedef unsigned (*funPtr)(std::array<unsigned, 10000>::iterator I);  

funPtr getFunPtr(unsigned N) {
  switch (N) {
    case 1: return foo1;
    case 2: return foo2;
    case 3: return foo3;
    case 4: return foo4;
    case 5: return foo5;
    case 6: return foo6;
    case 7: return foo7;
    case 8: return foo8;
    case 9: return foo9;
    default: return foo10; // takes 80% of all indirect calls
  }
}

int main() {
  const unsigned min = 0;
  const unsigned max = 49;
  std::default_random_engine generator;
  std::uniform_int_distribution<unsigned> distribution(min,max);

  std::array<unsigned, 10000> randoms;
  std::array<funPtr, 10000> funPtrs;
  for (unsigned i = 0; i < 10000; i++) {
    randoms[i] = distribution(generator);
    funPtrs[i] = getFunPtr(randoms[i]);
  }
  
  unsigned result = 0;
  for (unsigned i = 0; i < 100000; i++) {
      for (unsigned j = 10; j < 9990; j++) {        
       if (funPtrs[j] != foo10)
          result += funPtrs[j](randoms.begin() + j);
       else
          result += foo10(randoms.begin() + j);
    }
  }

  return result < 10;
}
