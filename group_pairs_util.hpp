// Author : Sukriti Kumar
#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "sorted_merge_iterator.hpp"

// Utility class to group key-value pairs by key. This class implementa all 3
// steps as described in README.md.
class GroupByKeyUtil {
 public:
  GroupByKeyUtil(std::istream& istream, std::ostream& ostream,
                 int64_t memory_limit, int64_t buffer_size);
  // Entry method to run the program.
  bool Run();

 private:
  // Step 1: Read all key value pairs one by one from STDIN and add them to
  // a temporary buffer. When buffer size exceeds @memory_limit_, sort the
  // pairs in buffer, write them to a file and clear the buffer.
  // This way we divide all pairs into K files. Key value pairs in each file
  // are sorted. And size of each file is approximately equal to
  // @memory_limit_ provided.
  // Populates @files with created files.  Returns true on success, false
  // otherwise.
  bool BuildSortedFiles(std::vector<std::string>* files);
  // Step 2: Merge @files to reduce number of files to @max_files. Populates
  // @merged_files with merged filenames. @merged_files should not exceed
  // @max_files. Returns true on success, false otherwise.
  bool MergeSortedFiles(size_t max_files,
                        const std::vector<std::string>& files,
                        std::vector<std::string>* merged_files);
  // Step 3: Group values by key and write to STDOUT. Returns true on success,
  // false otherwise.
  bool GroupValuesByKey(SortedMergeIterator& iterator);

  // Writes key-value pairs from @iterator to @filename.  Returns true on
  // success, false otherwise.
  bool WriteToFile(SortedMergeIterator& iterator, const std::string& filename);
  // Writes key-value pairs from @buffer to @filename.  Returns true on success,
  // false otherwise.
  bool WriteBuffer(std::vector<KeyValuePair>& buffer,
                   const std::string& filename);

  // Read key-value pairs from this stream.
  std::istream& istream_;
  // Write gropued values to this stream.
  std::ostream& ostream_;
  // Max memory this program can use at any moment.
  int64_t memory_limit_ = 0;
  // Buffer size of each file stream.
  int64_t buffer_size_ = 0;
};
