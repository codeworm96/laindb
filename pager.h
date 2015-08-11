#ifndef LAINDB_PAGER_H_
#define LAINDB_PAGER_H_

#include <cstdio>
#include <string>

#include "utility.h"

namespace laindb {

    const int CACHE_SIZE = 1000;

    /**
     * struct: Page
     * A page in file
     */

    struct Page {
        int address;

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

            Pager(const std::string & name, FileMode mode);

            /**
             * Destructor
             * close file
             */

            ~Pager();

            /**
             * method: read
             * from `address` read `cnt` bytes to `buf`
             */

            void read(char * buf, int cnt, Address address);

            /**
             * method: write 
             * write `cnt` bytes from `buf` to file at `address`
             */

            void write(char * buf, int cnt, Address address);

        private:
            //the file that the object manages
            FILE * file;

            //cache
            Page * cache[CACHE_SIZE];

            void init_cache();

            void clear_cache();

            void write_to_disk(Page * page);

            Page * read_from_disk(Address addr);

            Page * fetch_page(Address addr);
    };
}

#endif //LAINDB_PAGER_H_
