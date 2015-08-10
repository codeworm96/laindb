#ifndef LAINDB_NODE_STORE_HPP_
#define LAINDB_NODE_STORE_HPP_

#include <string>
#include <vector>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>

#include "bptree_index.hpp"
#include "utility.h"

namespace laindb {

    //block size of the disk
    const int BLOCK_SIZE = 4096;

    /**
     * class: NodeStore
     * store BNodes in a file
     */

    class NodeStore {
        public:

            /**
             * constructor
             * construct a node store
             * using name `file_name` & mode
             */

            NodeStore(std::string file_name, FileMode mode);

            //destructor
            ~NodeStore();

            /**
             * method: alloc
             * allocate a block for use
             */

            int alloc();
            
            /**
             * method: dealloc
             * deallocate a block;
             */

            void dealloc(int id);

            /*
             * method: write
             * write a node to disk
             * Side effect: node will be deleted
             */

            void write(BNode * node);

            /*
             * method: load
             * load a node from file
             */

            BNode * load(int id);

            /*
             * method: getRootID
             * getter for rootID
             */

            int getRootID() { return rootID; }

            /*
             * method: setRootID
             * setter for rootID
             */

            void setRootID(int new_id) { rootID = new_id; }

        private:
            //name
            std::string name;

            //file
            FILE * file;

            //available blocks
            std::vector<int> blocks;

            //# of blocks in file
            int size;

            //id of root of the b+tree
            int rootID;
    };

    NodeStore::NodeStore(std::string file_name, FileMode mode) :name(file_name), file(nullptr), size(0), rootID(0)
    {
        std::string idle_name = std::string("idle_") + name;
        if(mode & OPEN){
            file = std::fopen(name.c_str(), "r+b");
        }
        if(file){
            FILE * idle_file = std::fopen(idle_name.c_str(), "rb");
            if (idle_file){
                std::fread(&rootID, sizeof(rootID), 1, file);
                std::fread(&size, sizeof(size), 1, file);

                int num;
                std::fread(&num, sizeof(num), 1, idle_file);
                int t;
                for (int i = 0; i < num; ++i){
                    std::fread(&t, sizeof(t), 1, idle_file);
                    blocks.push_back(t);
                }
                std::fclose(idle_file);
            }else{
                throw std::runtime_error("error when opening index file");
            }
        }else{
            if (mode & NEW){
                file = std::fopen(name.c_str(), "w+b");
            }
            if (!file){
                throw std::runtime_error("error when opening index file");
            }
        }
    }

    NodeStore::~NodeStore()
    {
        std::string idle_name = std::string("idle_") + name;
        std::fseek(file, 0, SEEK_SET);
        std::fwrite(&rootID, sizeof(rootID), 1, file);
        std::fwrite(&size, sizeof(size), 1, file);
        std::fclose(file);

        FILE * idle_file = std::fopen(idle_name.c_str(), "wb");
        int num = blocks.size();
        std::fwrite(&num, sizeof(num), 1, idle_file); 
        for (int i = 0; i < num; ++i){
            std::fwrite(&blocks[i], sizeof(blocks[i]), 1, idle_file);
        }
        std::fclose(idle_file);
    }

    int NodeStore::alloc()
    {
        if(blocks.empty()){
            return ++size;
        }else{
            int res = blocks.back();
            blocks.pop_back();
            return res;
        }
    }

    void NodeStore::dealloc(int id)
    {
        blocks.push_back(id);
    }

    void NodeStore::write(BNode * node)
    {
        if (!node->modified){
            delete node;
            return;
        }

        char * raw = static_cast<char *>(std::malloc(BLOCK_SIZE));
        char * cur = raw;
        std::memcpy(cur, &node->num, sizeof(node->num));
        cur += sizeof(node->num);
        std::memcpy(cur, &node->is_leaf, sizeof(node->is_leaf));
        cur += sizeof(node->is_leaf);
        std::memcpy(cur, node->keys, sizeof(Key) * node->num);
        cur += sizeof(Key) * node->num;

        int num = node->num;
        if(!node->is_leaf){
            num++;
        }

        std::memcpy(cur, node->children, sizeof(Address) * num);

        std::fseek(file, BLOCK_SIZE * node->id, SEEK_SET);
        std::fwrite(raw, BLOCK_SIZE, 1, file);
        std::free(raw);
        delete node;
    }

    BNode * NodeStore::load(int id)
    {
        BNode * res = new BNode;
        res->id = id;
        res->modified = false;

        char * raw = static_cast<char *>(std::malloc(BLOCK_SIZE));
        std::fseek(file, BLOCK_SIZE * id, SEEK_SET);
        std::fread(raw, BLOCK_SIZE, 1, file);
        char * cur = raw;

        std::memcpy(&res->num, cur, sizeof(res->num));
        cur += sizeof(res->num);
        std::memcpy(&res->is_leaf, cur, sizeof(res->is_leaf));
        cur += sizeof(res->is_leaf);
        std::memcpy(res->keys, cur, sizeof(Key) * res->num);
        cur += sizeof(Key) * res->num;

        int num = res->num;
        if(!res->is_leaf){
            num++;
        }

        std::memcpy(res->children, cur, sizeof(Address) * num);

        std::free(raw);
        return res;
    }

}

#endif //LAINDB_NODE_STORE_HPP_
