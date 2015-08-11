test_insert: pager.o
	clang++ ./test_insert.cpp ./pager.o -o ./test_insert -std=c++11 -O2
bm: pager.o
	clang++ ./bm.cpp ./pager.o -o ./bm -std=c++11 -O2
pager.o:
	clang++ -c ./pager.cpp -o ./pager.o -std=c++11 -O2
clean:
	rm pager.o test_insert bm
