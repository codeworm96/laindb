#ifndef LAINDB_BNODE_H_
#define LAINDB_BNODE_H_

#include "key_type.h"
#include "utility.h"

namespace laindb {

    //the minimum # of keys in a node (except root)
    const int MIN_KEYS = 64;
    //the maximum # of keys in a node
    const int MAX_KEYS = MIN_KEYS * 2 + 1;
    
    /**
     * Struct: BNode
     * A node of the b+tree
     */

    struct BNode {
        int id;  //the id of node to distinguish a node in the file
        int num; //# of keys
        bool is_leaf;
        bool modified; //used to decide whether to write to file

        //the keys
        Key keys[MAX_KEYS];

        //Note: in a inner node it represents subtrees
        //in a leaf, children[i] is the value that is associated with keys[i]
        Address children[MAX_KEYS + 1];
    };

}

#endif //LAINDB_BNODE_H_
