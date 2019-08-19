#include <random>
#include <iostream>

int goFastPath(int* arr, int n);
int goSlowPath(int* arr, int n);

int main() {
  int arr[1000];
  for (int i = 0; i < 1000; i++) {
    arr[i] = i;
  }

  const int min = 0;
  const int max = 999;
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(min,max);

  // counting sum up to N
  for (int i = 0; i < 100000; i++) {
    int random_int = distribution(generator);
    if (random_int < 999)
      goFastPath(arr, random_int);
    else
	  goSlowPath(arr, random_int);
  }
  return 0;
}

/*
    int res1 = goFastPath(arr, random_int);
    int res2 = goSlowPath(arr, random_int);
    if (res1 != res2)
	  std::cout << "No: i = " << random_int << " " << res1 << " " << res2 << "\n";
*/
