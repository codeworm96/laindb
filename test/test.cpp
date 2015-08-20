#define BENCHMARK
#include <iostream>
#include <sstream>
#include <ctime>
#include "database.hpp"
#include "utility.h"

int main()
{
    laindb::Database db("123", laindb::NEW);
    while(true){
        std::string s;
        std::cin >> s;
        if (s == "exit"){
            break;
        }else if (s == "i"){
            std::string a;
            int b;
            std::cin >> a >> b;
            db.put(a.c_str(), b);
        }else if (s == "f"){
            std::string a;
            std::cin >> a;
            try{
               std::cout << db.get(a.c_str()) << std::endl;
            }catch(std::runtime_error & e){
               std::cout << e.what() << std::endl;
            }
        }else if (s == "d"){
            std::string a;
            std::cin >> a;
            db.erase(a.c_str());
        }
    }
    
    std::cout << db.TIME / static_cast<double>(CLOCKS_PER_SEC) << "s" << std::endl;
}
