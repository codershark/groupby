CXX=g++
CPPFLAGS=-std=c++11 -Wall

all: main

main: main.o group_pairs_util.o sorted_merge_iterator.o
	$(CXX) $(CPPFLAGS) main.o group_pairs_util.o sorted_merge_iterator.o -o group_pairs_util

test: test.o group_pairs_util.o sorted_merge_iterator.o
	$(CXX) $(CPPFLAGS) test.o group_pairs_util.o sorted_merge_iterator.o -o group_pairs_test

test.o: group_pairs_util.hpp sorted_merge_iterator.hpp
main.o: group_pairs_util.hpp sorted_merge_iterator.hpp
group_pairs_util.o: group_pairs_util.hpp sorted_merge_iterator.hpp
sorted_merge_iterator.o: sorted_merge_iterator.hpp

clean:
	rm -f group_pairs_util group_pairs_test *.o
