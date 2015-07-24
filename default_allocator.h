#ifndef LAINDB_DEFAULT_ALLOCATOR_H_ 
#define LAINDB_DEFAULT_ALLOCATOR_H_

#include <cstdint>

namespace laindb {

    typedef int Address;

    /*
     * struct: Block
     * represents a block in file
     */

    struct Block {
        //address of the block
        Address addr;

        //size of the block
        int16_t size;

        //pointer to next block;
        Block * next;

        Block(Address pos, int16_t s, Block * p):addr(pos), size(s), next(p) {}
    };
        


    /*
     * class: DefaultAllocator
     * An allocator using bestfit strategy.
     */

    class DefaultAllocator {
        public:

            /*
             * Constructor
             * construct a allocator
             * using name & mode
             */

            DefaultAllocator(const std::string & name, FileMode mode);

            /*
             * Destructor
             */

            ~DefaultAllocator();

            /*
             * method: alloc
             * allocate a block of size s
             * @return: the address of the block
             */

            Address alloc(int16_t s);
            
            /*
             * method: dealloc
             * free a block
             */

            void dealloc(Address addr, int16_t s);
        private:
            //size of the data file
            int size;

            //list of available blocks
            Block * avail;

            //name of the idle file
            std::string _name;

            //get the length of the list
            int length(Block * p);

            //reverse a list
            void reverse(Block * & list);

            //insert a block to the list, keeping the ascending order of size
            void insert_sort(Block * & list, Block * blk);

            //find the bestfit block
            Block * bestfit(Block * & list, int16_t size);

            //defragmentation
            void defragment(Block * & list);
    };

    DefaultAllocator::DefaultAllocator(const std::string & name, FileMode mode):size(0), avail(nullptr), _name(name)
    {
        if(mode & OPEN){
            FILE * idle_file = std::fopen(name.c_str(), "rb");
            if (idle_file){
                std::fread(&size, sizeof size, 1, idle_file); //load size
                int cnt;
                std::fread(&cnt, sizeof cnt, 1, idle_file); //load length of the list
                for (int i = 0; i < cnt; ++i){
                    int pos;
                    int s;
                    std::fread(&pos, sizeof pos, 1, idle_file); //load pos
                    std::fread(&s, sizeof s, 1, idle_file); //load size of the block
                    avail = new Block(pos, s, avail);
                    reverse(avail); //make the list in correct order
                }
                fclose(idle_file);
            }
        }
    }

    DefaultAllocator::~DefaultAllocator()
    {
        FILE * idle_file = std::fopen(_name.c_str(), "wb");
        std::fwrite(&size, sizeof size, 1, idle_file); //write size
        int cnt = length(avail);
        std::fwrite(&cnt, sizeof cnt, 1, idle_file); //write the length of the list
        Block * p = avail;
        while(p){
            int pos = p->addr;
            int s = p->size;
            std::fwrite(&pos, sizeof pos, 1, idle_file); //write address
            std::fwrite(&s, sizeof s, 1, idle_file); //write size
            Block * tmp = p;
            p = p->next;
            delete tmp;
        }
        fclose(idle_file);
    }

    Address DefaultAllocator::alloc(int16_t s)
    {
        Block * blk = bestfit(avail, s);
        if (!blk){ //if it fails, defragment & try again
            defragment(avail);
            blk = bestfit(avail, s);
        }

        if (blk){
            Address res = blk->addr;
            delete blk;
            return res;
        }else{
            Address res = size;
            size += s;
            return res;
        }
    }

    void DefaultAllocator::dealloc(Address addr, int16_t s)
    {
        Block * blk = new Block(addr, s, nullptr);
        insert_sort(avail, blk);
    }

    int DefaultAllocator::length(Block * p)
    {
        int res = 0;
        while(p){
            ++res;
            p = p->next;
        }
        return res;
    }

    void DefaultAllocator::reverse(Block * & list)
    {
        Block * p = list;
        Block * res = nullptr;
        while(p){
            Block * tmp = p->next;
            p->next = res;
            res = p;
            p = tmp;
        }
        list = res;
    }

    void DefaultAllocator::insert_sort(Block * & list, Block * blk)
    {
        Block * tmp = new Block(0, 0, list);
        Block * p = tmp;
        while(true){
            if (p->next == nullptr || blk->size < p->next->size){
                blk->next = p->next;
                p->next = blk;
                break;
            }
            p = p->next;
        }
        list = tmp->next;
        delete tmp;
    }

    Block * DefaultAllocator::bestfit(Block * & list, int16_t size)
    {
        Block * tmp = new Block(0, 0, list);
        Block * p = tmp;
        while(true){
            if(p->next == nullptr){
                delete tmp;
                return nullptr;
            }

            if (p->next->size >= size){
                Block * blk = p->next;
                p->next = blk->next;
                blk->next = nullptr;
                list = tmp->next;
                delete tmp;
                return blk;
            }
            p = p->next;
        }
    }

    void DefaultAllocator::defragment(Block * & list)
    {
        //sort the list by the ascending order of address (insert sort)
        Block * tmp = new Block(0, 0, nullptr);
        Block * cur = list;
        while(cur){
            Block * next = cur->next;
            Block * p = tmp;
            while(true){
                if (p->next == nullptr || cur->addr < p->next->addr){
                    cur->next = p->next;
                    p->next = cur;
                    break;
                }
                p = p->next;
            }
            cur = next;
        }
        cur = tmp->next;
        delete tmp;

        //merge blocks
        Block * head = cur;
        while(cur){
            if (cur->next && cur->addr + cur->size == cur->next->addr){
                cur->size += cur->next->size;
                Block * tmp = cur->next;
                cur->next = tmp->next;
                delete tmp;
            }else{
                cur = cur->next;
            }
        }

        //sort by ascending order of size
        list = nullptr;
        while(head){
            Block * p = head;
            head = head->next;
            insert_sort(list, p);
        }
    }

}

#endif //LAINDB_APPEND_ONLY_ALLOCATOR_H_
