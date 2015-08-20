test_insert: pager.o hashmap.o
	clang++ ./test_insert.cpp ./pager.o ./hashmap.o -o ./test_insert -std=c++11 -O2
bm: pager.o hashmap.o
	clang++ ./bm.cpp ./pager.o ./hashmap.o -o ./bm -std=c++11 -O2
pager.o: hashmap.o
	clang++ -c ./pager.cpp -o ./pager.o -std=c++11 -O2
hashmap.o:
	clang++ -c ./hashmap.cpp -o ./hashmap.o -std=c++11 -O2
clean:
	rm pager.o hashmap.o test_insert bm
