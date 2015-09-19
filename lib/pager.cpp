#include "pager.h"

#include <cstdio>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cstring>

#include "utility.hpp"
#include "page.h"
#include "hashmap.h"

namespace laindb {

    Pager::Pager(const std::string & name, FileMode mode, int size_limit)
        :file(nullptr), index(size_limit), max_size(size_limit), size(0) //Note: load factor of the hashmap is 1
    {
        if (mode & OPEN){
            file = std::fopen(name.c_str(), "r+b");
            _status = OPEN;
        }
        if (!file && (mode & NEW)){
            file = std::fopen(name.c_str(), "w+b");
            _status = NEW;
        }
        if (file){
            init_cache();
        }else{
            throw std::runtime_error("cannot open data file");
        }
    }

    Pager::~Pager()
    {
        clear_cache();
        std::fclose(file);
    }

    void Pager::init_cache()
    {
        //init the list as a empty list
        list_head = new Page;
        list_head->prev = list_head;
        list_head->next = list_head;
    }

    void Pager::clear_cache()
    {
        //clear the list
        while(list_head->next != list_head){
            evict(list_head->next);
        }
        delete list_head;
    }

    void Pager::write_to_disk(Page * page)
    {
        if (page->modified){ //write back
            std::fseek(file, page->address, SEEK_SET);
            std::fwrite(page->content, BLOCK_SIZE, 1, file);
        }
        delete page;
    }

    Page * Pager::read_from_disk(Address addr)
    {
        Page * res = new Page;
        res->modified = false;
        res->address = addr;
        std::fseek(file, addr, SEEK_SET);
        std::fread(res->content, BLOCK_SIZE, 1, file);

        //fail to read, often when the program wants to access address that is large than the size of the file
        if (std::ferror(file)){ 
            std::memset(res->content, 0, BLOCK_SIZE);
        }
        return res;
    }

    Address Pager::evict(Page * p)
    {
        //remove form list
        p->next->prev = p->prev;
        p->prev->next = p->next;

        Address res = p->address;
        write_to_disk(p);
        return res;
    }

    Page * Pager::fetch_page(Address addr)
    {
        Page * res = index.get(addr);
        if (res){ //cache hit
            //remove from list
            res->prev->next = res->next;
            res->next->prev = res->prev;

            //insert to head
            res->next = list_head->next;
            res->prev = list_head;
            res->prev->next = res;
            res->next->prev = res;

            return res;
        }else{ //cache miss
            if (size == max_size){ //replace
                Address tmp = evict(list_head->prev);
                index.erase(tmp);
                --size;
            }

            ++size;
            Page * res = read_from_disk(addr);

            //insert to head
            res->next = list_head->next;
            res->prev = list_head;
            res->prev->next = res;
            res->next->prev = res;

            index.put(addr, res);

            return res;
        }
    }

    void Pager::read(void * buf, int cnt, Address address)
    {
        //cursor
        char * cur = static_cast<char *>(buf);

        //calculate aligned address
        Address aligned_addr = address / BLOCK_SIZE * BLOCK_SIZE;
        int len = std::min(BLOCK_SIZE - (address - aligned_addr), cnt);
        Page * tmp = fetch_page(aligned_addr);
        memcpy(cur, tmp->content + (address - aligned_addr), len);
        cnt -= len;
        cur += len;
        while(cnt){
            aligned_addr += BLOCK_SIZE;
            len = std::min(BLOCK_SIZE, cnt);
            tmp = fetch_page(aligned_addr);
            memcpy(cur, tmp->content, len);
            cnt -= len;
            cur += len;
        }
    }

    void Pager::write(void * buf, int cnt, Address address)
    {
        //cursor
        char * cur = static_cast<char *>(buf);

        //calculate aligned address
        Address aligned_addr = address /  BLOCK_SIZE * BLOCK_SIZE;
        int len = std::min(BLOCK_SIZE - (address - aligned_addr), cnt);
        Page * tmp = fetch_page(aligned_addr);
        tmp->modified = true;
        memcpy(tmp->content + (address - aligned_addr), cur, len);
        cnt -= len;
        cur += len;
        while(cnt){
            aligned_addr += BLOCK_SIZE;
            len = std::min(BLOCK_SIZE, cnt);
            tmp = fetch_page(aligned_addr);
            tmp->modified = true;
            memcpy(tmp->content, cur, len);
            cnt -= len;
            cur += len;
        }
    }
}
