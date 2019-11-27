#include <random>
#include <array>

unsigned foo1(unsigned N)  { return N * 113; }
unsigned foo2(unsigned N)  { return N * 117; }
unsigned foo3(unsigned N)  { return N * 119; }
unsigned foo4(unsigned N)  { return N * 123; }
unsigned foo5(unsigned N)  { return N * 129; }
unsigned foo6(unsigned N)  { return N * 131; }
unsigned foo7(unsigned N)  { return N * 133; }
unsigned foo8(unsigned N)  { return N * 137; }
unsigned foo9(unsigned N)  { return N * 143; }
unsigned foo10(unsigned N) { return N * 147; }

typedef unsigned (*funPtr)(unsigned N);  

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
      for (unsigned j = 0; j < 10000; j++) {
        //if (__builtin_expect(funPtrs[j] == foo10, 1))
        /*if (funPtrs[j] == foo10)
          result += foo10(randoms[j]);
        else
          result += funPtrs[j](randoms[j]);*/
        if (funPtrs[j] != foo10)
          result += funPtrs[j](randoms[j]);
        else
          result += foo10(randoms[j]);
    }
  }

  return result < 10;
}
