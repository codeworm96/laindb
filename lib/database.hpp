#ifndef LAINDB_DATABASE_HPP_
#define LAINDB_DATABASE_HPP_

#ifdef BENCHMARK

#include <ctime>

#endif

#include <string>
#include <stdexcept>

#include "key_type.h"
#include "bytes.h"
#include "utility.h"
#include "default_serializer.hpp"
#include "data_store.hpp"
#include "bptree_index.hpp"

namespace laindb {

    /**
     * Class: Database:
     * It is the class that represents the whole database.
     * It provides users interfaces to operate the database.
     */

    template <typename Value,
             typename ValueSerializer = DefaultSerializer<Value>,
             typename Index = BptreeIndex,
             typename Data = DataStore>
    class Database {
        public:

            /**
             * Constructor
             * Should open a database which name is `name`
             * using mode `mode` (default: CREATE)
             */

            Database(const std::string & name, FileMode mode = CREATE);

            /**
             * Method: get
             * Fetch the value from the data file according to the key
             */
            //TODO: when fails?

            Value get(const char * key);

            /*
             * Method: put
             * Put a pair of key/value into the database
             * if the key exists, overwrites the value.
             */

            void put(const char * key, const Value & value);

            /*
             * Method: erase
             * Erase a key/value pair from the database accoring to the key
             * if the key does not exist, do nothing.
             */

            void erase(const char * key);

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

    template <typename Value, typename ValueSerializer, typename Index, typename Data>
    Database<Value, ValueSerializer, Index, Data>::Database(const std::string & name, FileMode mode) 
        :_name(name), data(name + std::string(".dat"), mode), index(name + std::string(".idx"), mode) 
    {
#ifdef BENCHMARK
        TIME = 0;
#endif
    }

    template <typename Value, typename ValueSerializer, typename Index, typename Data>
    Database<Value, ValueSerializer, Index, Data>::~Database() {}

    template <typename Value, typename ValueSerializer, typename Index, typename Data>
    Value Database<Value, ValueSerializer, Index, Data>::get(const char * key)
    {
#ifdef BENCHMARK
        int START = std::clock();
#endif

        Address address = index.get(make_key(key));
        if (address == ENTRY_NOT_FOUND){
            throw std::runtime_error("not found");
        }
        Bytes raw = data.load(address);
        Value res = ValueSerializer::deserialize(raw);

#ifdef BENCHMARK
        int END = std::clock();
        TIME += END - START;
#endif

        return res;
    }

    template <typename Value, typename ValueSerializer, typename Index, typename Data>
    void Database<Value, ValueSerializer, Index, Data>::put(const char * key, const Value & value)
    {
#ifdef BENCHMARK
        int START = std::clock();
#endif

        Bytes raw = ValueSerializer::serialize(value);
        Address address = data.store(raw);
        Address old_addr = index.put(make_key(key), address);
        if (old_addr != ENTRY_NOT_FOUND){
            data.free(old_addr);
        }

#ifdef BENCHMARK
        int END = std::clock();
        TIME += END - START;
#endif
    }

    template <typename Value, typename ValueSerializer, typename Index, typename Data>
    void Database<Value, ValueSerializer, Index, Data>::erase(const char * key)
    {
#ifdef BENCHMARK
        int START = std::clock();
#endif

        Address address = index.erase(make_key(key));
        if (address != ENTRY_NOT_FOUND){
            data.free(address);
        }

#ifdef BENCHMARK
        int END = std::clock();
        TIME += END - START;
#endif
    }

}

#endif //LAINDB_DATABASE_HPP_
