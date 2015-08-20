#include "hashmap.h"

#include "page.h"

namespace laindb {
    HashMap::HashMap(int capacity):_capacity(capacity)
    {
        hash = new Bucket*[capacity];
        for (int i = 0; i < capacity; ++i){
            hash[i] = nullptr;
        }
    }

    HashMap::~HashMap()
    {
        for (int i = 0; i < _capacity; ++i){
            Bucket * p;
            while(hash[i]){
                p = hash[i];
                hash[i] = p->next;
                delete p;
            }
        }
        delete hash;
    }

    Page * HashMap::get(Address addr)
    {
        int pos = hash_code(addr);
        Page * res = nullptr;
        Bucket * p = hash[pos];
        while(p){
            if (p->addr == addr){
                res = p->page;
                break;
            }
            p = p->next;
        }
        return res;
    }

    void HashMap::put(Address addr, Page * page)
    {
        int pos = hash_code(addr);
        Bucket * p = new Bucket;
        p->addr = addr;
        p->page = page;
        p->next = hash[pos];
        hash[pos] = p;
    }

    void HashMap::erase(Address addr)
    {
        int pos = hash_code(addr);
        Bucket ** p = &hash[pos];
        while(*p){
            if((*p)->addr == addr){
                Bucket * tmp = *p;
                *p = tmp->next;
                delete tmp;
                break;
            }
            p = &((*p)->next);
        }
    }

}
