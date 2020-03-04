# GroupBy Solution

## Problem Statement

The goal of this assignment is to implement a program which can group a
collection of key-value pairs by their keys. The program will combine all values
in sorted order associated with a key, and return the resulting keys in sorted
order. The program will take a memory limit as input as well as the key-value
pairs from STDIN. It should write the output (key, sorted list of values) in
stdout. The function should work even if the input size exceeds the size of
memory.

## Solution

### Step 1:
Read all key-value pairs one by one from STDIN and add them to a temporary
buffer. When buffer size exceeds memory limit (let's say M), sort the pairs in
buffer, write them to a file and clear the buffer. This way we divide all pairs
into K files, where key-value pairs in each file are sorted. And size of each
file is approximately equal to memory limit provided.

Time complexity: O(N log P)
Max memory: M + delta
  where N = Total number of key-value pairs
        K = Number of files created
        P = Average key-value pairs per file ~= N / K
        M = Memory limit
        delta = STDIN and output file stream buffer + process overhead

### Step 2:
Now we have K sorted files. We would like to read key-value pair from these
files one-by-one and perform K-way sorted merge to get globally sorted stream of
key-value pairs. To minimize disk operations we use buffer of size B in each
stream, when reading values from disk. Now merging K streams takes approximately
K * B + delta memory. If number of steams K is very high we might exceed the
memory limit here. To avoid this we limit the number of streams that can be
merged at once. Let's say this limit is F. With B buffer size F ~= M / B.
If K exceeds F we pick F streams and merge them into single stream. We repeat
this process till we are left with only F or less streams.

Time complexity: O(S * n * log F)
Max memory: F * B + delta
  where S = Number of merge steps needed. Each step reduces number of streams
            by F - 1. Hence S ~= K / (F - 1)
        F = Maximum number of file streams that can be merged at once. If
            program memory limit is M and buffer size of each strema is B.
            F ~= M / B
        n = Average number of pairs read in merge step
        B = Buffer size of each stream

### Step 3:
This step is pretty straight forward. In this step we perform K-way sorted merge on
the files created in previous step. Since this is a sorted stream all values for
a key will appear together in stream. We write these values on STDOUT along with
the key. We keep a variable to store key currently being processed and write
space seperated values as long as we receive values for the same key. When we
receive a pair with differect key then write key on newline and update the stored key.

Time complexity: (N log F)
  where N = Total number of key-value pairs
        F = Maximum number of file streams that can be merged at once.

Max memory: F * B + delta
  where F = Maximum number of file streams that can be merged at once. If
            program memory limit is M and buffer size of each strema is B.
            F ~= M / B
        B = Buffer size of each stream

## Source code structure
sorted_merge_iterator.hpp : header file which contains KeyValuePair struct and
                            k-way sorted merge iterator class.
sorted_merge_iterator.cpp : contains implementation of k-way sorted merge
                            iterator class.
group_pairs_util.hpp : header file for utility which groups values by key.
group_pairs_util.cpp : source file which implements all the logic of grouping
                       as described above.
main.cpp             : Main function.
test.cpp             : unit tests for group pairs utility.

## Build Instructions

### Prerequsites:
- c++11 compiler
- make

### Build and run program
$ make && ./group_pairs_util < input.txt > output.txt

### Build and run tests
$ make test && ./group_pairs_test

### Implementation Notes and Further Improvements
1. We use C++ sytle io cin/cout for reading and writing which is slower as
   compared to C style io scanf/printf. We can change that for better
   performance.
2. Program uses approximately 2x of the memory limit, as memory freed in various
   steps may not be returned to OS immediately. In order to reduce peak memory 
   usage of the program we have considered memory limit as half of the limit 
   provided. This might need more tuning to optimize peak memory usage.
3. We write all temporary files in current working directory. Better approach
   would be to write them in a staging directory provided to program as input.
   We avoided to add dependency on boost::filesystem library which can be
   used to create directory and joining file paths in platform agnostic way.
4. If we can store all key-value pairs in memory without going over the max
   memory limit. We can avoid creating intermediate file.
5. We use 256 KB as buffer size for file streams. This might need more tuning
   for better performance.
