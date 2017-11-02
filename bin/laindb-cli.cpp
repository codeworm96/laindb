#include <iostream>
#include <string>

//include these file to use laindb
#include "../lib/database.hpp" 
#include "../lib/optional.hpp"

class StringSerializer {
        public:
            
            /**
             * Function: serialize
             * convert string to Bytes
             */

            static laindb::Bytes serialize(const std::string & s)
            {
                laindb::Bytes res(s.length() + 1, std::malloc(s.length() + 1));
                std::memcpy(res.raw, s.c_str(), res.size);
                return res;
            }

            /**
             * Function: deserialize
             * convert Bytes to string
             * Side effect: after the call raw will be invalid
             */

            static std::string deserialize(laindb::Bytes & raw)
            {
                std::string res(static_cast<char *>(raw.raw));
                std::free(raw.raw); //free space
                return res;
            }

    };

int main(int argc, char** argv)
{
    if (argc <= 1){
        std::cout << "database name expected" << std::endl;
        return 1;
    }
    std::string name = argv[1];

    laindb::Database<std::string, StringSerializer> db(name); //open the database
    while (true) {
        std::cout << "> ";
        std::string command;
        std::cin >> command;
        if (command == "GET"){
            std::string key;
            std::cin >> key;
            laindb::Optional<std::string> res = db.get(key.c_str());
            if(res.is_valid()){//check result
                std::cout << "-> " << res.just() << std::endl;
            }else{
                std::cout << "-> (Nothing)" << std::endl;
            }
        }else if (command == "DEL") {
            std::string key;
            std::cin >> key;
            db.erase(key.c_str());//erase a key value pair
        }else if (command == "PUT") {
            std::string key, val;
            std::cin >> key >> val;
            db.put(key.c_str(), val); //insert a key-value pair
        }else if (command == "EXIT") {
            break;
        }else{
            std::cout << "Unknown operation" << std::endl;
        }
    }

    //database will be closed by the destructor
    return 0;
}

