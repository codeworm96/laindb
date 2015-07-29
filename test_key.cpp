#include <type_traits>
#include <iostream>

#include "key_type.h"

using namespace std;

int main()
{
    cout <<"size: " << sizeof(laindb::Key) << endl;
    cout <<"is_pod: " << is_pod<laindb::Key>::value << endl;
    return 0;
}
