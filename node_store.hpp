#ifndef LAINDB_NODE_STORE_HPP_
#define LAINDB_NODE_STORE_HPP_

#include <string>
#include <vector>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>

#include "bptree_index.hpp"
#include "utility.h"
#include "bytes.h"
#include "default_serializer.hpp"

namespace laindb {

    const int BLOCK_SIZE = 4096;

    const int KEY_SIZE = 16;

    const char ZERO = 0;

    /*
     * class: NodeStore
     * store BNodes in a file
     */

    class NodeStore {
        public:

            /*
             * constructor
             * construct a node store
             */

            NodeStore(std::string file_name, FileMode mode);

            //destructor
            ~NodeStore();

            /*
             * method: alloc
             * allocate a block for use
             */

            int alloc();
            
            /*
             * method: dealloc
             * deallocate a ablock;
             */

            void dealloc(int id);

            /*
             * method: write
             * write a node to disk
             */

            void write(BNode * node);

            /*
             * method: load
             * load a node from file
             */

            BNode * load(int id);

            //id of root of the b+tree
            int rootID;

        private:
            //name
            std::string name;

            //file
            FILE * file;

            //available blocks
            std::vector<int> blocks;

            //# of blocks in file
            int size;

    };

    NodeStore::NodeStore(std::string file_name, FileMode mode) :name(file_name), file(nullptr), size(0)
    {
        std::string idle_name = std::string("idle_") + name;
        if(mode & OPEN){
            file = std::fopen(name.c_str(), "r+b");
        }
        if(file){
            FILE * idle_file = std::fopen(idle_name.c_str(), "r+b");
            if (idle_file){
                std::fread(&rootID, sizeof rootID, 1, file);
                std::fread(&size, sizeof size, 1, file);
                int num;
                std::fread(&num, sizeof num, 1, idle_file);
                for (int i = 0; i < num; ++i){
                    int t;
                    std::fread(&t, sizeof t, 1, idle_file);
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
            if (file){
                rootID = 0;
                size = 0;
            }else{
                throw std::runtime_error("error when opening index file");
            }
        }
    }

    NodeStore::~NodeStore()
    {
        std::cout<<"begin";
        std::string idle_name = std::string("idle_") + name;
        std::fseek(file, 0, SEEK_SET);
        std::fwrite(&rootID, sizeof rootID, 1, file);
        std::fwrite(&size, sizeof size, 1, file);

        FILE * idle_file = std::fopen(idle_name.c_str(), "wb");
        int num = blocks.size();
        std::fwrite(&num, sizeof num, 1, idle_file); 
        for (int i = 0; i < num; ++i){
            std::fwrite(&blocks[i], sizeof blocks[i], 1, idle_file);
        }
        fclose(idle_file);
        std::cout<<"end";
    }

    int NodeStore::alloc()
    {
        if(blocks.empty()){
            ++size;
            std::fseek(file, BLOCK_SIZE * size, SEEK_SET);
            std::fwrite(&ZERO, sizeof ZERO, BLOCK_SIZE, file);
            return size;
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
        char * raw = static_cast<char *>(std::malloc(BLOCK_SIZE));
        char * cur = raw;
        std::memcpy(cur, &node->num, sizeof node->num);
        cur += sizeof node->num;
        std::memcpy(cur, &node->is_leaf, sizeof node->is_leaf);
        cur += sizeof node->is_leaf;
        for (int i = 0; i < node->num; ++i){
            Bytes bytes = DefaultSerializer::serialize(node->keys[i]);
            if (bytes.size + sizeof bytes.size <= KEY_SIZE){
                std::memcpy(cur, &bytes.size, sizeof bytes.size);
                std::memcpy(cur + sizeof(bytes.size), bytes.raw, bytes.size);
                cur += KEY_SIZE;
                std::free(bytes.raw);
            }else{
                std::free(bytes.raw);
                throw std::runtime_error("key too long");
            }
        }
        int num = node->num;
        if(!node->is_leaf){
            num++;
        }

        for (int i = 0; i < num; ++i){
            std::memcpy(cur, &node->children[i], sizeof node->children[i]);
            cur += sizeof node->children[i];
        }

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

        std::memcpy(&res->num, cur, sizeof res->num);
        cur += sizeof res->num;
        std::memcpy(&res->is_leaf, cur, sizeof res->is_leaf);
        cur += sizeof res->is_leaf;
        for (int i = 0; i < res->num; ++i){            
            Bytes bytes;
            std::memcpy(&bytes.size, cur, sizeof bytes.size);
            bytes.raw = std::malloc(bytes.size);
            std::memcpy(bytes.raw, cur + sizeof(bytes.size), bytes.size);
            cur += KEY_SIZE;
            res->keys[i] = DefaultSerializer::deserialize(bytes);
        }

        int num = res->num;
        if(!res->is_leaf){
            num++;
        }

        for (int i = 0; i < num; ++i){
            std::memcpy(&res->children[i], cur, sizeof res->children[i]);
            cur += sizeof res->children[i];
        }

        std::free(raw);
        return res;
    }

}

#endif //LAINDB_NODE_STORE_HPP_
