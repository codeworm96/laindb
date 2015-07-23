#define BENCHMARK
#include <iostream>
#include <ctime>
#include "database.hpp"
#include "modes.h"

const int NREC = 100000;

int main()
{
    laindb::Database db("123", laindb::NEW);
    for (int i = 0; i < NREC; ++i){
        db.put(i, i);
    }
    std::cout << db.TIME / static_cast<double>(CLOCKS_PER_SEC) << "s" << std::endl;
}
