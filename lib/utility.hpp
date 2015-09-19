#ifndef LAINDB_UTILITY_HPP_
#define LAINDB_UTILITY_HPP_

#include <utility>
#include <iostream>
#include <cstring>

namespace laindb {

    //static assert
    template <bool x> struct STATIC_ASSERTION_FAILURE;
    template <> struct STATIC_ASSERTION_FAILURE<true> {};
    //STATIC_ASSERTION_FAILURE<false> is not defined delibrately

    template<int x> struct static_assert_test {};

#define STATIC_ASSERT(EXPR) typedef static_assert_test<sizeof(STATIC_ASSERTION_FAILURE<bool(EXPR)>)> static_assert_typedef_

    //block size of the disk
    const int BLOCK_SIZE = 4096;

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

    template<typename T> inline void shift(T * arr, int len)
    {
        for (int i = 0; i < len - 1; ++i){
            arr[i] = std::move(arr[i + 1]);
        }
    }

    /**
     * Function template: unshift
     * (right) shift a array of length `len`
     */

    template<typename T> inline void unshift(T * arr, int len)
    {
        for (int i = len; i > 0; --i){
            arr[i] = std::move(arr[i - 1]);
        }
    }

    /**
     * Function template: item_move
     * move `num` items from source to dest
     */

    //helper for non-POD
    template<typename T> inline void item_move_helper(T * dest, T * source, int num, std::false_type)
    {
        for (int i = 0; i < num; ++i){
            dest[i] = std::move(source[i]);
        }
    }

    //helper for POD
    template<typename T> inline void item_move_helper(T * dest, T * source, int num, std::true_type)
    {
        std::memcpy(dest, source, num * sizeof(T));
    }

    template<typename T> inline void item_move(T * dest, T * source, int num)
    {
        item_move_helper(dest, source, num, typename std::is_pod<T>::type());
    }

}
#endif //LAINDB_UTILITY_HPP_
