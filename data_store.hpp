#ifndef LAINDB_DATASTORE_HPP_
#define LAINDB_DATASTORE_HPP_

#include <vector>

#include "bytes.h"

namespace laindb {

    /*
     * Class: DataStore
     * A implementation of Data for debug
     */

    //NOTE: use typedef instead of template parameters now
    typedef int Address;

    class DataStore{
        public:

            DataStore(std::string name) {}
            ~DataStore() {}
            Bytes load(Address address) { return table[address]; }
            Address store(Bytes raw) { table.push_back(raw); return table.size() - 1; }
            void free(Address address) { }

        private:
            std::vector<Bytes> table;
    };
}


#endif //LAINDB_DATASTORE_HPP_
