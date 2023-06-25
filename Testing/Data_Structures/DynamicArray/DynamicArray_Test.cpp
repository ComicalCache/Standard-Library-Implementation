#define BOOST_TEST_MODULE Array_Test

#include <boost/test/included/unit_test.hpp>
#include <string>

#include "DynamicArray/DynamicArray.hpp"
#include <TestObj.hpp>

BOOST_AUTO_TEST_CASE(DynamicArray_Runtime_Constructor) {
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

    auto test_0_dim_len1 = [&arr1]() {
        return arr1.length_of_dimension(0);
    };
    auto test_0_dim_len2 = [&arr1]() {
        return arr1.length_of_dimension(6);
    };
    BOOST_CHECK_THROW(test_0_dim_len1(), std::runtime_error);
    BOOST_CHECK_THROW(test_0_dim_len2(), std::runtime_error);

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

BOOST_AUTO_TEST_CASE(DynamicArray_Copy_Constructor) {
    ext::dynamic_array<int> arr1(4, 4);
    ext::dynamic_array<std::string> arr2(5, 5);

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

BOOST_AUTO_TEST_CASE(DynamicArray_Move_Constructor) {
    ext::dynamic_array<int> arr1(4, 4);
    arr1[0, 1, 2, 3] = 7;
    arr1[3, 2, 1, 0] = 5;
    int *arr1_data_pointer = arr1.data();
    size_t *arr1_ns_pointer = arr1.dimension_sizes();
    ext::dynamic_array<int> arr1_move(std::move(arr1));

    ext::dynamic_array<testObj> arr2(4, 4);
    arr2[0, 1, 2, 3] = testObj(7);
    arr2[3, 2, 1, 0] = testObj(5);
    testObj *arr2_data_pointer = arr2.data();
    size_t *arr2_ns_pointer = arr2.dimension_sizes();
    ext::dynamic_array<testObj> arr2_move(std::move(arr2));

    BOOST_TEST(arr1_move.at(0, 1, 2, 3) == 7);
    BOOST_TEST(arr1_move.at(3, 2, 1, 0) == 5);
    BOOST_TEST(arr1_move.data() == arr1_data_pointer);
    BOOST_TEST(arr1_move.dimension_sizes() == arr1_ns_pointer);
    BOOST_TEST(arr1.data() == nullptr);
    BOOST_TEST(arr1.dimension_sizes() == nullptr);

    BOOST_TEST(arr2_move.at(0, 1, 2, 3).val == testObj(7).val);
    BOOST_TEST(arr2_move.at(3, 2, 1, 0).val == testObj(5).val);
    BOOST_TEST(arr2_move.data() == arr2_data_pointer);
    BOOST_TEST(arr2_move.dimension_sizes() == arr2_ns_pointer);
    BOOST_TEST(arr2.data() == nullptr);
    BOOST_TEST(arr2.dimension_sizes() == nullptr);
}

BOOST_AUTO_TEST_CASE(DynamicArray_List_Constructor) {
    int list1[3 * 3 * 3];
    std::string list2[3 * 3 * 3];
    testObj list3[3 * 3 * 3];
    for (int i = 0; i < 3; i += 1) {
        for (int j = 0; j < 3; j += 1) {
            for (int k = 0; k < 3; k += 1) {
                int temp = (i + 1) * 100 + (j + 1) * 10 + (k + 1);
                list1[i * 3 * 3 + j * 3 + k] = temp;
                list2[i * 3 * 3 + j * 3 + k] = std::to_string(temp);
                list3[i * 3 * 3 + j * 3 + k] = testObj(temp);
            }
        }
    }

    ext::dynamic_array<int> arr1(list1, 3 * 3 * 3, 3, 3);
    ext::dynamic_array<std::string> arr2(list2, 3 * 3 * 3, 3, 3);
    ext::dynamic_array<testObj> arr3(list3, 3 * 3 * 3, 3, 3);

    BOOST_TEST(arr1.at(1, 2, 2) == 233);
    for (int i = 0; i < 3; i += 1) {
        for (int j = 0; j < 3; j += 1) {
            for (int k = 0; k < 3; k += 1) {
                int temp1 = list1[i * 3 * 3 + j * 3 + k];
                std::string temp2 = list2[i * 3 * 3 + j * 3 + k];
                testObj temp3 = list3[i * 3 * 3 + j * 3 + k];
                BOOST_TEST(arr1.at(i, j, k) == temp1);
                BOOST_TEST(arr2.at(i, j, k) == temp2);
                BOOST_TEST(arr3.at(i, j, k).val == temp3.val);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(DynamicArray_Iterator_Constructor) {
    std::array<int, 3 * 3 * 3> list1;
    std::array<std::string, 3 * 3 * 3> list2;
    std::array<testObj, 3 * 3 * 3> list3;
    for (int i = 0; i < 3; i += 1) {
        for (int j = 0; j < 3; j += 1) {
            for (int k = 0; k < 3; k += 1) {
                int temp = (i + 1) * 100 + (j + 1) * 10 + (k + 1);
                list1[i * 3 * 3 + j * 3 + k] = temp;
                list2[i * 3 * 3 + j * 3 + k] = std::to_string(temp);
                list3[i * 3 * 3 + j * 3 + k] = testObj(temp);
            }
        }
    }

    ext::dynamic_array<int> arr1(list1.begin(), list1.end(), 3, 3);
    ext::dynamic_array<std::string> arr2(list2.begin(), list2.end(), 3, 3);
    ext::dynamic_array<testObj> arr3(list3.begin(), list3.end(), 3, 3);

    for (int i = 0; i < 3; i += 1) {
        for (int j = 0; j < 3; j += 1) {
            for (int k = 0; k < 3; k += 1) {
                int temp1 = list1[i * 3 * 3 + j * 3 + k];
                std::string temp2 = list2[i * 3 * 3 + j * 3 + k];
                testObj temp3 = list3[i * 3 * 3 + j * 3 + k];
                BOOST_TEST(arr1.at(i, j, k) == temp1);
                BOOST_TEST(arr2.at(i, j, k) == temp2);
                BOOST_TEST(arr3.at(i, j, k).val == temp3.val);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(DynamicArray_Indexing) {
    ext::dynamic_array<int> arr1(3, 4);
    ext::dynamic_array<std::string> arr2(4, 5);
    ext::dynamic_array<testObj> arr3(5, 6);

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

BOOST_AUTO_TEST_CASE(DynamicArray_Clear) {
    ext::dynamic_array<int> arr1(3, 4);
    ext::dynamic_array<std::string> arr2(4, 5);
    ext::dynamic_array<testObj> arr3(5, 6);

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

BOOST_AUTO_TEST_CASE(DynamicArray_Indexed_Clear) {
    ext::dynamic_array<int> arr1(4, 5);
    ext::dynamic_array<std::string> arr2(4, 5);

    for (size_t i = 0; i < 5; i += 1) {
        for (size_t j = 0; j < 5; j += 1) {
            for (size_t l = 0; l < 5; l += 1) {
                arr1[2ul, i, j, l] = i * 100 + j * 10 + l;
            }
        }
    }

    arr1.clear(2, 2, 2, 2);
    for (size_t i = 0; i < 5; i += 1) {
        for (size_t j = 0; j < 5; j += 1) {
            for (size_t l = 0; l < 5; l += 1) {
                if (i != 2 || j != 2 || l != 2) {
                    BOOST_TEST(arr1.at(2ul, i, j, l) == i * 100 + j * 10 + l);
                } else {
                    BOOST_TEST(arr1.at(2, 2, 2, 2) == int());
                }
            }
        }
    }

    arr1.clear(2, 2, 2);
    for (size_t i = 0; i < 5; i += 1) {
        for (size_t j = 0; j < 5; j += 1) {
            for (size_t l = 0; l < 5; l += 1) {
                if (i != 2 || j != 2) {
                    BOOST_TEST(arr1.at(2ul, i, j, l) == i * 100 + j * 10 + l);
                } else {
                    BOOST_TEST(arr1.at(2, 2, 2, 2) == int());
                }
            }
        }
    }

    arr1.clear(2, 2);
    for (size_t i = 0; i < 5; i += 1) {
        for (size_t j = 0; j < 5; j += 1) {
            for (size_t l = 0; l < 5; l += 1) {
                if (i != 2) {
                    BOOST_TEST(arr1.at(2ul, i, j, l) == i * 100 + j * 10 + l);
                } else {
                    BOOST_TEST(arr1.at(2, 2, 2, 2) == int());
                }
            }
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

BOOST_AUTO_TEST_CASE(DynamicArray_Swap_Method) {
    ext::dynamic_array<int> arr1(4, 4);
    arr1[0, 1, 2, 3] = 7;
    arr1[3, 2, 1, 0] = 5;
    int *arr1_data_pointer = arr1.data();
    size_t *arr1_ns_pointer = arr1.dimension_sizes();
    ext::dynamic_array<int> arr2(3, 3);
    arr2[0, 1, 2] = 7;
    arr2[2, 1, 0] = 5;
    int *arr2_data_pointer = arr2.data();
    size_t *arr2_ns_pointer = arr2.dimension_sizes();

    ext::dynamic_array<testObj> arr3(4, 4);
    arr3[0, 1, 2, 3] = testObj(7);
    arr3[3, 2, 1, 0] = testObj(5);
    testObj* arr3_data_pointer = arr3.data();
    size_t *arr3_ns_pointer = arr3.dimension_sizes();
    ext::dynamic_array<testObj> arr4(3, 3);
    arr4[0, 1, 2] = testObj(7);
    arr4[2, 1, 0] = testObj(5);
    testObj* arr4_data_pointer = arr4.data();
    size_t *arr4_ns_pointer = arr4.dimension_sizes();

    arr1.swap(arr2);
    arr3.swap(arr4);

    BOOST_TEST(arr1.at(0, 1, 2) == 7);
    BOOST_TEST(arr1.at(2, 1, 0) == 5);
    BOOST_TEST(arr2.at(0, 1, 2, 3) == 7);
    BOOST_TEST(arr2.at(3, 2, 1, 0) == 5);
    BOOST_TEST(arr1.data() == arr2_data_pointer);
    BOOST_TEST(arr1.dimension_sizes() == arr2_ns_pointer);
    BOOST_TEST(arr1.dimensions() == 3);
    BOOST_TEST(arr1.size() == 3 * 3 * 3);
    BOOST_TEST(arr2.data() == arr1_data_pointer);
    BOOST_TEST(arr2.dimension_sizes() == arr1_ns_pointer);
    BOOST_TEST(arr2.dimensions() == 4);
    BOOST_TEST(arr2.size() == 4 * 4 * 4 * 4);

    BOOST_TEST(arr3.at(0, 1, 2).val == testObj(7).val);
    BOOST_TEST(arr3.at(2, 1, 0).val == testObj(5).val);
    BOOST_TEST(arr4.at(0, 1, 2, 3).val == testObj(7).val);
    BOOST_TEST(arr4.at(3, 2, 1, 0).val == testObj(5).val);
    BOOST_TEST(arr3.data() == arr4_data_pointer);
    BOOST_TEST(arr3.dimension_sizes() == arr4_ns_pointer);
    BOOST_TEST(arr3.dimensions() == 3);
    BOOST_TEST(arr3.size() == 3 * 3 * 3);
    BOOST_TEST(arr4.data() == arr3_data_pointer);
    BOOST_TEST(arr4.dimension_sizes() == arr3_ns_pointer);
    BOOST_TEST(arr4.dimensions() == 4);
    BOOST_TEST(arr4.size() == 4 * 4 * 4 * 4);
}
