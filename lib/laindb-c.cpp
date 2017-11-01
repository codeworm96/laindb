#include "database.hpp"
#include "optional.hpp"
#include "laindb-c.h"
#include <string>

extern "C" struct Slice
{
    char *raw;
    size_t len;
};

class SliceSerializer
{
  public:
    /**
         * Function: serialize
         * convert Slice to Bytes
         */

    static laindb::Bytes serialize(const Slice &s)
    {
        laindb::Bytes res(s.len, std::malloc(s.len));
        std::memcpy(res.raw, s.raw, res.size);
        return res;
    }

    /**
         * Function: deserialize
         * convert Bytes to Slice
         * Side effect: after the call raw will be invalid
         */

    static Slice deserialize(laindb::Bytes &raw)
    {
        Slice res;
        res.len = raw.size;
        res.raw = static_cast<char *>(raw.raw);
        raw.size = 0;
        raw.raw = nullptr;
        return res;
    }
};

extern "C" struct LainDB
{
    laindb::Database<Slice, SliceSerializer> *db;
};

extern "C" {

LainDB *laindb_new(char *name, int mode)
{
    LainDB *db = (LainDB *)std::malloc(sizeof(LainDB));
    db->db = new laindb::Database<Slice, SliceSerializer>(std::string(name), (laindb::FileMode)mode);
    return db;
}

Slice *laindb_get(LainDB *db, char *key)
{
    laindb::Optional<Slice> res = db->db->get(key);
    if (res.is_valid())
    {
        Slice *slice = (Slice *)std::malloc(sizeof(Slice));
        *slice = res.just();
        return slice;
    }
    else
    {
        return NULL;
    }
}

void laindb_put(LainDB *db, char *key, char *value, size_t len)
{
    Slice slice;
    slice.len = len;
    slice.raw = value;
    db->db->put(key, slice);
}

void laindb_erase(LainDB *db, char *key)
{
    db->db->erase(key);
}

void laindb_drop(LainDB *db)
{
    if (db)
    {
        if (db->db)
        {
            delete db->db;
        }
        std::free(db);
    }
}

size_t laindb_slice_len(Slice *slice)
{
    if (slice)
    {
        return slice->len;
    }
    else
    {
        return 0;
    }
}

char *laindb_slice_raw(Slice *slice)
{
    if (slice)
    {
        return slice->raw;
    }
    else
    {
        return NULL;
    }
}

void laindb_slice_drop(Slice *slice)
{
    if (slice)
    {
        std::free(slice->raw);
        std::free(slice);
    }
}

} // extern "C"