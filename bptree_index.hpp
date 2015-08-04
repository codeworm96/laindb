#ifndef LAINDB_BPTREE_INDEX_HPP_
#define LAINDB_BPTREE_INDEX_HPP_

#include <vector>

#include "utility.h"
#include "bnode.h"
#include "node_store.hpp"


namespace laindb {

    const Address ENTRY_NOT_FOUND = -1;

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
            Address erase(const Key & key);

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
             */

            void merge(BNode * p, int pos, BNode * l, BNode * r);

            /*
             * method: borrow_from_left
             * borrow a key from left node
             */

            void borrow_from_left(BNode * p, int pos, BNode * cur, BNode * l);

            /*
             * method: borrow_from_right
             * borrow a key from right node
             */

            void borrow_from_right(BNode * p, int pos, BNode * cur, BNode * r);

            /*
             * method: adjust_for_del
             * adjust the node p->children[pos] for deletion
             */

            void adjust_for_del(BNode * p, int pos);

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
            return ENTRY_NOT_FOUND;
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
            return ENTRY_NOT_FOUND;
        }

        Address res = cur->children[pos];

        if (cur != root){
            store.write(cur);
        }

        return res;
    }

    BNode * BptreeIndex::split(BNode * node, const Key & key, int pos, BNode * left)
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

        if(key < node->keys[pos]){
            store.write(right);
            return left;
        }else{
            store.write(left);
            return right;
        }
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

        BNode * cur = root;

        if(root->num == MAX_KEYS){
            BNode * old_root = root;
            root = new BNode;
            root->id = store.alloc();
            store.setRootID(root->id);
            root->modified = true;
            root->is_leaf = false;
            root->num = 0;
            root->children[0] = old_root->id; 
            cur = split(root, key, 0, old_root);
        }

        while(!cur->is_leaf){
            int pos = search(cur, key);
            ++pos;
            BNode * next = store.load(cur->children[pos]);
            if (next->num >= MAX_KEYS){
                next = split(cur, key, pos, next);
            }
            
            if (cur != root){
                store.write(cur);
            }

            cur = next;
        }

        int pos = search(cur, key);
        Address res = ENTRY_NOT_FOUND;
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

    Address BptreeIndex::erase(const Key & key)
    {
        if(root == nullptr){
            return ENTRY_NOT_FOUND;
        }

        BNode * cur = root;
        while(!cur->is_leaf){
            int pos = search(cur, key);
            ++pos;
            BNode * next = store.load(cur->children[pos]);
            if(next->num <= MIN_KEYS){
                store.write(next);
                adjust_for_del(cur, pos);
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

        //not found
        if (pos < 0 || cur->keys[pos] != key){
            if (cur != root){
                store.write(cur);
            }
            return ENTRY_NOT_FOUND;
        }

        if (pos == 0 && cur->num > 1){
            replace_key(cur->keys[0], cur->keys[1]);
        }

        int res = cur->children[pos];

        shift(cur->keys + pos, cur->num - pos);
        shift(cur->children + pos, cur->num - pos);
        cur->num--;
        cur->modified = true;

        if (cur != root){
            store.write(cur);
        }

        if (root->num == 0){
            if (root->is_leaf){
                store.dealloc(root->id);
                delete root;
                root = nullptr;
                store.setRootID(0);
            }else{
                BNode * tmp = store.load(root->children[0]);
                store.dealloc(root->id);
                delete root;
                root = tmp;
                store.setRootID(root->id);
            }
        }

        return res;
    }

    void BptreeIndex::replace_key(const Key & origin_key, const Key & new_key)
    {
        if(root == nullptr){
            return ;
        }

        BNode * cur = root;
        while(!cur->is_leaf){
            int pos = search(cur, origin_key);
            if (pos >= 0 && cur->keys[pos] == origin_key){
                cur->keys[pos] = new_key;
                cur->modified = true;
                break;
            }
            ++pos;
            if (cur != root){
                BNode * tmp = cur;
                cur = store.load(cur->children[pos]);
                store.write(tmp);
            }else{
                cur = store.load(cur->children[pos]);
            }
        }

        if (cur != root){
            store.write(cur);
        }
    }

    void BptreeIndex::adjust_for_del(BNode * p, int pos)
    {
        BNode * cur = store.load(p->children[pos]);
        BNode * l = nullptr;
        BNode * r = nullptr;
        if (pos > 0){
            l = store.load(p->children[pos - 1]);
        }
        if (pos < p->num){
            r = store.load(p->children[pos + 1]);
        }

        if(l && l->num > MIN_KEYS){
            borrow_from_left(p, pos, cur, l);
        }else if (r && r->num > MIN_KEYS){
            borrow_from_right(p, pos, cur, r);
        }else if (l){
            merge(p, pos - 1, l, cur);
        }else{
            merge(p, pos, cur, r);
        }

    }

    void BptreeIndex::merge(BNode * p, int pos, BNode * l, BNode * r)
    {
        if (l->is_leaf){
            shift(p->keys + pos, p->num - pos);
            shift(p->children + (pos + 1), p->num - pos);
            p->modified = true;
            p->num--;
            item_move(l->keys + l->num, r->keys, r->num);
            item_move(l->children + l->num, r->children, r->num);
            l->modified = true;
            l->num += r->num;
            store.dealloc(r->id);
            delete r;
            store.write(l);
        }else{
            l->keys[l->num] = p->keys[pos];
            l->num++;
            shift(p->keys + pos, p->num - pos);
            shift(p->children + (pos + 1), p->num - pos);
            p->modified = true;
            p->num--;
            item_move(l->keys + l->num, r->keys, r->num);
            item_move(l->children + l->num, r->children, r->num + 1);
            l->modified = true;
            l->num += r->num;
            store.dealloc(r->id);
            delete r;
            store.write(l);
        }
    }

    void BptreeIndex::borrow_from_left(BNode * p, int pos, BNode * cur, BNode * l)
    {
        if(cur->is_leaf){
            p->keys[pos - 1] = l->keys[l->num - 1];
            p->modified = true;
            unshift(cur->keys, cur->num);
            unshift(cur->children, cur->num);
            cur->num++;
            cur->modified = true;
            cur->keys[0] = l->keys[l->num - 1];
            cur->children[0] = l->children[l->num - 1];
            l->num--;
            l->modified = true;
        }else{
            unshift(cur->keys, cur->num);
            unshift(cur->children, cur->num + 1);
            cur->num++;
            cur->modified = true;
            cur->keys[0] = p->keys[pos - 1];
            p->keys[pos - 1] = l->keys[l->num - 1];
            p->modified = true;
            cur->children[0] = l->children[l->num];
            l->num--;
            l->modified = true;
        }

        store.write(l);
        store.write(cur);
    }

    void BptreeIndex::borrow_from_right(BNode * p, int pos, BNode * cur, BNode * r)
    {
        if(cur->is_leaf){
            cur->keys[cur->num] = r->keys[0];
            cur->children[cur->num] = r->children[0];
            cur->num++;
            cur->modified = true;
            shift(r->keys, r->num);
            shift(r->children, r->num);
            r->num--;
            r->modified = true;
            p->keys[pos] = r->keys[0];
            p->modified = true;
        }else{
            cur->keys[cur->num] = p->keys[pos];
            p->keys[pos] = r->keys[0];
            cur->children[cur->num + 1] = r->children[0];
            shift(r->keys, r->num);
            shift(r->children, r->num + 1);
            cur->num++;
            cur->modified = true;
            p->modified = true;
            r->num--;
            r->modified = true;
        }

        store.write(r);
        store.write(cur);
    }

}


#endif //LAINDB_BPTREE_INDEX_HPP_
