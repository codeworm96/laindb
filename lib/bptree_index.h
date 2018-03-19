#ifndef LAINDB_BPTREE_INDEX_H_
#define LAINDB_BPTREE_INDEX_H_

#include <vector>

#include "utility.hpp"
#include "bnode.h"
#include "node_store.h"


namespace laindb {

    const Address ENTRY_NOT_FOUND = -1;

    /**
     * Class: BptreeIter
     * (forward declaration)
     */

    class BptreeIter;

    /**
     * Class: Bptree Index
     * A implementation of Index using b+tree
     */

    class BptreeIndex{
        public:

            /**
             * constructor
             * construct a b+tree index using name & mode (default CREATE)
             */

            BptreeIndex(const std::string & name, FileMode mode = CREATE);
            
            //destructor
            ~BptreeIndex();

            /**
             * methdod: get
             * get address according to the key
             * returns ENTRY_NOT_FOUND if not found
             */

            Address get(const Key & key);

            /**
             * method: put
             * put a key/value pair
             * returns the old value, returns ENTRY_NOT_FOUND if key does not exist
             */

            Address put(const Key & key, Address address);

            /**
             * method: erase
             * erase a key/value pair
             * returns the old value, returns ENTRY_NOT_FOUND if key does not exitst
             */

            Address erase(const Key & key);

            typedef BptreeIter iterator;
            typedef std::pair<Key, Address> value_type;
            typedef std::input_iterator_tag iterator_category;
            iterator begin();
            iterator end();
        private:
            //the root of the b+tree, should be in the memory when the database is open
            BNode * root;

            //to store the nodes
            NodeStore store;

            // the leftest leaf
            int leftest;

            /*
             * method: search
             * search x in the keys
             * if found, returns the index
             * if not found, returns the index of the biggest key that is less than x (if does not exist, returns -1)
             */

            int search(BNode * node, const Key & x);

            /*
             * method: split
             * split children[pos] of node
             * left: children[pos]
             * return: the splited node that contains key
             */

            BNode * split(BNode * node, const Key & key, int pos, BNode * left);

            /*
             * method: replace_key
             * update keys that has been deleted
             */

            void replace_key(const Key & origin_key, const Key & new_key);

            /*
             * method: merge
             * merge two nodes
             * return: merged node
             */

            BNode * merge(BNode * p, int pos, BNode * l, BNode * r);

            /*
             * method: borrow_from_left
             * borrow a key from left node
             * return: the node that get one key
             */

            BNode * borrow_from_left(BNode * p, int pos, BNode * cur, BNode * l);

            /*
             * method: borrow_from_right
             * borrow a key from right node
             * return: the node that get one key
             */

            BNode * borrow_from_right(BNode * p, int pos, BNode * cur, BNode * r);

            /*
             * method: adjust_for_del
             * adjust the node p->children[pos] for deletion
             * return: the node that adjusted
             */

            BNode * adjust_for_del(BNode * p, int pos, BNode * cur);

            friend class BptreeIter;
    };

    /**
     * Class: BptreeIter
     * Iterator of the BptreeIndex
     */

    class BptreeIter {
        public:
            BptreeIter(BptreeIndex * _index, int _node_id, int _pos);
            bool operator==(const BptreeIter & other);
            bool operator!=(const BptreeIter & other) { return !(*this == other); }
            BptreeIter & operator++();
            std::pair<Key, Address> operator*();
        private:
            BptreeIndex * index;
            int node_id;
            int pos;
    };

}

#endif //LAINDB_BPTREE_INDEX_H_
