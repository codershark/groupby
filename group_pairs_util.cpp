// Author : Sukriti Kumar
#include "group_pairs_util.hpp"

namespace {

// For simplicity we use current directory to store temporary files.
// A better apporach would be to write these files in staging directory.
std::string NextFile() {
  static int file_id = 0;
  return "tempfile" + std::to_string(file_id++) + ".txt";
}

}  // anonymous namespace

GroupByKeyUtil::GroupByKeyUtil(std::istream& istream, std::ostream& ostream,
                               int64_t memory_limit, int64_t buffer_size)
    : istream_(istream),
      ostream_(ostream),
      memory_limit_(memory_limit / 2),
      buffer_size_(buffer_size) {}

bool GroupByKeyUtil::Run() {
  // Step 1: Create sorted files.
  std::vector<std::string> files;
  if (not BuildSortedFiles(&files)) {
    return false;
  }

  // Maximum number of files that can be merged at once. Substract 1 to include
  // output stream buffer.
  int64_t max_files = memory_limit_ / buffer_size_ - 1;

  // Step 2: Merge sorted files if required.
  std::vector<std::string> merged_files;
  if (not MergeSortedFiles(max_files, files, &merged_files)) {
    return false;
  }

  // Step 3: Group values by keys and write to STDOUT.
  SortedMergeIterator iterator(merged_files, buffer_size_);
  if (not GroupValuesByKey(iterator)) {
    return false;
  }
  return true;
}

bool GroupByKeyUtil::BuildSortedFiles(std::vector<std::string>* files) {
  std::vector<KeyValuePair> buffer;
  int read_bytes = 0;

  KeyValuePair pair;
  while (istream_ >> pair) {
    buffer.push_back(pair);
    read_bytes += pair.size();
    int64_t memory_used = read_bytes + buffer.capacity() * sizeof(KeyValuePair);
    if (memory_used >= memory_limit_) {
      std::string filename = NextFile();
      files->push_back(filename);
      if (not WriteBuffer(buffer, filename)) {
        return false;
      }
      read_bytes = 0;
    }
  }
  if (buffer.size() > 0) {
    std::string filename = NextFile();
    files->push_back(filename);
    if (not WriteBuffer(buffer, filename)) {
      return false;
    }
  }
  return true;
}

bool GroupByKeyUtil::MergeSortedFiles(size_t max_files,
                                      const std::vector<std::string>& files,
                                      std::vector<std::string>* merged_files) {
  if (max_files > files.size()) {
    *merged_files = files;
    return true;
  }
  std::queue<std::string> tobe_merged;
  for (const std::string& file : files) {
    tobe_merged.push(file);
  }
  while (tobe_merged.size() > max_files) {
    // Extract max_size files and  merge them.
    std::vector<std::string> batch;
    for (size_t i = 0; i < max_files; ++i) {
      batch.push_back(tobe_merged.front());
      tobe_merged.pop();
    }
    std::string merge_file = NextFile();
    SortedMergeIterator iterator(batch, buffer_size_);
    if (not WriteToFile(iterator, merge_file)) {
      return false;
    }
    tobe_merged.push(merge_file);
  }
  while (not tobe_merged.empty()) {
    merged_files->push_back(tobe_merged.front());
    tobe_merged.pop();
  }
  return true;
}

bool GroupByKeyUtil::WriteBuffer(std::vector<KeyValuePair>& buffer,
                                 const std::string& filename) {
  std::ofstream fout(filename);
  if (not fout.is_open()) {
    std::cerr << "Failed to create file: " << filename << std::endl;
    return false;
  }
  std::sort(buffer.begin(), buffer.end());
  for (const KeyValuePair& pair : buffer) {
    fout << pair << '\n';
  }
  fout.close();
  buffer.clear();
  return true;
}

bool GroupByKeyUtil::WriteToFile(SortedMergeIterator& iterator,
                                 const std::string& filename) {
  std::ofstream fout(filename);
  if (not fout.is_open()) {
    std::cerr << "Failed to create file: " << filename << std::endl;
    return false;
  }
  while (iterator.HasNext()) {
    fout << iterator.Next() << '\n';
  }
  fout.close();
  return true;
}

bool GroupByKeyUtil::GroupValuesByKey(SortedMergeIterator& iterator) {
  std::string curr_key = "";
  while (iterator.HasNext()) {
    KeyValuePair pair = iterator.Next();
    if (pair.key == curr_key) {
      ostream_ << " " << pair.value;
    } else {
      if (curr_key != "") {
        ostream_ << '\n';
      }
      curr_key = pair.key;
      ostream_ << pair;
    }
  }
  ostream_ << '\n';
  return true;
}
