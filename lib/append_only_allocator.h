#ifndef LAINDB_APPEND_ONLY_ALLOCATOR_H_ 
#define LAINDB_APPEND_ONLY_ALLOCATOR_H_

#include "utility.hpp"

namespace laindb {

    /**
     * class: AppendOnlyAllocator
     * An allocator that simply append data to the end of file.
     */

    class AppendOnlyAllocator {
        public:

            /**
             * Constructor
             * construct a allocator
             * using name & mode
             */

            AppendOnlyAllocator(const std::string & name, FileMode mode);

            //Destructor
            ~AppendOnlyAllocator();

            /**
             * method: alloc
             * allocate a block of size s
             * @return: the address of the block
             */

            Address alloc(int16_t s) { Address res = size; size += s; return res; }
            
            /**
             * method: dealloc
             * free a block
             */

            void dealloc(Address addr, int16_t s) {}
        private:
            //size of the data file
            int size;

            //name of the idle file
            std::string _name;
    };

    AppendOnlyAllocator::AppendOnlyAllocator(const std::string & name, FileMode mode):_name(name), size(0)
    {
        if(mode & OPEN){
            FILE * idle_file = std::fopen(name.c_str(), "rb");
            if (idle_file){
                std::fread(&size, sizeof(size), 1, idle_file); //load size
                fclose(idle_file);
            }
        }
    }

    AppendOnlyAllocator::~AppendOnlyAllocator()
    {
        FILE * idle_file = std::fopen(_name.c_str(), "wb");
        std::fwrite(&size, sizeof(size), 1, idle_file); //write size
        fclose(idle_file);
    }
}

#endif //LAINDB_APPEND_ONLY_ALLOCATOR_H_
