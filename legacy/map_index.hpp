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
    
    const Address NEW_ENTRY = -1;


    class MapIndex{
        public:

            MapIndex(const std::string & name) {}
            ~MapIndex() {}
            Address get(const Key & key) { return table[key]; }
            Address put(const Key & key, Address address); 
            void erase(const Key & key) { table.erase(key); }

        private:
            std::map<Key, Address> table;
    };

    Address MapIndex::put(const Key & key, Address address)
    {
        Address res = NEW_ENTRY;
        if(table.count(key)){
            res = table[key];
            table[key] = address;
        }else{
            table[key] = address;
        }
        return res;
    }
}


#endif //LAINDB_MAP_INDEX_HPP_
