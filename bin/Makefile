PONY. all: example laindb-cli laindb-c-cli
laindb-c-cli: liblaindb.a
	clang ./laindb-c-cli.c ./liblaindb.a -lstdc++ -o ./laindb-c-cli -O2
example: liblaindb.a
	clang++ ./example.cpp ./liblaindb.a -o ./example -std=c++11 -O2
laindb-cli: liblaindb.a
	clang++ ./laindb-cli.cpp ./liblaindb.a -o ./laindb-cli -std=c++11 -O2
liblaindb.a: pager.o hashmap.o bptree_index.o node_store.o laindb-c.o
	ar rcs ./liblaindb.a ./pager.o ./hashmap.o ./bptree_index.o ./node_store.o ./laindb-c.o
pager.o:
	clang++ -c ../lib/pager.cpp -o ./pager.o -std=c++11 -O2 -fPIC
hashmap.o:
	clang++ -c ../lib/hashmap.cpp -o ./hashmap.o -std=c++11 -O2 -fPIC
bptree_index.o:
	clang++ -c ../lib/bptree_index.cpp -o ./bptree_index.o -std=c++11 -O2 -fPIC
node_store.o:
	clang++ -c ../lib/node_store.cpp -o ./node_store.o -std=c++11 -O2 -fPIC
laindb-c.o:
	clang++ -c ../lib/laindb-c.cpp -o ./laindb-c.o -std=c++11 -O2 -fPIC
clean:
	rm *.o liblaindb.a example laindb-cli laindb-c-cli
