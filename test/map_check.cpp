#include <sstream>
#include <string>
#include <cstdlib>
#include <map>

#include "test_framework.hpp"
#include "../lib/database.hpp"
#include "../lib/utility.hpp"
#include "../lib/optional.hpp"

const int TEST_NUM = 10000000;

const int TEST_SIZE = 10000;

enum TestType {INSERT, SEARCH, DELETE};

std::string itos(int x)
{
  std::ostringstream oss;
    oss << x;
    return oss.str();
}

std::string get_key()
{
    return itos(rand() % TEST_SIZE);
}

TestType get_type()
{
    switch(rand() % 10) {
        case 0: case 1:
            return INSERT;
        case 2:
            return DELETE;
        default:
            return SEARCH;
    }
}

int main()
{
    laindb::Database<int> db("test", laindb::NEW);
    std::map<std::string, int> db2;
    
    for (int i = 0; i < TEST_NUM; ++i){
        std::string key = get_key();
        switch(get_type()){
            case INSERT:
                db.put(key.c_str(), i);
                db2[key] = i;
                break;
            case SEARCH:
                {
                    laindb::Optional<int> res = db.get(key.c_str());
                    auto iter = db2.find(key);
                    if (iter != db2.end()){
                        assert_equal(true, res.is_valid());
                        assert_equal(iter->second, res.just());
                    }else{
                        assert_equal(false, res.is_valid());
                    }
                }
                break;
            case DELETE:
                db.erase(key.c_str());
                db2.erase(key);
                break;
            size_t size = 0;
            for (auto kv : db) {
                ++size;
                assert_equal(db2[kv.first], kv.second);
            }
            assert_equal(size, db2.size());
        }
    }
    
    return 0;
} 
