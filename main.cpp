#include "group_pairs_util.hpp"

// Main driver program to group the values by key.
int main() {
  int64_t memory_limit;
  std::cin >> memory_limit;
  int64_t buffer_size = 256 * 1024;  // 256 KB
  GroupByKeyUtil util(std::cin, std::cout, memory_limit, buffer_size);
  if (not util.Run()) {
    std::cerr << "Failed to execute." << std::endl;
    return 1;
  }
  return 0;
}
