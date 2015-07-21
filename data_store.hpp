#ifndef LAINDB_DATASTORE_HPP_
#define LAINDB_DATASTORE_HPP_

#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdint>

#include "bytes.h"

namespace laindb {

    //NOTE: use typedef instead of template parameters now
    typedef int Address;

    class Allocator {
        public:
            Allocator(const std::string & name) :size(0){}
            Address alloc(int16_t s) { Address res = size; size += s; return res; }
            void dealloc(Address addr, int16_t s) {}
        private:
            int size;
    };

    /*
     * Class: DataStore
     * A implementation of Data for debug
     */

    class DataStore{
        public:

            DataStore(const std::string & name);
            ~DataStore() { std::fclose(data_file);}
            Bytes load(Address address);
            //Side effect: after the call the raw will be invalid
            Address store(Bytes & raw);
            void free(Address address);

        private:
            FILE * data_file;
            Allocator allocator;
    };

    DataStore::DataStore(const std::string & name):allocator(name)
    {
        std::string data_file_name = name + std::string(".dat");
        data_file = fopen(data_file_name.c_str(), "w+b");
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
        Address res = allocator.alloc(raw.size + sizeof raw.size);
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
        allocator.dealloc(address, size + sizeof size);
    }
}


#endif //LAINDB_DATASTORE_HPP_
