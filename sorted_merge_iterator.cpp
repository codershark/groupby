// Author : Sukriti Kumar
#include "sorted_merge_iterator.hpp"

SortedMergeIterator::SortedMergeIterator(
    const std::vector<std::string>& filenames, int buffer_size) {
  // Initialize file streams.
  for (const std::string& file : filenames) {
    buffer_.push_back(new char[buffer_size]);
    streams_.emplace_back();
    std::ifstream& stream = streams_.back();
    stream.rdbuf()->pubsetbuf(buffer_.back(), buffer_size);
    stream.open(file, std::ifstream::in);
  }
  // Read first pair from all streams to initialize heap.
  for (size_t i = 0; i < streams_.size(); ++i) {
    std::ifstream& stream = streams_.at(i);
    KeyValuePair pair;
    if (stream >> pair) {
      HeapNode node(pair, i);
      min_heap_.push(node);
    }
  }
}

SortedMergeIterator::~SortedMergeIterator() {
  // Delete buffers.
  for (char* buffer : buffer_) {
    delete[] buffer;
  }
  // Close file streams.
  for (std::ifstream& stream : streams_) {
    if (stream.is_open()) {
      stream.close();
    }
  }
}

bool SortedMergeIterator::HasNext() {
  return not min_heap_.empty();
}

KeyValuePair SortedMergeIterator::Next() {
  // Extract lexicographically smallest key value pair from heap and push
  // next key value pair from the same file stream.
  HeapNode node = min_heap_.top();
  min_heap_.pop();
  std::ifstream& stream = streams_.at(node.stream_id);
  KeyValuePair pair;
  if (stream >> pair) {
    HeapNode next(pair, node.stream_id);
    min_heap_.push(next);
  }
  return node.pair;
}
