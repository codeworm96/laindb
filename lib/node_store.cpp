#include "node_store.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <type_traits>

#include "bnode.h"
#include "pager.h"
#include "utility.hpp"

namespace laindb {

    NodeStore::NodeStore(std::string file_name, FileMode mode) :name(file_name), pager(name, mode, NODE_CACHE_SIZE), size(0), rootID(0)
    {
        std::string idle_name = std::string("idle_") + name;
        if(pager.status() == OPEN){
            FILE * idle_file = std::fopen(idle_name.c_str(), "rb");
            if (idle_file){
                //load rootID & size
                pager.read(&rootID, sizeof(rootID), 0);
                pager.read(&size, sizeof(size), sizeof(rootID));

                //load idle blocks' info
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
        
        //save rootID & size
        pager.write(&rootID, sizeof(rootID), 0);
        pager.write(&size, sizeof(size), sizeof(rootID));

        //save idle blocks' addresses
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

    void NodeStore::write(BNode * node)
    {
        if (node->modified){
            pager.write(node, sizeof(*node), NODE_SIZE * node->id);
        }
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