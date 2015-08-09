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
    
    std::cout << "step 1" << std::endl;
    
    //step 2
    for (int i = 0; i < NREC; ++i){
        int x = db.get(itos(i).c_str());
    }

    std::cout << "step 2" << std::endl;

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
            db.erase(itos(k).c_str());
            keys.erase(k);
        }

        if (i % 11 == 0){
            int k = rand() % (5 * NREC);
            while(keys.count(k)){
                k = rand() % (5 * NREC);
            }
            db.put(itos(k).c_str(), k);
            keys.insert(k);
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

    std::cout << "step 3" << std::endl;

    std::vector<int> tmp;
    for (auto & k: keys){
        tmp.push_back(k);
    }

    //step 4
    while(!tmp.empty()){
        for(int i = 0; i < 10; ++i){
            int k = rand() % tmp.size();
            int x = db.get(itos(tmp[k]).c_str());
        }
        db.erase(itos(tmp.back()).c_str());
        tmp.pop_back();
    }
    std::cout << "step 4" << std::endl;

    std::cout << db.TIME / static_cast<double>(CLOCKS_PER_SEC) << "s" << std::endl;
}
