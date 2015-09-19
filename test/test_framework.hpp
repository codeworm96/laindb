#ifndef LAINDB_TEST_FRAMEWORK_HPP_
#define LAINDB_TEST_FRAMEWORK_HPP_

#include <iostream>
#include <stdexcept>

#define TESTCASE(NAME) class TEST_CASE_##NAME { \
    public: \
    TEST_CASE_##NAME() { \
        std::cout << "testing " << #NAME << "..." << std::endl; \
        test_body(); \
    } \
    private: \
    void test_body(); \
}; \
TEST_CASE_##NAME test_##NAME; \
void TEST_CASE_##NAME::test_body()

template <typename T> void assert_equal(const T & expect, const T & res)
{
    if (expect == res) {
        //std::cout << "[PASS]" << std::endl;
    }else{
        std::cout << "[FAIL]" << expect << " expected, but gets " << res << std::endl;
        throw std::runtime_error("assert fails");
    }
}

#endif //LAINDB_TEST_FRAMEWORK_HPP_
