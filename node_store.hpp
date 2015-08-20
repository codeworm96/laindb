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

    //space for a node
    const int NODE_SIZE = BLOCK_SIZE;

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

            //pager
            Pager pager;

            //available blocks
            std::vector<int> blocks;

            //# of blocks in file
            int size;

            //id of root of the b+tree
            int rootID;
    };

    NodeStore::NodeStore(std::string file_name, FileMode mode) :name(file_name), pager(name, mode, 6151), size(0), rootID(0)
    {
        std::string idle_name = std::string("idle_") + name;
        if(pager.status() == OPEN){
            FILE * idle_file = std::fopen(idle_name.c_str(), "rb");
            if (idle_file){
                pager.read(&rootID, sizeof(rootID), 0);
                pager.read(&size, sizeof(size), sizeof(rootID));

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
        }
    }

    NodeStore::~NodeStore()
    {
        std::string idle_name = std::string("idle_") + name;
        pager.write(&rootID, sizeof(rootID), 0);
        pager.write(&size, sizeof(size), sizeof(rootID));

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
        
        pager.write(node, sizeof(*node), NODE_SIZE * node->id);

        delete node;
    }

    BNode * NodeStore::load(int id)
    {
        BNode * res = new BNode;
        pager.read(res, sizeof(*res), NODE_SIZE * id);
        
        res->id = id;
        res->modified = false;

        return res;
    }

}

#endif //LAINDB_NODE_STORE_HPP_
