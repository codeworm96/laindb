#ifndef LAINDB_DATASTORE_HPP_
#define LAINDB_DATASTORE_HPP_

#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <stdexcept>

#include "pager.h"
#include "bytes.h"
#include "utility.h"
#include "default_allocator.h"

namespace laindb {

    //NOTE: use typedef instead of template parameters now
    typedef DefaultAllocator Allocator;

    /**
     * Class: DataStore
     * In change of storing the values
     */

    class DataStore{
        public:

            /**
             * Constructor:
             * Construct a data store.
             * name: name of the file
             * mode: construct modes;
             */

            DataStore(const std::string & name, FileMode mode);

            /**
             * Destructor
             * close data store
             */

            ~DataStore();

            /**
             * method: load
             * load data from file
             */

            Bytes load(Address address);

            /**
             * method: store
             * store data into file
             * Side effect: after the call the raw will be invalid
             */

            Address store(Bytes & raw);


            /**
             * method: free
             * free a block
             */

            void free(Address address);

        private:
            Pager data_pager;
            Allocator * allocator;
    };

    DataStore::DataStore(const std::string & name, FileMode mode) :data_pager(name, mode), allocator(nullptr)
    {
        std::string idle_file_name = std::string("idle_") + name;
        FileMode status = data_pager.status();
        allocator = new Allocator(idle_file_name, status);
    }
    
    DataStore::~DataStore()
    {
        delete allocator;
    }

    Bytes DataStore::load(Address address)
    {
        Bytes res;
        data_pager.read(&res.size, sizeof(res.size), address);
        res.raw = std::malloc(res.size);
        data_pager.read(res.raw, res.size, address + sizeof(res.size));
        return res;
    }

    Address DataStore::store(Bytes & raw)
    {
        Address res = allocator->alloc(sizeof(raw.size) + raw.size);
        data_pager.write(&raw.size, sizeof(raw.size), res);
        data_pager.write(raw.raw, raw.size, res + sizeof(raw.size));

        //free
        std::free(raw.raw);
        raw.raw = nullptr;
        raw.size = 0;
        return res;
    }

    void DataStore::free(Address address)
    {
        int16_t size;
        data_pager.read(&size, sizeof(size), address);
        allocator->dealloc(address, sizeof(size) + size);
    }
}

#endif //LAINDB_DATASTORE_HPP_
