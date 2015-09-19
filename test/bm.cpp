#define BENCHMARK
#include <iostream>
#include <vector>
#include <ctime>

#include "../lib/database.hpp"
#include "../lib/utility.hpp"
#include "../lib/optional.hpp"
#include "../lib/bptree_index.h"
#include "../lib/data_store.hpp"
#include "../lib/append_only_allocator.h"
#include "data_seeder.h"

double benchmark(int n)
{
    DataSeeder seeder(2 * n);
    laindb::Database<int, laindb::DefaultSerializer<int>, laindb::BptreeIndex, laindb::DataStore<laindb::AppendOnlyAllocator> > db("test", laindb::NEW);
    std::vector<std::string> keys;
    
    for (int i = 0; i < n; ++i){
        keys.push_back(seeder.key_for_put());
    }
    for (int i = 0; i < n; ++i){
        db.put(keys[i].c_str(), i);
    }
    for (int i = 0; i < n; ++i){
        laindb::Optional<int> res = db.get(keys[i].c_str());
    }
    
    int size = n;
    for (int i = 0; i < 5 * n; ++i){
        laindb::Optional<int> res = db.get(seeder.key_for_get().c_str());
        if (i % 37 == 0){
            db.erase(seeder.key_for_del().c_str());
            --size;
        }
        if (i % 11 == 0){
            std::string key = seeder.key_for_put();
            db.put(key.c_str(), i);
            ++size;
            laindb::Optional<int> res = db.get(key.c_str());
        }
        if (i % 17 == 0){
            db.put(seeder.key_for_get().c_str(), i);
        }
    }
    
    while (size > 0){
        for (int i = 0; i < 10; ++i){
            laindb::Optional<int> res = db.get(seeder.key_for_get().c_str());
        }
        db.erase(seeder.key_for_del().c_str());
        --size;
    }
    return db.TIME / static_cast<double>(CLOCKS_PER_SEC);
}

int main()
{
    std::vector<double> res;
    for (int i = 1 << 16; i < 1 << 20; i += 1 << 16){
        res.push_back(benchmark(i));
    }
    std::cout << '[';
    for (int i = 0; i < res.size(); ++i){
        if (i > 0){
            std::cout << ',';
        }
        std::cout << res[i];
    }
    std::cout << ']' << std::endl;
    return 0;
}
