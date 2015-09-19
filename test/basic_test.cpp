#include <sstream>
#include <string>

#include "test_framework.hpp"
#include "../lib/database.hpp"
#include "../lib/utility.hpp"
#include "../lib/optional.hpp"

std::string itos(int x)
{
  std::ostringstream oss;
    oss << x;
    return oss.str();
}

TESTCASE(PUT){
    laindb::Database<int> db("test", laindb::NEW);
    for (int i = 0; i < 10; ++i){
        db.put(itos(i).c_str(), i);
    }
    
    for (int i = 0; i < 10; ++i){
        laindb::Optional<int> res = db.get(itos(i).c_str());
        assert_equal(true, res.is_valid());
        assert_equal(i, res.just());
    }
    
    laindb::Optional<int> res = db.get(itos(10).c_str());
    assert_equal(false, res.is_valid());
}

TESTCASE(ERASE){
    laindb::Database<int> db("test", laindb::NEW);
    for (int i = 0; i < 10; ++i){
        db.put(itos(i).c_str(), i);
    }

    for (int i = 0; i < 10; i += 2){
        db.erase(itos(i).c_str());
    }
    
    for (int i = 0; i < 10; ++i){
        laindb::Optional<int> res = db.get(itos(i).c_str());
        if (i % 2){
            assert_equal(true, res.is_valid());
            assert_equal(i, res.just());
        }else{
            assert_equal(false, res.is_valid());
        }
    }
}

int main()
{
    return 0;
}
