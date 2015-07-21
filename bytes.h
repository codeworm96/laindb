#ifndef LAINDB_BYTES_H_
#define LAINDB_BYTES_H_

#include <cstdint>

namespace laindb {

    /*
     * struct: Bytes
     * This struct is used to represent a string of bytes.
     * The user of the database need to use it, only when customizing the serializer.
     * Otherwise, there may be memory leak.
     */

    struct Bytes {
        //the length of the byte string (16 bits integer)
        int16_t size;
        //pointer to the byte string
        void * raw;

        //constructors
        Byte() :size(0), raw(nullptr) {}
        Byte(int16_t sz, char * p) :size(sz), raw(p) {}
    };

}

#endif //LAINDB_BYTES_H_
