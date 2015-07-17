#ifndef LAINDB_MAP_INDEX_HPP_
#define LAINDB_MAP_INDEX_HPP_

#include <map>

namespace laindb {

    /*
     * Class: MapIndex
     * A implementation of Index for debug
     */

    //NOTE: use typedef instead of template parameters now
    typedef int Key;
    typedef int Address;

    class MapIndex{
        public:

            MapIndex(std::string name) {}
            ~MapIndex() {}
            Address get(Key key) { return table[key]; }
            void put(Key key, Address address) { table[key] = address; }
            void erase(Key key) { table.erase(key); }

        private:
            std::map<Key, Address> table;
    };
}


#endif //LAINDB_MAP_INDEX_HPP_
