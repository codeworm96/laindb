#ifndef LAINDB_C_H_
#define LAINDB_C_H_

#ifdef __cplusplus
extern "C" {
#endif

struct Slice;
struct LainDB;

enum LaindbMode {
    OPEN = 1, //open a file that exists
    NEW = 2,  //create a new file
    CREATE = 3 //try to open first. If it fails, create a new file
};

/**
  * Constructor
  * Should open a database which name is `name`
  * using mode `mode`
  */
struct LainDB* laindb_new(char * name, int mode);

/**
  * Method: get
  * Fetch the value from the data file according to the key
  * returns a Slice* to represent a possible fail operation
  */

struct Slice* laindb_get(struct LainDB *db, char *key);

/*
 * Method: put
 * Put a pair of key/value into the database
 * if the key exists, updates the value.
 */
void laindb_put(struct LainDB *db, char *key, char *value, size_t len);

/*
 * Method: erase
 * Erase a key/value pair from the database accoring to the key
 * if the key does not exist, do nothing.
 */
void laindb_erase(struct LainDB *db, char *key);

/*
 * Destructor
 * Should close the database safely.
 */
void laindb_drop(struct LainDB *db);

/* getters */
size_t laindb_slice_len(struct Slice *slice);
char *laindb_slice_raw(struct Slice *slice);

/* destructor */
void laindb_slice_drop(struct Slice *slice);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LAINDB_C_H_ */