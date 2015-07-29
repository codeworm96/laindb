#define BENCHMARK
#include <iostream>
#include <ctime>
#include <set>
#include <sstream>

#include "database.hpp"
#include "utility.h"

const int NREC = 1000000;

std::string itos(int x)
{
  std::ostringstream oss;
    oss << x;
    return oss.str();
}


int main()
{
    laindb::Database db("123", laindb::NEW);
    std::set<int> keys;

    //step 1
    for (int i = 0; i < NREC; ++i){
        db.put(itos(i).c_str(), i);
        keys.insert(i);
    }
    
    //step 2
    for (int i = 0; i < NREC; ++i){
        int x = db.get(itos(i).c_str());
    }

    //step 3
    for (int i = 0; i < NREC * 5; ++i){
        int k = rand() % (5 * NREC);
        while(keys.count(k) == 0){
            k = rand() % (5 * NREC);
        }
        int x = db.get(itos(k).c_str());

        if (i % 37 == 0){
            int k = rand() % (5 * NREC);
            while(keys.count(k) == 0){
                k = rand() % (5 * NREC);
            }
           // db.erase(k);
        }

        if (i % 11 == 0){
            int k = rand() % (5 * NREC);
            while(keys.count(k)){
                k = rand() % (5 * NREC);
            }
            db.put(itos(k).c_str(), k);
            int x = db.get(itos(k).c_str());
        }

        if (i % 17 == 0){
            int k = rand() % (5 * NREC);
            while(keys.count(k) == 0){
                k = rand() % (5 * NREC);
            }
            db.put(itos(k).c_str(), k);
        }
    }
    

    std::cout << db.TIME / static_cast<double>(CLOCKS_PER_SEC) << "s" << std::endl;
}
