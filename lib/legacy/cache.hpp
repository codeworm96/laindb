#ifndef LAINDB_CACHE_HPP_
#define LAINDB_CACHE_HPP_

#include "bnode.h"
#include "node_store.hpp"

namespace laindb {

    const int CACHESIZE = 6151;

    /**
     * Class: Cache
     * A simple cache for BNode
     */

    class Cache {
        public:

            /*
             * constructor
             */

            Cache(const std::string & name, FileMode mode) :store(name, mode)
            {
                for (int i = 0; i < CACHESIZE; ++i){
                    buf[i] = nullptr;
                }
            }

            ~Cache()
            {
                for (int i = 0; i < CACHESIZE; ++i){
                    if(buf[i]){
                        store.write(buf[i]);
                    }
                }
            }

            /*
             * Method: load
             * load a BNode from cache
             */
            
            BNode * load(int id)
            {
                int pos = id % CACHESIZE;
                if (buf[pos]){
                    if (buf[pos]->id == id){
                        BNode * res = buf[pos];
                        buf[pos] = nullptr;
                        return res;
                    }else{
                        return store.load(id);
                    }
                }else{
                    return store.load(id);
                }
            }

            /*
             * Method: write
             * write a BNode to cache
             */

            void write(BNode * node)
            {
                int pos = node->id % CACHESIZE;
                if (buf[pos]){
                    store.write(buf[pos]);
                }
                buf[pos] = node;
            }

            int getRootID() { return store.getRootID(); }

            void setRootID(int new_id) { store.setRootID(new_id); }

            int alloc() { return store.alloc(); }

            void dealloc(int id) { store.dealloc(id); }
            
        private:

            //the NodeStore
            NodeStore store;

            BNode * buf[CACHESIZE];

    };
}

#endif //LAINDB_CACHE_HPP_
