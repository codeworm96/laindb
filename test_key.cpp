#include <type_traits>
#include <iostream>

#include "key_type.h"
#include "bnode.h"

using namespace std;

int main()
{
    cout <<"size: " << sizeof(laindb::BNode) << endl;
    cout <<"is_pod: " << is_pod<laindb::BNode>::value << endl;
    return 0;
}
