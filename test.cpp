#define BENCHMARK
#include <iostream>
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
            int a, b;
            std::cin >> a >> b;
            db.put(a, b);
        }else if (s == "f"){
            int a;
            std::cin >> a;
            std::cout << db.get(a) << std::endl;
        }else if (s == "d"){
            int a;
            std::cin >> a;
            db.erase(a);
        }
    }
    
    std::cout << db.TIME / static_cast<double>(CLOCKS_PER_SEC) << "s" << std::endl;
}
