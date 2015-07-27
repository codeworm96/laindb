#ifndef LAINDB_BPTREE_INDEX_HPP_
#define LAINDB_BPTREE_INDEX_HPP_

#include <vector>

#include "utility.h"
#include "bnode.h"
#include "node_store.hpp"


namespace laindb {

    const Address NEW_ENTRY = -1;

    /*
    class IndexStore {
        public:
            IndexStore(std::string, FileMode) :rootID(0), table(1){}
            int alloc() { int res = table.size(); table.resize(res + 1); return res; }
            BNode * load(int x) { return table[x]; }
            void write(BNode * node) { table[node->id] = node; }
            int rootID;
        private:
            std::vector<BNode *> table;
    };
    */

    /*
     * Class: Bptree Index
     * A implementation of Index using b+tree
     */

    class BptreeIndex{
        public:

            BptreeIndex(const std::string & name, FileMode mode = CREATE);
            ~BptreeIndex();
            Address get(const Key & key);
            Address put(const Key & key, Address address);
            void erase(const Key & key) {}

        private:
            //the root of the b+tree, should be in the memory when the database is open
            BNode * root;

            //to store the nodes
            NodeStore store;

            /*
             * method: search
             * search x in the keys
             * if found, returns the index
             * if not found, returns the index of the biggest key that is less than x (if there is not, returns -1)
             */

            int search(BNode * node, const Key & x);

            /*
             * method: split
             * split children[pos] of node
             */

            void split(BNode * node, int pos, BNode * left);
    };

    BptreeIndex::BptreeIndex(const std::string & name, FileMode mode) :store(name, mode)
    {
        int rootID = store.getRootID();
        if(rootID){
            root = store.load(rootID);
        }else{
            root = nullptr;
        }
    }

    BptreeIndex::~BptreeIndex()
    {
        if(root){
            store.write(root);
        }
    }

    int BptreeIndex::search(BNode * node, const Key & x)
    {
        int r = node->num - 1;
        int l = 0;
        while(l <= r){
            int m = (l + r) / 2;
            if (node->keys[m] == x){
                return m;
            }else if (node->keys[m] > x){
                r = m - 1;
            }else{
                l = m + 1;
            }
        }
        return r;
    }

    Address BptreeIndex::get(const Key & key)
    {
        if(root == nullptr){
            throw std::runtime_error("not found");
        }

        BNode * cur = root;
        while(!cur->is_leaf){
            int pos = search(cur, key);
            ++pos;
            if (cur != root){
                BNode * tmp = cur;
                cur = store.load(cur->children[pos]);
                store.write(tmp);
            }else{
                cur = store.load(cur->children[pos]);
            }
        }

        int pos = search(cur, key);

        if (pos < 0 || cur->keys[pos] != key){
            throw std::runtime_error("not found");
        }

        Address res = cur->children[pos];

        if (cur != root){
            store.write(cur);
        }

        return res;
    }

    void BptreeIndex::split(BNode * node, int pos, BNode * left)
    {
        //point arithmetic
        unshift(node->keys + pos, node->num - pos);
        unshift(node->children + (pos + 1), node->num - pos);
        node->modified = true;

        node->num++;
        BNode * right = new BNode;
        right->id = store.alloc();
        right->modified = true;
        node->keys[pos] = left->keys[MIN_KEYS];
        node->children[pos + 1] = right->id;

        if (left->is_leaf){
            right->is_leaf = true;
            right->num = left->num - MIN_KEYS;
            item_move(right->keys, left->keys + MIN_KEYS, right->num);
            item_move(right->children, left->children + MIN_KEYS, right->num);
        }else{
            right->is_leaf = false;
            right->num = left->num - MIN_KEYS - 1;
            item_move(right->keys, left->keys + (MIN_KEYS + 1), right->num);
            item_move(right->children, left->children + (MIN_KEYS + 1), right->num + 1);
        }

        left->num = MIN_KEYS;
        left->modified = true;

        store.write(right);
    }

    Address BptreeIndex::put(const Key & key, Address address)
    {
        if(root == nullptr){
            root = new BNode;
            root->id = store.alloc();
            store.setRootID(root->id);
            root->modified = true;
            root->is_leaf = true;
            root->num = 0;
        }

        if(root->num == MAX_KEYS){
            BNode * old_root = root;
            root = new BNode;
            root->id = store.alloc();
            store.setRootID(root->id);
            root->modified = true;
            root->is_leaf = false;
            root->num = 0;
            root->children[0] = old_root->id; 
            split(root, 0, old_root);
            store.write(old_root);
        }

        BNode * cur = root;
        while(!cur->is_leaf){
            int pos = search(cur, key);
            ++pos;
            BNode * next = store.load(cur->children[pos]);
            if (next->num >= MAX_KEYS){
                split(cur, pos, next);
                store.write(next);
                pos = search(cur, key);
                ++pos;
                next = store.load(cur->children[pos]);
            }
            
            if (cur != root){
                store.write(cur);
            }

            cur = next;
        }

        int pos = search(cur, key);
        Address res = NEW_ENTRY;
        if (pos >= 0 && cur->keys[pos] == key){
            res = cur->children[pos];
            cur->children[pos] = address;
        }else{
            ++pos;
            unshift(cur->keys + pos, cur->num - pos);
            unshift(cur->children + pos, cur->num - pos);
            cur->keys[pos] = key;
            cur->children[pos] = address;
            cur->num++;
        }
        cur->modified = true;

        if (cur != root){
            store.write(cur);
        }

        return res;
    }

}


#endif //LAINDB_BPTREE_INDEX_HPP_
