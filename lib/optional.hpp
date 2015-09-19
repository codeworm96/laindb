#ifndef LAINDB_OPTIONAL_HPP_
#define LAINDB_OPTIONAL_HPP_

#include <stdexcept>

namespace laindb {

    /*
     * class template: Optional
     * a value of type Optional<T>, may contain a value of type T and may not
     */

    template<typename T> class Optional {
        public:
            /**
             * constructors
             */

            //invalid
            Optional():valid(false) {}

            //valid value
            explicit Optional(const T & value):valid(true), content(value) {}

            /**
             * method: is_valid
             * test if a optional is valid
             */

            bool is_valid() { return valid; }

            /**
             * method: just
             * get the value (checked)
             */

            const T & just()
            {
                if (valid) {
                    return content;
                }else{
                    throw std::runtime_error("using invalid Optional value");
                }
            }
            
        private:
            bool valid;
            T content;
    };
}

#endif //LAINDB_OPTIONAL_HPP_
