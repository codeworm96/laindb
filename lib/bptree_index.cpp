#include "bptree_index.h"

#include <vector>

#include "utility.hpp"
#include "bnode.h"
#include "node_store.h"


namespace laindb {

    BptreeIndex::BptreeIndex(const std::string & name, FileMode mode) :store(name, mode), leftest(0)
    {
        int rootID = store.getRootID();
        if(rootID){
            root = store.load(rootID);
        }else{
            root = nullptr;
        }

        if(root != nullptr){
            BNode * cur = root;
            while(!cur->is_leaf){
                BNode * next = store.load(cur->children[0]);
                if (cur != root){
                    store.write(cur);
                }
                cur = next;
            }
            leftest = cur->id;
            if (cur != root){
                store.write(cur);
            }
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
        //binary search
        int r = node->num - 1;
        int l = 0;
        while(l <= r){
            int m = (l + r) / 2;
            int cmp_res = key_cmp(node->keys[m], x);
            if (cmp_res == 0){ //equal
                return m;
            }else if (cmp_res > 0){ //node->keys[m] > x
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
            BNode * next = store.load(cur->children[pos]);
            if (cur != root){
                store.write(cur);
            }
            cur = next;
        }

        int pos = search(cur, key);

        Address res;
        if (pos < 0 || cur->keys[pos] != key){
            res =  ENTRY_NOT_FOUND;
        }else{
            res = cur->children[pos];
        }

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

            // maintain link
            right->children[right->num] = left->children[left->num];
            left->children[MIN_KEYS] = right->id;
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
            root->children[0] = 0; // link
            leftest = root->id;
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
                next = adjust_for_del(cur, pos, next);
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
        shift(cur->children + pos, cur->num - pos + 1); // maintain link
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

    BNode * BptreeIndex::adjust_for_del(BNode * p, int pos, BNode * cur)
    {
        BNode * l = nullptr;
        BNode * r = nullptr;
        if (pos > 0){
            l = store.load(p->children[pos - 1]);
        }
        if (pos < p->num){
            r = store.load(p->children[pos + 1]);
        }

        BNode * res = nullptr;
        if(l && l->num > MIN_KEYS){
            if(r){
                store.write(r);
            }
            res = borrow_from_left(p, pos, cur, l);
        } else if (r && r->num > MIN_KEYS){
            if(l){
                store.write(l);
            }
            res = borrow_from_right(p, pos, cur, r);
        } else if (l){
            if (r){
                store.write(r);
            }
            res = merge(p, pos - 1, l, cur);
        } else {
            if (l){
                store.write(l);
            }
            res = merge(p, pos, cur, r);
        }

        return res;
    }

    BNode * BptreeIndex::merge(BNode * p, int pos, BNode * l, BNode * r)
    {
        if (l->is_leaf){
            shift(p->keys + pos, p->num - pos);
            shift(p->children + (pos + 1), p->num - pos);
            p->modified = true;
            p->num--;
            item_move(l->keys + l->num, r->keys, r->num);
            item_move(l->children + l->num, r->children, r->num + 1); // maintain link
            l->modified = true;
            l->num += r->num;
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
        }
        store.dealloc(r->id);
        delete r;
        return l;
    }

    BNode * BptreeIndex::borrow_from_left(BNode * p, int pos, BNode * cur, BNode * l)
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
            l->children[l->num - 1] = l->children[l->num]; // maintain link
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
        return cur;
    }

    BNode * BptreeIndex::borrow_from_right(BNode * p, int pos, BNode * cur, BNode * r)
    {
        if(cur->is_leaf){
            cur->keys[cur->num] = r->keys[0];
            cur->children[cur->num] = r->children[0];
            cur->num++;
            cur->modified = true;
            shift(r->keys, r->num);
            shift(r->children, r->num + 1); // maintain link
            r->num--;
            r->modified = true;
            p->keys[pos] = r->keys[0];
            p->modified = true;
        }else{
            cur->keys[cur->num] = p->keys[pos];
            p->keys[pos] = r->keys[0];
            cur->children[cur->num + 1] = r->children[0];
            cur->num++;
            cur->modified = true;
            p->modified = true;
            shift(r->keys, r->num);
            shift(r->children, r->num + 1);
            r->num--;
            r->modified = true;
        }

        store.write(r);
        return cur;
    }

    BptreeIndex::iterator BptreeIndex::begin() {
        return iterator(this, leftest, 0);
    }

    BptreeIndex::iterator BptreeIndex::end() {
        return iterator(this, 0, 0);

    }

    BptreeIter::BptreeIter(BptreeIndex * _index, int _node_id, int _pos): index(_index), node_id(_node_id), pos(_pos) {}

    bool BptreeIter::operator==(const BptreeIter & other) {
        if (index == other.index) {
            if (node_id > 0) {
                return node_id == other.node_id && pos == other.pos;
            } else {
                return node_id == other.node_id;
            }
        } else {
            return false;
        }
    }

    BptreeIter & BptreeIter::operator++() {
        if (node_id > 0) {
            BNode * cur = index->store.load(node_id);
            ++pos;
            if (pos == cur->num) {
                pos = 0;
                node_id = cur->children[cur->num];
            }
            index->store.write(cur);
        }
        return *this;
    }

    std::pair<Key, Address> BptreeIter::operator*() {
        BNode * cur = index->store.load(node_id);
        std::pair<Key, Address> res = std::make_pair(cur->keys[pos], cur->children[pos]);
        index->store.write(cur);
        return res;
    }
}
