#ifndef LAINDB_DEFAULT_SERIALIZER_HPP_
#define LAINDB_DEFAULT_SERIALIZER_HPP_

#include <cstdlib>
#include <cstring>
#include <type_traits>

#include "bytes.h"

namespace laindb {

    /**
     * Class: DefaultSerializer
     * contains methods that converts type T to Bytes
     * & that converts Bytes to type T
     */

    template<typename T> class DefaultSerializer {
        public:

            //T must be a POD to use this serializer
            STATIC_ASSERT(std::is_pod<T>::value);

            /**
             * Function: serialize
             * convert type T to Bytes
             */

            static Bytes serialize(const T & obj);

            /**
             * Function: deserialize
             * convert Bytes to type T
             * Side effect: after the call raw will be invalid
             */

            static T deserialize(Bytes & raw);
    };

    template<typename T> Bytes DefaultSerializer<T>::serialize(const T & obj)
    {
        Bytes res(sizeof(obj), std::malloc(sizeof(obj)));
        std::memcpy(res.raw, &obj, res.size);
        return res;
    }

    template<typename T> T DefaultSerializer<T>::deserialize(Bytes & raw)
    {
        T res(*(static_cast<T *>(raw.raw)));
        std::free(raw.raw); //free space
        return res;
    }

}

#endif //LAINDB_DEFAULT_SERIALIZER_HPP_
