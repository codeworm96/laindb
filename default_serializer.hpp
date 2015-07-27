#ifndef LAINDB_DEFAULT_SERIALIZER_HPP_
#define LAINDB_DEFAULT_SERIALIZER_HPP_

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "bytes.h"

namespace laindb {

    /*
     * Class: DefaultSerializer
     * contains methods that converts type T to Bytes
     * & that converts Bytes to type T
     */

    //NOTE: use these alias to replace template parameter for now
    typedef int T;//tmp;

    class DefaultSerializer {
        public:

            /*
             * Function: serialize
             * convert type T to Bytes
             */

            static Bytes serialize(const T & obj);

            /*
             * Function:deserialize
             * convert Bytes to type T
             */

            static T deserialize(Bytes & raw);
    };

    Bytes DefaultSerializer::serialize(const T & obj)
    {
        Bytes res;
        res.size = sizeof(obj);
        res.raw = std::malloc(res.size);
        std::memcpy(res.raw, &obj, res.size);
        return res;
    }

    T DefaultSerializer::deserialize(Bytes & raw)
    {
        T res(*(static_cast<T *>(raw.raw)));
        return res;
    }

}

#endif //LAINDB_DEFAULT_SERIALIZER_HPP_
