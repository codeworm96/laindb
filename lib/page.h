#ifndef LAINDB_PAGE_H_
#define LAINDB_PAGE_H_

#include "utility.hpp"

namespace laindb {
    
    /**
     * struct: Page
     * A page in file
     */

    struct Page {
        bool modified;

        //address in file
        int address;

        //for link list
        Page * prev;
        Page * next;

        char content[BLOCK_SIZE];
    };
}

#endif //LAINDB_PAGE_H_
     
