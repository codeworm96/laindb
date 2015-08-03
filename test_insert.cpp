#define BENCHMARK
#include <iostream>
#include <sstream>
#include <ctime>
#include <stdexcept>

#include "database.hpp"
#include "utility.h"

const int NREC = 100000;

std::string itos(int x)
{
  std::ostringstream oss;
    oss << x;
    return oss.str();
}

int main()
{
    //step 1 insert
    {
        laindb::Database db("123", laindb::NEW);
        for (int i = NREC - 1; i >= 0; --i){
            db.put(itos(i).c_str(), i);
        }
        std::cout << "insert " << NREC << " " << db.TIME / static_cast<double>(CLOCKS_PER_SEC) << "s" << std::endl;
    }
    
    //step 2 fetch
    {
        laindb::Database db("123", laindb::OPEN);
        for (int i = 0; i < NREC; ++i){
            int t = db.get(itos(i).c_str());
            if (t != i){
                throw std::runtime_error("error");
            }
        }
        std::cout << "fetch " << NREC << " " << db.TIME / static_cast<double>(CLOCKS_PER_SEC) << "s" << std::endl;
    }

    //step 3 delete
    {
        laindb::Database db("123", laindb::OPEN);
        for (int i = NREC - 1; i >= 0; --i){
            db.erase(itos(i).c_str());
        }
        std::cout << "erase " << NREC << " " << db.TIME / static_cast<double>(CLOCKS_PER_SEC) << "s" << std::endl;
    }
    
}
