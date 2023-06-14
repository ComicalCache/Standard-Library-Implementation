#define BOOST_TEST_MODULE Array_Test

#include <boost/test/included/unit_test.hpp>
#include <string>

#include "Array/Array.hpp"
#include <TestObj.hpp>

BOOST_AUTO_TEST_CASE(DEFAULT_CONSTRUCTOR) {
    ext::array<int, 5, 4> arr1;
    ext::array<std::string, 5, 5, 4, 3> arr2;
    ext::array<testObj, 5, 5, 4, 3, 2, 1> arr3;

    BOOST_TEST(arr1.dimensions() == 5);
    BOOST_TEST(arr2.dimensions() == 5);
    BOOST_TEST(arr3.dimensions() == 5);

    BOOST_TEST(arr1.size() == 4 * 4 * 4 * 4 * 4);
    BOOST_TEST(arr2.size() == 5 * 4 * 3 * 3 * 3);
    BOOST_TEST(arr3.size() == 5 * 4 * 3 * 2 * 1);

    size_t arr2_dims[] = {5, 4, 3, 3, 3};
    size_t arr3_dims[] = {5, 4, 3, 2, 1};
    for (size_t i = 1; i <= 5; i += 1) {
        BOOST_TEST(arr1.length_of_dimension(i) == 4);
        BOOST_TEST(arr2.length_of_dimension(i) == arr2_dims[i - 1]);
        BOOST_TEST(arr3.length_of_dimension(i) == arr3_dims[i - 1]);
    }

    auto test_0_dim_size = []() {
            return ext::array<int, 5, 0>();
    };
    BOOST_CHECK_THROW(test_0_dim_size(), std::runtime_error);

    auto test_too_many_dim_sizes = []() {
        return ext::array<int, 3, 5, 5, 5, 5>();
    };
    BOOST_CHECK_THROW(test_too_many_dim_sizes(), std::runtime_error);

    auto test_0_dims = []() {
        return ext::array<int, 0, 5>();
    };
    BOOST_CHECK_THROW(test_0_dims(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(RUNTIME_CONSTRUCTOR) {
    ext::array<int> arr1(5, 4);
    ext::array<std::string> arr2(5, 5, 4, 3);
    ext::array<testObj> arr3(5, 5, 4, 3, 2, 1);

    BOOST_TEST(arr1.dimensions() == 5);
    BOOST_TEST(arr2.dimensions() == 5);
    BOOST_TEST(arr3.dimensions() == 5);

    BOOST_TEST(arr1.size() == 4 * 4 * 4 * 4 * 4);
    BOOST_TEST(arr2.size() == 5 * 4 * 3 * 3 * 3);
    BOOST_TEST(arr3.size() == 5 * 4 * 3 * 2 * 1);

    size_t arr2_dims[] = {5, 4, 3, 3, 3};
    size_t arr3_dims[] = {5, 4, 3, 2, 1};
    for (size_t i = 1; i <= 5; i += 1) {
        BOOST_TEST(arr1.length_of_dimension(i) == 4);
        BOOST_TEST(arr2.length_of_dimension(i) == arr2_dims[i - 1]);
        BOOST_TEST(arr3.length_of_dimension(i) == arr3_dims[i - 1]);
    }

    auto test_0_dim_size = []() {
        return ext::array<int>(5, 0);
    };
    BOOST_CHECK_THROW(test_0_dim_size(), std::runtime_error);

    auto test_too_many_dim_sizes = []() {
        return ext::array<int>(3, 5, 5, 5, 5);
    };
    BOOST_CHECK_THROW(test_too_many_dim_sizes(), std::runtime_error);

    auto test_0_dims = []() {
        return ext::array<int>(0, 5);
    };
    BOOST_CHECK_THROW(test_0_dims(), std::runtime_error);
}
