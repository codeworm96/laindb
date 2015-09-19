all: bm test_single basic_test map_check
.PHONY: all

bm: pager.o hashmap.o bptree_index.o node_store.o
	clang++ ./bm.cpp ./pager.o ./hashmap.o ./bptree_index.o ./node_store.o -o ./bm -std=c++11 -O2
test_single: pager.o hashmap.o bptree_index.o node_store.o
	clang++ ./test_single.cpp ./pager.o ./hashmap.o ./bptree_index.o ./node_store.o -o ./test_single -std=c++11 -O2
basic_test: pager.o hashmap.o bptree_index.o node_store.o
	clang++ ./basic_test.cpp ./pager.o ./hashmap.o ./bptree_index.o ./node_store.o -o ./basic_test -std=c++11 -O2
map_check: pager.o hashmap.o bptree_index.o node_store.o
	clang++ ./map_check.cpp ./pager.o ./hashmap.o ./bptree_index.o ./node_store.o -o ./map_check -std=c++11 -O2
pager.o:
	clang++ -c ../lib/pager.cpp -o ./pager.o -std=c++11 -O2
hashmap.o:
	clang++ -c ../lib/hashmap.cpp -o ./hashmap.o -std=c++11 -O2
node_store.o:
	clang++ -c ../lib/node_store.cpp -o ./node_store.o -std=c++11 -O2
bptree_index.o:
	clang++ -c ../lib/bptree_index.cpp -o ./bptree_index.o -std=c++11 -O2
clean:
	rm pager.o hashmap.o bptree_index.o node_store.o bm test_single basic_test map_check
