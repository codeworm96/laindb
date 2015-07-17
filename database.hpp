#ifndef LAINDB_DATABASE_HPP_
#define LAINDB_DATABASE_HPP_

#include <string>

#include "bytes.h"
#include "default_serializer.hpp"
#include "data_store.hpp"
#include "map_index.hpp"

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
    typedef MapIndex Index;//tmp;
    typedef DefaultSerializer ValueSerializer;//tmp;

    class Database {
        public:

            /*
             * Constructor
             * Should open a database which name is `name`
             */

            Database(std::string name);

            /*
             * Method: get
             * Fetch the value from the data file according to the key
             */

            Value get(Key key);

            /*
             * Method: put
             * Put a pair of key/value into the database
             */

            void put(Key key, Value value);

            /*
             * Method: erase
             * Erase a key/value pair from the database accoring to the key
             */

            void erase(Key key);

            /*
             * Destructor
             * Should close the database safely.
             */

            ~Database();

        private:

            //The name of the database
            std::string _name;

            //The data store system
            Data data;

            //The index of the database
            Index index;

    };

    Database::Database(std::string name) :_name(name), data(name), index(name) {}

    Database::~Database() {}

    Value Database::get(Key key)
    {
        Address address = index.get(key);
        Bytes raw = data.load(address);
        return ValueSerializer::deserialize(raw);
    }

    void Database::put(Key key, Value value)
    {
        Bytes raw = ValueSerializer::serialize(value);
        Address address = data.store(raw);
        index.put(key, address);
    }

    void Database::erase(Key key)
    {
        Address address = index.get(key);
        index.erase(key);
        data.free(address);
    }

}

#endif //LAINDB_DATABASE_HPP_
