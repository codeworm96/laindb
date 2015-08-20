#ifndef LAINDB_HASHMAP_H_
#define LAINDB_HASHMAP_H_

#include "page.h"

namespace laindb {

    /**
     * struct: bucket
     * bucket for HashMap
     */

    struct Bucket {

        Address addr;

        Page * page;

        Bucket * next;
        
    };

    /**
     * class: HashMap
     * a simple hash map
     */

    class HashMap {
        public:

            /**
             * constructor
             * construct an empty map, set the buckets
             */

            HashMap(int capacity);

            //destructor
            ~HashMap();

            /**
             * method: get
             * get the value of the key, if key does not exist return nullptr
             */

            Page * get(Address addr);

            /**
             * method: put
             * put a key/value pair into the map
             */

            void put(Address addr, Page * page);

            /**
             * method: erase
             * erase a key/value pair from the map
             */

            void erase(Address addr);
        private:
            int _capacity;

            Bucket ** hash;

            int hash_code(Address addr)
            {
                return addr % _capacity;
            }

    };

}

#endif //LAINDB_HASHMAP_H_
