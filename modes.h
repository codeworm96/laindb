#ifndef LAINDB_MODES_H_
#define LAINDB_MODES_H_

namespace laindb {

    /* 
     * Type: FileMode
     * used to represent options of opening a database file
     */

    enum FileMode {
        OPEN = 1, //open a file that exists
        NEW = 2,  //create a new file
        CREATE = 3 //try to open first. If it fails, create a new file
    };

}
#endif //LAINDB_MODES_H_
