#ifndef LAINDB_PAGE_H_
#define LAINDB_PAGE_H_

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
}

#endif //LAINDB_PAGE_H_
     