// Author: Sukriti Kumar
//
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

#include "group_pairs_util.hpp"

const int64_t kMemoryLimit = 1024 * 1024;  // 1 MB
const int64_t kBufferSize = 16 * 1024;  // 16 KB

std::string ToString(const std::vector<std::string>& pairs) {
  std::stringstream sstream;
  for (const auto& p : pairs) sstream << p << std::endl;
  return sstream.str();
};

void Test(std::vector<std::string> pairs, const std::string& expected) {
  // Shuffle input pairs.
  std::shuffle(pairs.begin(), pairs.end(),
      std::default_random_engine(pairs.size()));
  std::string input = ToString(pairs);

  std::stringstream in(input, std::istream::in);
  std::stringstream out;
  GroupByKeyUtil util(in, out, kMemoryLimit, kBufferSize);
  util.Run();
  std::string actual = out.str();
  if (actual == expected) {
    std::cout << "Test Passed" << std::endl;
    return;
  }
  std::cout << "Test Failed" << std::endl;
  std::cout << "expected=" << expected << " actual=" << actual << std::endl;
}

int main() {
  std::cout << "Running tests..." << std::endl;
  // Single pair.
  Test({"a a"}, "a a\n");
  // Two pairs with same key.
  Test({"a a", "a b"}, "a a b\n");
  // Two pairs with different key.
  Test({"a a", "b b"}, "a a\nb b\n");

  // 26 x 26 pairs of the form "a a", "a b", ... "z z"
  {
    std::vector<std::string> pairs;
    std::string expected;
    for (char i = 'a'; i <= 'z'; ++i) {
      expected += i;
      for (char j = 'a'; j <= 'z'; ++j) {
        pairs.push_back({i, ' ', j});
        expected += {' ', j};
      }
      expected.push_back('\n');
    }
    Test(pairs, expected);
  }

  // Test large input which exceeds memory limit of 1 MB.
  {
    std::vector<std::string> pairs;
    std::string expected;
    for (int i = 0; i < 500; ++i) {
      std::stringstream key;
      key << std::setfill('0') << std::setw(3) << i;
      expected += key.str();;
      for (int j = 0; j < 500; ++j) {
        std::stringstream pair;
        pair << std::setfill('0') << std::setw(3) << i << " "
             << std::setfill('0') << std::setw(3) << j;
        pairs.push_back(pair.str());
        expected += " " + pair.str().substr(4);
      }
      expected.push_back('\n');
    }
    Test(pairs, expected);
  }
}
