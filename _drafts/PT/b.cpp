int goFastPath(int* arr, int n) {
  return (n * (n + 1)) / 2;
}

int goSlowPath(int* arr, int n) {
  int res = 0;
  for (int i = 0; i <= n; i++)
    res += arr[i];
  return res;
}
