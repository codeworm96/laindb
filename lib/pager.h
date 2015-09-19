#ifndef LAINDB_PAGER_H_
#define LAINDB_PAGER_H_

#include <cstdio>
#include <string>

#include "utility.hpp"
#include "page.h"
#include "hashmap.h"

namespace laindb {

    /**
     * class: Pager
     * manages the pages (as a cache)
     * using doubly circular linked list & hashmap
     * adopted the LRU algorithm
     */

    class Pager {
        public:

            /**
             * Constructor:
             * construct a pager for file `name`
             * size_limit: max size for the cache
             */

            Pager(const std::string & name, FileMode mode, int size_limit);

            /**
             * Destructor
             * close file
             */

            ~Pager();

            /**
             * method: read
             * from `address` read `cnt` bytes to `buf`
             */

            void read(void * buf, int cnt, Address address);

            /**
             * method: write 
             * write `cnt` bytes from `buf` to file at `address`
             */

            void write(void * buf, int cnt, Address address);

            /**
             * method: status
             * returns the status of the file
             */

            FileMode status() { return _status; }

        private:
            //the file that the object manages
            FILE * file;

            //max size
            int max_size;

            //size of cache
            int size;

            //index of block
            HashMap index;

            //head of block list
            Page * list_head;

            //status of the file
            FileMode _status;

            void init_cache();

            void clear_cache();

            void write_to_disk(Page * page);

            Page * read_from_disk(Address addr);

            Page * fetch_page(Address addr);

            Address evict(Page * p);
    };
}

#endif //LAINDB_PAGER_H_
