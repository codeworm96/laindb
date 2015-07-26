#ifndef LAINDB_DATABASE_HPP_
#define LAINDB_DATABASE_HPP_

#ifdef BENCHMARK

#include <ctime>

#endif

#include <string>

#include "bytes.h"
#include "utility.h"
#include "default_serializer.hpp"
#include "data_store.hpp"
#include "bptree_index.hpp"

namespace laindb {

    /*
     * Class: Database:
     * It is the class that represents the whole database.
     * It provides users interfaces to operate the database.
     */

    //NOTE: use these alias to replace template parameter for now
    typedef int Value;//tmp;
    typedef int Key;//tmp;
    typedef DataStore Data;//tmp;
    typedef BptreeIndex Index;//tmp;
    typedef DefaultSerializer ValueSerializer;//tmp;

    class Database {
        public:

            /*
             * Constructor
             * Should open a database which name is `name`
             * using mode `mode`
             */

            Database(const std::string & name, FileMode mode = CREATE);

            /*
             * Method: get
             * Fetch the value from the data file according to the key
             */

            Value get(const Key & key);

            /*
             * Method: put
             * Put a pair of key/value into the database
             */

            void put(const Key & key, const Value & value);

            /*
             * Method: erase
             * Erase a key/value pair from the database accoring to the key
             */

            void erase(const Key & key);

            /*
             * Destructor
             * Should close the database safely.
             */

            ~Database();

#ifdef BENCHMARK
            int TIME;
#endif

        private:

            //The name of the database
            std::string _name;

            //The data store system
            Data data;

            //The index of the database
            Index index;

    };

    Database::Database(const std::string & name, FileMode mode) :_name(name), data(name + std::string(".dat"), mode), index(name + std::string(".idx"), mode) 
    {
#ifdef BENCHMARK
        TIME = 0;
#endif
    }

    Database::~Database() {}

    Value Database::get(const Key & key)
    {
#ifdef BENCHMARK
        int START = std::clock();
#endif

        Address address = index.get(key);
        Bytes raw = data.load(address);

#ifdef BENCHMARK
        int END = std::clock();
        TIME += END - START;
#endif

        return ValueSerializer::deserialize(raw);
    }

    void Database::put(const Key & key, const Value & value)
    {
#ifdef BENCHMARK
        int START = std::clock();
#endif

        Bytes raw = ValueSerializer::serialize(value);
        Address address = data.store(raw);
        Address old_addr = index.put(key, address);
        if (old_addr != NEW_ENTRY){
            data.free(old_addr);
        }

#ifdef BENCHMARK
        int END = std::clock();
        TIME += END - START;
#endif
    }

    void Database::erase(const Key & key)
    {
#ifdef BENCHMARK
        int START = std::clock();
#endif

        Address address = index.get(key);
        index.erase(key);
        data.free(address);

#ifdef BENCHMARK
        int END = std::clock();
        TIME += END - START;
#endif
    }

}

#endif //LAINDB_DATABASE_HPP_
