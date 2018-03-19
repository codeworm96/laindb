#ifndef LAINDB_DATABASE_HPP_
#define LAINDB_DATABASE_HPP_

#ifdef BENCHMARK

#include <ctime>

#endif

#include <string>

#include "key_type.h"
#include "bytes.h"
#include "utility.hpp"
#include "default_serializer.hpp"
#include "data_store.hpp"
#include "default_allocator.h"
#include "bptree_index.h"
#include "optional.hpp"

namespace laindb {

    /**
     * Class template: Database:
     * It is the class that represents the whole database.
     * It provides users interfaces to operate the database.
     */

    template <typename Value,
             typename ValueSerializer = DefaultSerializer<Value>,
             typename Index = BptreeIndex,
             typename Data = DataStore<DefaultAllocator> >
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
             * returns a Optional<Value> type to represent a possible fail operation
             */

            Optional<Value> get(const char * key);

            /*
             * Method: put
             * Put a pair of key/value into the database
             * if the key exists, updates the value.
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

            class iterator {
                public:
                    iterator(Database * _db, BptreeIndex::iterator _iter): db(_db), iter(_iter) {}
                    bool operator==(const iterator & other) { return db == other.db && iter == other.iter; }
                    bool operator!=(const iterator & other) { return !(*this == other); }
                    iterator & operator++() {
                        ++iter;
                        return *this;
                    }
                    std::pair<std::string, Address> operator*() {
                        std::pair<Key, Address> r = *iter;
                        Bytes raw = db->data.load(r.second);
                        Value value = ValueSerializer::deserialize(raw);
                        return std::make_pair(std::string(r.first.content), value);
                    }
                private:
                    Database * db;
                    BptreeIndex::iterator iter;
            };

            typedef std::pair<std::string, Value> value_type;
            typedef std::input_iterator_tag iterator_category;
            iterator begin() { return iterator(this, index.begin()); }
            iterator end() { return iterator(this, index.end()); }

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

    //constructor
    template <typename Value, typename ValueSerializer, typename Index, typename Data>
    Database<Value, ValueSerializer, Index, Data>::Database(const std::string & name, FileMode mode) 
        :_name(name), data(name + std::string(".dat"), mode), index(name + std::string(".idx"), mode) 
    {
#ifdef BENCHMARK
        TIME = 0;
#endif
    }

    //destructor
    template <typename Value, typename ValueSerializer, typename Index, typename Data>
    Database<Value, ValueSerializer, Index, Data>::~Database() {}

    template <typename Value, typename ValueSerializer, typename Index, typename Data>
    Optional<Value> Database<Value, ValueSerializer, Index, Data>::get(const char * key)
    {
#ifdef BENCHMARK
        int START = std::clock();
#endif

        Address address = index.get(make_key(key));
        if (address == ENTRY_NOT_FOUND){
            Optional<Value> res;
            return res;
        }
        Bytes raw = data.load(address);
        Optional<Value> res(ValueSerializer::deserialize(raw));

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
