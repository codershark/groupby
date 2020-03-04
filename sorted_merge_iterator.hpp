// Author : Sukriti Kumar
#pragma once

#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <vector>

// Struct to represent a key-value pair.
struct KeyValuePair {
  std::string key;
  std::string value;

  // Returns memory consumed by key-value strings. Use string capacity instead
  // of size for better estimation of memory consumption.
  size_t size() {
    return key.capacity() + value.capacity();
  }
  // Overload < operator.
  bool operator <(const KeyValuePair& pair) const {
    return key == pair.key ? value < pair.value : key < pair.key;
  }
  // Overload << operator.
  friend std::ostream& operator<<(std::ostream& out, const KeyValuePair& pair) {
    return out << pair.key + " " + pair.value;
  }
  // Overload >> operator.
  friend std::istream& operator>>(std::istream& in, KeyValuePair& pair) {
    return in >> pair.key >> pair.value;
  }
};

// SortedMergeIterator class is an utility which can merge multiple sorted files
// into single sorted stream. This class internally uses a min-heap data
// structure to find mininum of all file streams.
// Time complexity of reading all pairs O(n*logk)
//   where k = number of files and n = total number of pairs.
class SortedMergeIterator {
 public:
  // Constructs SortedMergeIterator for given @filenames. Each file in
  // @filenames must contain sorted key-value pairs.
  SortedMergeIterator(const std::vector<std::string>& filenames,
                      int buffer_size);
  // Clenaup open file streams.
  ~SortedMergeIterator();

  // Returns true if iterator has any key-value pair left.
  bool HasNext();
  // Returns next key-value pair in sorted order.
  KeyValuePair Next();

 private:
  // Internal node used to in heap.
  struct HeapNode {
    HeapNode(const KeyValuePair& pair, int stream_id)
        : pair(pair), stream_id(stream_id) {}
    // Key-value pair.
    KeyValuePair pair;
    // Id of the file stream this key-value pair belongs to.
    int stream_id;
    // std::priority_queue is a max-heap with natural sort order. Reverse
    // the sort order of pair to create min-heap.
    bool operator <(const HeapNode& node) const {
      return node.pair < pair;
    }
  };

  // Custom buffers used for file streams.
  std::vector<char*> buffer_;
  // Underlying sorted streams.
  std::vector<std::ifstream> streams_;
  // min-heap to find mininum pair efficiently during merge.
  std::priority_queue<HeapNode> min_heap_;
};
