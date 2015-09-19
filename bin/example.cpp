//include these file to use laindb
#include "../lib/database.hpp" 
#include "../lib/optional.hpp"

int main()
{
    laindb::Database<int> db("example"); //open a database
    db.put("sjtu", 1896); //insert a key-value pair
    laindb::Optional<int> res = db.get("sjtu"); //get value, Optional is a type for value that may exist and may not
    if(res.is_valid()){//check result
        //do something with res.just(), the value of the key
    }
    db.erase("sjtu");//erase a key value pair
    //database will be closed by the destructor
}

