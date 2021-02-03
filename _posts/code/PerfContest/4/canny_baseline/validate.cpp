#include <vector>
#include <iostream>
#include <fstream>

template <typename T>
std::vector<T> read_binary_file(const char *fname, size_t num = 0) {
  std::vector<T> vec;
  std::ifstream ifs(fname, std::ios::in | std::ios::binary);
  if (ifs.good()) {
    ifs.unsetf(std::ios::skipws);
    std::streampos file_size;
    ifs.seekg(0, std::ios::end);
    file_size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    size_t max_num = file_size / sizeof(T);
    vec.resize(num ? (std::min)(max_num, num) : max_num);
    ifs.read(reinterpret_cast<char *>(vec.data()), vec.size() * sizeof(T));
  }
  return vec;
}

template <typename T>
bool cmp_binary_files(const char *fname1, const char *fname2, T tolerance) {
  const auto vec1 = read_binary_file<T>(fname1);
  const auto vec2 = read_binary_file<T>(fname2);
  if (vec1.size() != vec2.size()) {
    std::cerr << fname1 << " size is " << vec1.size();
    std::cerr << " whereas " << fname2 << " size is " << vec2.size()
              << std::endl;
    return false;
  }
  for (size_t i = 0; i < vec1.size(); i++) {
    if (abs(vec1[i] - vec2[i]) > tolerance) {
      std::cerr << "Mismatch at " << i << ' ';
      if (sizeof(T) == 1) {
        std::cerr << (int)vec1[i] << " vs " << (int)vec2[i] << std::endl;
      } else {
        std::cerr << vec1[i] << " vs " << vec2[i] << std::endl;
      }
      return false;
    }
  }
  return true;
}

int main(int argc, char* argv[]) {
    
    bool result = cmp_binary_files<char>(argv[1], argv[2], 1);

    if (!result) {
      std::cout << "Validation failed" << std::endl;        
      return 1;
    }
        
    std::cout << "Validation successful" << std::endl;
    return 0;
}
