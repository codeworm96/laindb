#ifndef LAINDB_UTILITY_H_
#define LAINDB_UTILITY_H_

#include <utility>

namespace laindb {

    //type alias for Address
    typedef int Address;

    /** 
     * enum: FileMode
     * used to represent options of opening a database file
     */

    enum FileMode {
        OPEN = 1, //open a file that exists
        NEW = 2,  //create a new file
        CREATE = 3 //try to open first. If it fails, create a new file
    };

    /**
     * Function template: shift
     * (left) shift a array of length `len`
     */

    template<typename T> void shift(T * arr, int len)
    {
        for (int i = 0; i < len - 1; ++i){
            arr[i] = std::move(arr[i + 1]);
        }
    }

    /**
     * Function template: unshift
     * (right) shift a array of length `len`
     */

    template<typename T> void unshift(T * arr, int len)
    {
        for (int i = len; i > 0; --i){
            arr[i] = std::move(arr[i - 1]);
        }
    }

    /**
     * Function template: item_move
     * move `num` items from source to dest
     */

    template<typename T> void item_move(T * dest, T * source, int num)
    {
        for (int i = 0; i < num; ++i){
            dest[i] = std::move(source[i]);
        }
    }
}
#endif //LAINDB_UTILITY_H_
