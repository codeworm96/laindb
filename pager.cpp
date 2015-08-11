#include "pager.h"

#include <cstdio>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cstring>

#include "utility.h"

namespace laindb {

    Pager::Pager(const std::string & name, FileMode mode):file(nullptr)
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
        for (int i = 0; i < CACHE_SIZE; ++i){
            cache[i] = nullptr;
        }
    }

    void Pager::clear_cache()
    {
        for (int i = 0; i < CACHE_SIZE; ++i){
            if (cache[i]){
                write_to_disk(cache[i]);
            }
        }
    }

    void Pager::write_to_disk(Page * page)
    {
        std::fseek(file, page->address, SEEK_SET);
        std::fwrite(page->content, BLOCK_SIZE, 1, file);
        delete page;
    }

    Page * Pager::read_from_disk(Address addr)
    {
        Page * res = new Page;
        res->address = addr;
        std::fseek(file, addr, SEEK_SET);
        std::fread(res->content, BLOCK_SIZE, 1, file);
        if (std::ferror(file)){
            std::memset(res->content, 0, BLOCK_SIZE);
        }
        return res;
    }

    Page * Pager::fetch_page(Address addr)
    {
        int pos = addr % CACHE_SIZE;
        if (cache[pos]){
            if (cache[pos]->address != addr){
                write_to_disk(cache[pos]);
                cache[pos] = read_from_disk(addr);
            }
        }else{
            cache[pos] = read_from_disk(addr);
        }
        return cache[pos];
    }

    void Pager::read(void * buf, int cnt, Address address)
    {
        char * cur = static_cast<char *>(buf);
        Address aligned_addr = address /  BLOCK_SIZE * BLOCK_SIZE;
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
        char * cur = static_cast<char *>(buf);
        Address aligned_addr = address /  BLOCK_SIZE * BLOCK_SIZE;
        int len = std::min(BLOCK_SIZE - (address - aligned_addr), cnt);
        Page * tmp = fetch_page(aligned_addr);
        memcpy(tmp->content + (address - aligned_addr), cur, len);
        cnt -= len;
        cur += len;
        while(cnt){
            aligned_addr += BLOCK_SIZE;
            len = std::min(BLOCK_SIZE, cnt);
            tmp = fetch_page(aligned_addr);
            memcpy(tmp->content, cur, len);
            cnt -= len;
            cur += len;
        }
    }
}
