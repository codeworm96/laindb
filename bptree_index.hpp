#ifndef LAINDB_BPTREE_INDEX_HPP_
#define LAINDB_BPTREE_INDEX_HPP_

#include "mode.h"

namespace laindb {

    //the minimum # of keys in a node (except root)
    const int MIN_KEYS = 4
    //the maximum # of keys in a node
    const int MAX_KEYS = MIN_KEYS * 2 + 1;

    //NOTE: use typedef instead of template parameters now
    typedef int Key;
    typedef int Address;

    /*
     * Struct: BNode
     * A node of the b+tree
     */

    struct BNode {
        int id;  //the id of node to distinguish a node in the file
        int num; //# of keys
        bool is_leaf;

        //the keys
        Key keys[MAX_KEYS];
        //Note: in a inner node it represents subtrees
        //in a leaf, children[i] is the value that is associated with keys[i]
        Address children[MAX_KEYS + 1];
    }


    /*
     * Class: Bptree Index
     * A implementation of Index using b+tree
     */

    class BptreeIndex{
        public:

            BptreeIndex(const std::string & name, FileMode mode);
            ~BptreeIndex();
            Address get(const Key & key)
            void put(const Key & key, Address address) { table[key] = address; }
            void erase(const Key & key) { table.erase(key); }

        private:
            //the root of the b+tree, should be in the memory when the database is open
            BNode * root;
            IndexStore store;
    };

    BptreeIndex::BptreeIndex(const std::string & name, FileMode mode) :store(name, mode)
    {
        if(store.rootID){
            root = store.load(store.rootID);
        }else{
            root = nullptr;
        }
    }

    BptreeIndex::~BptreeIndex()
    {
        if(root){
            store.write(root);
            delete root;
        }
    }


}


#endif //LAINDB_BPTREE_INDEX_HPP_
