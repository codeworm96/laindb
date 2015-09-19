#ifndef LAINDB_NODE_STORE_H_
#define LAINDB_NODE_STORE_H_

#include <string>
#include <vector>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <type_traits>

#include "bnode.h"
#include "pager.h"
#include "utility.hpp"

namespace laindb {

    //space for a node
    const int NODE_SIZE = BLOCK_SIZE;
    
    //cache size
    const int NODE_CACHE_SIZE = 3079;

    //restrains for BNode
    STATIC_ASSERT(NODE_SIZE >= sizeof(BNode));
    STATIC_ASSERT(std::is_pod<BNode>::value);

    /**
     * class: NodeStore
     * store BNodes in a file
     */

    class NodeStore {
        public:

            /**
             * constructor
             * construct a node store
             * using name `file_name` & mode
             */

            NodeStore(std::string file_name, FileMode mode);

            //destructor
            ~NodeStore();

            /**
             * method: alloc
             * allocate a block for use
             */

            int alloc();
            
            /**
             * method: dealloc
             * deallocate a block;
             */

            void dealloc(int id) { blocks.push_back(id); }

            /*
             * method: write
             * write a node to disk
             * Side effect: node will be deleted
             */

            void write(BNode * node);

            /*
             * method: load
             * load a node from file
             */

            BNode * load(int id);

            /*
             * method: getRootID
             * getter for rootID
             */

            int getRootID() { return rootID; }

            /*
             * method: setRootID
             * setter for rootID
             */

            void setRootID(int new_id) { rootID = new_id; }

        private:
            //name
            std::string name;

            //pager
            Pager pager;

            //available blocks
            std::vector<int> blocks;

            //# of blocks in file
            int size;

            //id of root of the b+tree
            int rootID;
    };

}

#endif //LAINDB_NODE_STORE_H_
