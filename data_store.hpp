#ifndef LAINDB_DATASTORE_HPP_
#define LAINDB_DATASTORE_HPP_

#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <stdexcept>

#include "bytes.h"
#include "utility.h"
#include "default_allocator.h"

namespace laindb {

    //NOTE: use typedef instead of template parameters now
    typedef int Address;
    typedef DefaultAllocator Allocator;

    /*
     * Class: DataStore
     * In change of storing the values
     */

    class DataStore{
        public:

            /*
             * Constructor:
             * Construct a data store.
             * name: name of the file
             * mode: construct modes;
             */

            DataStore(const std::string & name, FileMode mode);

            /*
             * Destructor
             * close data store
             */

            ~DataStore();

            /*
             * method: load
             * load data from file
             */

            Bytes load(Address address);

            /*
             * method: store
             * store data into file
             * Side effect: after the call the raw will be invalid
             */

            Address store(Bytes & raw);


            /*
             * method: free
             * free a block
             */

            void free(Address address);

        private:
            FILE * data_file;
            Allocator * allocator;
    };

    DataStore::DataStore(const std::string & name, FileMode mode) :data_file(nullptr), allocator(nullptr)
    {
        std::string idle_file_name = std::string("idle_") + name;
        if(mode & OPEN){
            data_file = std::fopen(name.c_str(), "r+b");
        }
        if (data_file){
            allocator = new Allocator(idle_file_name, OPEN);
        }else{
            if(mode & NEW){
                data_file = std::fopen(name.c_str(), "w+b");
            }
            if (data_file){
                allocator = new Allocator(idle_file_name, NEW);
            }else{
                throw std::runtime_error("cannot open data file");
            }
        }
    }
    
    DataStore::~DataStore()
    {
        delete allocator;
        std::fclose(data_file);
    }

    Bytes DataStore::load(Address address)
    {
        Bytes res;
        std::fseek(data_file, address, SEEK_SET);
        std::fread(&res.size, sizeof res.size, 1, data_file); //load size
        res.raw = std::malloc(res.size);
        std::fread(res.raw, res.size, 1, data_file); //load the byte string
        return res;
    }

    Address DataStore::store(Bytes & raw)
    {
        Address res = allocator->alloc(raw.size + sizeof raw.size);
        std::fseek(data_file, res, SEEK_SET);
        std::fwrite(&raw.size, sizeof raw.size, 1, data_file); //store size
        std::fwrite(raw.raw, raw.size, 1, data_file); //store the byte string
        //free
        std::free(raw.raw);
        raw.raw = nullptr;
        raw.size = 0;
        return res;
    }

    void DataStore::free(Address address)
    {
        int16_t size;
        std::fseek(data_file, address, SEEK_SET);
        std::fread(&size, sizeof size, 1, data_file); //load size
        allocator->dealloc(address, size + sizeof size);
    }
}


#endif //LAINDB_DATASTORE_HPP_
