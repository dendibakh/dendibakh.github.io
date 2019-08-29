#include <random>

int foo(unsigned int arg) 
{
  int x = 0;
  //int x = someComplexFunc(arg);
  //log("Enter calculations");
  switch (arg)
  {
    case 7498536:
      {
        int y = 7498536 / x; // potential div by zero
        x = y - 42;
        break;
      }
    // <lots of other cases>
    default:
      x = 42;
      break;
  }
  //log("result: x = %d\n", x);
  return x;
}

int main()
{
  const int min = 1;
  const int max = 10000000;
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(min,max);

  while (true) 
  {
    int random_int = distribution(generator);
    foo(random_int);
  }
}
