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
    inline bool operator<(Key k1, Key k2)
    {
        return (std::strcmp(k1.content, k2.content) < 0);
    }

    inline bool operator<=(Key k1, Key k2)
    {
        return (std::strcmp(k1.content, k2.content) <= 0);
    }

    inline bool operator>(Key k1, Key k2)
    {
        return (std::strcmp(k1.content, k2.content) > 0);
    }

    inline bool operator>=(Key k1, Key k2)
    {
        return (std::strcmp(k1.content, k2.content) >= 0);
    }

    inline bool operator==(Key k1, Key k2)
    {
        return (std::strcmp(k1.content, k2.content) == 0);
    }

    inline bool operator!=(Key k1, Key k2)
    {
        return (std::strcmp(k1.content, k2.content) != 0);
    }

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
