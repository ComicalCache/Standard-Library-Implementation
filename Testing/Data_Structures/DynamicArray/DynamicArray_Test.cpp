#define BOOST_TEST_MODULE Array_Test

#include <boost/test/included/unit_test.hpp>
#include <string>

#include "DynamicArray/DynamicArray.hpp"
#include <TestObj.hpp>

BOOST_AUTO_TEST_CASE(Array_Default_Constructor) {
    ext::dynamic_array<int, 5, 4> arr1;
    ext::dynamic_array<std::string, 5, 5, 4, 3> arr2;
    ext::dynamic_array<testObj, 5, 5, 4, 3, 2, 1> arr3;

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
        return ext::dynamic_array<int, 5, 0>();
    };
    BOOST_CHECK_THROW(test_0_dim_size(), std::runtime_error);

    auto test_too_many_dim_sizes = []() {
        return ext::dynamic_array<int, 3, 5, 5, 5, 5>();
    };
    BOOST_CHECK_THROW(test_too_many_dim_sizes(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(Array_Runtime_Constructor) {
    ext::dynamic_array<int> arr1(5, 4);
    ext::dynamic_array<std::string> arr2(5, 5, 4, 3);
    ext::dynamic_array<testObj> arr3(5, 5, 4, 3, 2, 1);

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
        return ext::dynamic_array<int>(5, 0);
    };
    BOOST_CHECK_THROW(test_0_dim_size(), std::runtime_error);

    auto test_too_many_dim_sizes = []() {
        return ext::dynamic_array<int>(3, 5, 5, 5, 5);
    };
    BOOST_CHECK_THROW(test_too_many_dim_sizes(), std::runtime_error);

    auto test_0_dims = []() {
        return ext::dynamic_array<int>(0, 5);
    };
    BOOST_CHECK_THROW(test_0_dims(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(Array_Copy_Constructor) {
    ext::dynamic_array<int, 4, 4> arr1;
    ext::dynamic_array<std::string, 5, 5> arr2;

    arr1[0, 1, 2, 3] = 5;
    arr1[3, 2, 1, 0] = 1;
    arr2[0, 1, 2, 3, 4] = "5";
    arr2[4, 3, 2, 1, 0] = "1";

    ext::dynamic_array<int> arr1_copy(arr1);
    ext::dynamic_array<std::string> arr2_copy(arr2);

    arr1_copy[1, 0, 0, 0] = 7;
    arr2_copy[1, 0, 0, 0] = "7";

    BOOST_TEST(arr1_copy.at(0, 1, 2, 3) == 5);
    BOOST_TEST(arr1_copy.at(3, 2, 1, 0) == 1);
    BOOST_TEST(arr1_copy.at(0, 1, 2, 3) == 5);
    BOOST_TEST(arr1_copy.at(3, 2, 1, 0) == 1);

    BOOST_TEST(arr1.at(1, 0, 0, 0) != 7);
    BOOST_TEST(arr2.at(1, 0, 0, 0) != "7");
}

BOOST_AUTO_TEST_CASE(Array_Indexing) {
    ext::dynamic_array<int, 3, 4> arr1;
    ext::dynamic_array<std::string, 4, 5> arr2;
    ext::dynamic_array<testObj, 5, 6> arr3;

    arr1[1, 2, 3] = 7;
    arr2[1, 2, 3, 4] = "7";
    arr3[1, 2, 3, 4, 5] = testObj(7);

    arr1[1, 2] = 8;
    arr2[1, 2] = "8";
    arr3[1, 2] = testObj(8);

    bool arr1_test1 = 7 == *(arr1.data() + 1 * 4 * 4 + 2 * 4 + 3);
    BOOST_TEST(arr1_test1);

    bool arr2_test1 = "7" == *(arr2.data() + 1 * 5 * 5 * 5 + 2 * 5 * 5 + 3 * 5 + 4);
    BOOST_TEST(arr2_test1);

    bool arr3_test1 = testObj(7).val ==
                      (arr3.data() + 1 * 6 * 6 * 6 * 6 + 2 * 6 * 6 * 6 + 3 * 6 * 6 + 4 * 6 + 5)->val;
    BOOST_TEST(arr3_test1);

    BOOST_TEST(7 == arr1.at(1, 2, 3));
    BOOST_TEST("7" == arr2.at(1, 2, 3, 4));
    BOOST_TEST(testObj(7).val == arr3.at(1, 2, 3, 4, 5).val);

    bool arr1_test3 = 8 == *(arr1.data() + 1 * 4 * 4 + 2 * 4);
    BOOST_TEST(arr1_test3);

    bool arr2_test3 = "8" == *(arr2.data() + 1 * 5 * 5 * 5 + 2 * 5 * 5);
    BOOST_TEST(arr2_test3);

    bool arr3_test3 = testObj(8).val == (arr3.data() + 1 * 6 * 6 * 6 * 6 + 2 * 6 * 6 * 6)->val;
    BOOST_TEST(arr3_test3);
    BOOST_TEST(8 == arr1.at(1, 2));
    BOOST_TEST("8" == arr2.at(1, 2));
    BOOST_TEST(testObj(8).val == arr3.at(1, 2).val);


    BOOST_CHECK_THROW(arr1.at(0, 0, 5), std::runtime_error);
    BOOST_CHECK_THROW(arr2.at(1, 0, 6, 3), std::runtime_error);
    BOOST_CHECK_THROW(arr3.at(7, 1, 1, 1, 1), std::runtime_error);


    BOOST_CHECK_THROW(arr1.at(1, 2, 3, 4), std::runtime_error);
    BOOST_CHECK_THROW(arr2.at(1, 2, 3, 4, 4, 5, 7, 2), std::runtime_error);
    BOOST_CHECK_THROW(arr2.at(1, 2, 3, 4, 5, 6, 3), std::runtime_error);
    BOOST_CHECK_THROW(arr2.at(1, 2, 3, 4, 2), std::runtime_error);
    BOOST_CHECK_THROW(arr3.at(1, 2, 3, 4, 6, 7), std::runtime_error);
    BOOST_CHECK_THROW(arr3.at(1, 2, 3, 4, 7, 3, 2, 3, 5, 8), std::runtime_error);


}

BOOST_AUTO_TEST_CASE(Array_Clear) {
    ext::dynamic_array<int, 3, 4> arr1;
    ext::dynamic_array<std::string, 4, 5> arr2;
    ext::dynamic_array<testObj, 5, 6> arr3;

    arr1[1, 2, 3] = 7;
    arr2[1, 2, 3, 4] = "7";
    arr3[1, 2, 3, 4, 5] = testObj(7);

    arr1.clear();
    arr2.clear();
    arr3.clear();

    BOOST_TEST(arr1.at(1, 2, 3) == int());
    BOOST_TEST(arr2.at(1, 2, 3, 4).empty());
    BOOST_TEST(arr3.at(1, 2, 3, 4, 5).val == testObj().val);
}

BOOST_AUTO_TEST_CASE(Array_Indexed_Clear) {
    ext::dynamic_array<int, 4, 5> arr1;
    ext::dynamic_array<std::string, 4, 5> arr2;

    int counter = 1;
    for (size_t i = 0; i < 5; i += 1) {
        for (size_t j = 0; j < 5; j += 1) {
            for (size_t l = 0; l < 5; l += 1) {
                arr1[2ul, i, j, l] = counter;
                counter += 1;
            }
        }
    }

    arr1.clear(2, 2, 2, 2);
    BOOST_TEST(arr1.at(2, 2, 2, 2) == int());

    arr1.clear(2, 2, 2);
    for (size_t i = 0; i < 5; i += 1) {
        BOOST_TEST(arr1.at(2ul, 2ul, 2ul, i) == int());
    }
    arr1.clear(2, 2);
    for (size_t i = 0; i < 5; i += 1) {
        for (size_t j = 0; j < 5; j += 1) {
            BOOST_TEST(arr1.at(2ul, 2ul, i, j) == int());
        }
    }

    arr1.clear(2);
    for (size_t i = 0; i < 5; i += 1) {
        for (size_t j = 0; j < 5; j += 1) {
            for (size_t l = 0; l < 5; l += 1) {
                BOOST_TEST(arr1.at(2ul, i, j, l) == int());
            }
        }
    }

    arr2.clear(2, 2, 2, 2);
    std::string arr2_test_val = arr2[2, 2, 2, 2];
    BOOST_TEST(arr2_test_val.empty());

    arr2.clear(2, 2, 2);
    for (size_t i = 0; i < 5; i += 1) {
        BOOST_TEST(arr2.at(2ul, 2ul, 2ul, i).empty());
    }
    arr2.clear(2, 2);
    for (size_t i = 0; i < 5; i += 1) {
        for (size_t j = 0; j < 5; j += 1) {
            BOOST_TEST(arr2.at(2ul, 2ul, i, j).empty());
        }
    }

    arr2.clear(2);
    for (size_t i = 0; i < 5; i += 1) {
        for (size_t j = 0; j < 5; j += 1) {
            for (size_t l = 0; l < 5; l += 1) {
                BOOST_TEST(arr2.at(2ul, i, j, l).empty());
            }
        }
    }
}
