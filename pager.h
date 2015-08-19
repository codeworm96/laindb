#ifndef LAINDB_PAGER_H_
#define LAINDB_PAGER_H_

#include <cstdio>
#include <string>
#include <map>

#include "utility.h"

namespace laindb {

    /**
     * struct: Page
     * A page in file
     */

    struct Page {
        bool modified;

        int address;

        Page * prev;

        Page * next;

        char content[BLOCK_SIZE];
    };
        
     
    /**
     * class: Pager
     * manages the pages (as a cache)
     */

    class Pager {
        public:

            /**
             * Constructor:
             * construct a pager for file `name`
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
            std::map<Address, Page *> index;

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
