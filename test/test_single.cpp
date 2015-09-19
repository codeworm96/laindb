#define BENCHMARK
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <algorithm>

#include "../lib/database.hpp"
#include "../lib/utility.hpp"
#include "../lib/optional.hpp"
#include "data_seeder.h"

const int SAMPLE_NUM = 1 << 7;

const int SAMPLE_SIZE = 1 << 13;

double res[200][3];

enum TestType {INSERT, SEARCH, DELETE};

int main()
{
    DataSeeder seeder((SAMPLE_NUM + 1) * SAMPLE_SIZE);

    laindb::Database<int> db("test", laindb::NEW);
    for (int i = 0; i < SAMPLE_NUM; ++i){
        //step 1
        for (int j = i * SAMPLE_SIZE; j < (i + 1) * SAMPLE_SIZE; ++j){
            db.put(seeder.key_for_put().c_str(), j);
        }
        //step 2 - insert
        int begin = db.TIME;
        for (int j = (i + 1) * SAMPLE_SIZE; j < (i + 2) * SAMPLE_SIZE; ++j){
            db.put(seeder.key_for_put().c_str(), j);
        }
        int end = db.TIME;
        res[i][INSERT] = (end - begin) / static_cast<double>(CLOCKS_PER_SEC) * 1000;

        //step 3 - search
        begin = db.TIME;
        for (int j = (i + 1) * SAMPLE_SIZE; j < (i + 2) * SAMPLE_SIZE; ++j){
            laindb::Optional<int> t = db.get(seeder.key_for_get().c_str());
        }
        end = db.TIME;
        res[i][SEARCH] = (end - begin) / static_cast<double>(CLOCKS_PER_SEC) * 1000;

        //step 4 - delete
        begin = db.TIME;
        for (int j = (i + 1) * SAMPLE_SIZE; j < (i + 2) * SAMPLE_SIZE; ++j){
            db.erase(seeder.key_for_del().c_str());
        }
        end = db.TIME;
        res[i][DELETE] = (end - begin) / static_cast<double>(CLOCKS_PER_SEC) * 1000;
    }

    std::ofstream fout("res.js");
    fout << "var res=[";
    for (int i = 0; i < SAMPLE_NUM; ++i){
        fout << '[' << (i + 1) * SAMPLE_SIZE << ',' << res[i][INSERT] << ',' << res[i][SEARCH] << ',' << res[i][DELETE] << ']';
        if (i < SAMPLE_NUM - 1){
            fout << ',' << std::endl;
        }else{
            fout << "];" << std::endl;
        }
    }
    return 0;
}
