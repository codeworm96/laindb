#define BENCHMARK
#include <iostream>
#include <ctime>
#include <stdexcept>

#include "database.hpp"
#include "utility.h"

const int NREC = 1000000;

int main()
{
    //step 1 insert
    {
        laindb::Database db("123", laindb::NEW);
        for (int i = NREC - 1; i >= 0; --i){
            db.put(i, i);
        }
        std::cout << "insert " << NREC << " " << db.TIME / static_cast<double>(CLOCKS_PER_SEC) << "s" << std::endl;
    }
    
    //step 2 fetch
    {
        laindb::Database db("123", laindb::OPEN);
        for (int i = 0; i < NREC; ++i){
            int t = db.get(i);
            if (t != i){
                throw std::runtime_error("error");
            }
        }
        std::cout << "fetch " << NREC << " " << db.TIME / static_cast<double>(CLOCKS_PER_SEC) << "s" << std::endl;
    }
    
}
