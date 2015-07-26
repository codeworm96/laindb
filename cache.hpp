#ifndef LAINDB_CACHE_HPP_
#define LAINDB_CACHE_HPP_

#include "bptree_index.hpp"
#include "node_store.hpp"

namespace laindb {

    /*
     * Class: Cache
     * A simple cache for BNode
     * do nothing now
     */

    class Cache {
        public:

            /*
             * constructor
             * associate cache with a NodeStore
             */

            Cache(NodeStore * ns) :store(ns) {}

            //the NodeStore will be handled by other code, so we do not
            //need a destructor
            
            /*
             * Method: load
             * load a BNode from cache
             */
            
            BNode * load(int id) { return store->load(id); }

            /*
             * Method: write
             * write a BNode to cache
             */

            void write(BNode * node)
            {
                if (node->modified){
                    store->write(node);
                }else{
                    delete node;
                }
            }
            
        private:

            //the NodeStore
            NodeStore * store;

    };
}

#endif //LAINDB_CACHE_HPP_
