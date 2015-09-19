example: pager.o hashmap.o bptree_index.o node_store.o
	clang++ ./example.cpp ./pager.o ./hashmap.o ./bptree_index.o ./node_store.o -o ./example -std=c++11 -O2
laindb-cli: pager.o hashmap.o bptree_index.o node_store.o
	clang++ ./laindb-cli.cpp ./pager.o ./hashmap.o ./bptree_index.o ./node_store.o -o ./laindb-cli -std=c++11 -O2
pager.o:
	clang++ -c ../lib/pager.cpp -o ./pager.o -std=c++11 -O2
hashmap.o:
	clang++ -c ../lib/hashmap.cpp -o ./hashmap.o -std=c++11 -O2
node_store.o:
	clang++ -c ../lib/node_store.cpp -o ./node_store.o -std=c++11 -O
bptree_index.o:
	clang++ -c ../lib/bptree_index.cpp -o ./bptree_index.o -std=c++11 -O2
clean:
	rm pager.o hashmap.o node_store.o bptree_index.o example laindb-cli
