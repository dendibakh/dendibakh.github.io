int main() {
  int i;
  for (i = 0; i < 10; i++) {
    if (i >= 8)
      *(int*)0 = 0; // crash
  }
  return 0;
}
