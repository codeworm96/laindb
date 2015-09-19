#ifndef LAINDB_DATASTORE_HPP_
#define LAINDB_DATASTORE_HPP_

#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <stdexcept>

#include "pager.h"
#include "bytes.h"
#include "utility.hpp"
#include "default_allocator.h"

namespace laindb {

    /**
     * Class template: DataStore
     * In change of storing the values
     */

    template<typename Allocator = DefaultAllocator> class DataStore{
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

            ~DataStore() { delete allocator; }

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

    template<typename Allocator> DataStore<Allocator>::DataStore(const std::string & name, FileMode mode) :data_pager(name, mode, 6151), allocator(nullptr)
    {
        std::string idle_file_name = std::string("idle_") + name;
        FileMode status = data_pager.status();
        allocator = new Allocator(idle_file_name, status);
    }
    
    template<typename Allocator> Bytes DataStore<Allocator>::load(Address address)
    {
        Bytes res;
        data_pager.read(&res.size, sizeof(res.size), address);
        res.raw = std::malloc(res.size);
        data_pager.read(res.raw, res.size, address + sizeof(res.size));
        return res;
    }

    template<typename Allocator> Address DataStore<Allocator>::store(Bytes & raw)
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

    template<typename Allocator> void DataStore<Allocator>::free(Address address)
    {
        int16_t size;
        data_pager.read(&size, sizeof(size), address);
        allocator->dealloc(address, sizeof(size) + size);
    }
}

#endif //LAINDB_DATASTORE_HPP_
