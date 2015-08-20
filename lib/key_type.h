#ifndef LAINDB_KEY_TYPE_H_
#define LAINDB_KEY_TYPE_H_

#include <cstring>
#include <stdexcept>

namespace laindb {

    //max length of the key
    const int MAX_KEY_LENGTH = 16;

    /**
     * struct: Key
     * the type of keys in the database
     * must be a POD
     */

    struct Key {

        char content[MAX_KEY_LENGTH];

    };

    //comparers
    inline int key_cmp(const Key & k1, const Key & k2)
    {
        return std::strcmp(k1.content, k2.content);
    }

    inline bool operator<(const Key & k1, const Key & k2)
    {
        return (std::strcmp(k1.content, k2.content) < 0);
    }

    inline bool operator<=(const Key & k1, const Key & k2)
    {
        return (std::strcmp(k1.content, k2.content) <= 0);
    }

    inline bool operator>(const Key & k1, const Key & k2)
    {
        return (std::strcmp(k1.content, k2.content) > 0);
    }

    inline bool operator>=(const Key & k1, const Key & k2)
    {
        return (std::strcmp(k1.content, k2.content) >= 0);
    }

    inline bool operator==(const Key & k1, const Key & k2)
    {
        return (std::strcmp(k1.content, k2.content) == 0);
    }

    inline bool operator!=(const Key & k1, const Key & k2)
    {
        return std::strcmp(k1.content, k2.content);
    }

    /**
     * function: make_key
     * convert a c style string to Key
     * throws when fails.
     */

    inline Key make_key(const char * str)
    {
        if(std::strlen(str) >= MAX_KEY_LENGTH){
            throw std::runtime_error("key too long");
        }else{
            Key res;
            std::strcpy(res.content, str);
            return res;
        }
    }
}

#endif //LAINDB_KEY_TYPE_H_
