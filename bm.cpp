#define BENCHMARK
#include <iostream>
#include <ctime>
#include <set>

#include "database.hpp"
#include "modes.h"

const int NREC = 1000000;

int main()
{
    laindb::Database db("123", laindb::NEW);
    std::set<int> keys;

    //step 1
    for (int i = 0; i < NREC; ++i){
        db.put(i, i);
        keys.insert(i);
    }
    //step 2
    for (int i = 0; i < NREC; ++i){
        int x = db.get(i);
    }

    //step 3
    for (int i = 0; i < NREC * 5; ++i){
        int k = rand() % (5 * NREC);
        while(keys.count(k) == 0){
            k = rand() % (5 * NREC);
        }
        int x = db.get(k);

        if (i % 37 == 0){
            int k = rand() % (5 * NREC);
            while(keys.count(k) == 0){
                k = rand() % (5 * NREC);
            }
            db.erase(k);
        }

        if (i % 11 == 0){
            int k = rand() % (5 * NREC);
            while(keys.count(k)){
                k = rand() % (5 * NREC);
            }
            db.put(k, k);
            int x = db.get(k);
        }

        if (i % 17 == 0){
            int k = rand() % (5 * NREC);
            while(keys.count(k) == 0){
                k = rand() % (5 * NREC);
            }
            db.put(k, k);
        }
    }

    std::cout << db.TIME / static_cast<double>(CLOCKS_PER_SEC) << "s" << std::endl;
}
