#ifndef LAINDB_DEFAULT_SERIALIZER_HPP_
#define LAINDB_DEFAULT_SERIALIZER_HPP_

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

            static Bytes serialize(T obj);

            /*
             * Function:deserialize
             * convert Bytes to type T
             */

            static T deserialize(Bytes raw);
    };

    Bytes DefaultSerializer::serialize(T obj)
    {
        return obj;
    }

    T DefaultSerializer::deserialize(Bytes raw)
    {
        return raw;
    }

}

#endif //LAINDB_DEFAULT_SERIALIZER_HPP_
