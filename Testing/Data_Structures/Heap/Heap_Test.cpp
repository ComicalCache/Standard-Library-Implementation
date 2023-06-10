#define BOOST_TEST_MODULE Heap_Test

#include <boost/test/included/unit_test.hpp>

#include <Heap/Heap.hpp>
#include <Vector/Vector.hpp>
#include <TestObj.hpp>

static const ext::vector<int> unorderedInt({4, 3, 7, 29, 12, 91, 44, 32, 74, 9, 37, 71, 93, 66, 77});
static const ext::vector<int> orderedInt({3, 4, 7, 9, 12, 29, 32, 37, 44, 66, 71, 74, 77, 91, 93});
static const ext::vector<testObj> unorderedObj(
		{testObj(4), testObj(3), testObj(7), testObj(29), testObj(12), testObj(91), testObj(44), testObj(32),
		 testObj(74), testObj(9), testObj(37), testObj(71), testObj(93), testObj(66), testObj(77)});

size_t vectLength = unorderedInt.size();

BOOST_AUTO_TEST_CASE(Heap_Default_Constructor) {
	ext::heap<int> heapInt;

	for (size_t i = 0; i < vectLength; ++i) {
		heapInt.push(unorderedInt.at(i));
	}

	for (size_t i = 0; i < vectLength; ++i) {
		BOOST_TEST(heapInt.peek() == orderedInt.at(i));
		heapInt.pop();
	}
}

BOOST_AUTO_TEST_CASE(Heap_Size_Constructor) {
	ext::heap<int> heapInt(EXT_HEAP_SIZE(5));

	BOOST_TEST(heapInt.size() == 0);
	BOOST_TEST(heapInt.capacity() == 5);

	for (size_t i = 0; i < vectLength; ++i) {
		heapInt.push(unorderedInt.at(i));
	}

	for (size_t i = 0; i < vectLength; ++i) {
		BOOST_TEST(heapInt.peek() == orderedInt.at(i));
		heapInt.pop();
	}
}

BOOST_AUTO_TEST_CASE(Heap_Func_Constructor) {
	ext::vector<int> reverseInt = orderedInt;
	std::reverse(reverseInt.begin(), reverseInt.end());

	ext::heap<int> heapInt(EXT_MAX_HEAP(int));
	ext::heap<testObj> heapTestObj([](const testObj &t1, const testObj &t2) { return t1.val > t2.val; });

	for (size_t i = 0; i < vectLength; ++i) {
		heapInt.push(unorderedInt.at(i));
		heapTestObj.push(unorderedObj.at(i));
	}

	for (size_t i = 0; i < vectLength; ++i) {
		BOOST_TEST(heapInt.peek() == reverseInt.at(i));
		BOOST_TEST(heapTestObj.peek().val == reverseInt.at(i));
		heapInt.pop();
		heapTestObj.pop();
	}
}

BOOST_AUTO_TEST_CASE(Heap_Func_And_Size_Constructor) {
	ext::vector<int> reverseInt = orderedInt;
	std::reverse(reverseInt.begin(), reverseInt.end());

	ext::heap<int> heapInt(5, EXT_MAX_HEAP(int));
	ext::heap<testObj> heapTestObj(5, [](const testObj &t1, const testObj &t2) { return t1.val > t2.val; });

	BOOST_TEST(heapInt.size() == 0);
	BOOST_TEST(heapInt.capacity() == 5);
	BOOST_TEST(heapTestObj.size() == 0);
	BOOST_TEST(heapTestObj.capacity() == 5);

	for (size_t i = 0; i < vectLength; ++i) {
		heapInt.push(unorderedInt.at(i));
		heapTestObj.push(unorderedObj.at(i));
	}

	for (size_t i = 0; i < vectLength; ++i) {
		BOOST_TEST(heapInt.peek() == reverseInt.at(i));
		BOOST_TEST(heapTestObj.peek().val == reverseInt.at(i));
		heapInt.pop();
		heapTestObj.pop();
	}
}

BOOST_AUTO_TEST_CASE(Heap_Initializer_List_Constructor) {
	ext::heap<int> heapInt({2, 5, 3, 4});

	BOOST_TEST(heapInt.peek() == 2);
	heapInt.pop();
	BOOST_TEST(heapInt.peek() == 3);
	heapInt.pop();
	BOOST_TEST(heapInt.peek() == 4);
	heapInt.pop();
	BOOST_TEST(heapInt.peek() == 5);
	heapInt.pop();
}

BOOST_AUTO_TEST_CASE(Heap_Initializer_List_And_Func_Constructor) {
	ext::heap<int> heapInt({2, 5, 3, 4}, EXT_MAX_HEAP(int));
	ext::heap<testObj> heapTestObj({testObj(2), testObj(5), testObj(3), testObj(4)},
	                               [](const testObj &t1, const testObj &t2) { return t1.val > t2.val; });

	BOOST_TEST(heapInt.peek() == 5);
	BOOST_TEST(heapTestObj.peek().val == 5);
	heapInt.pop();
	heapTestObj.pop();
	BOOST_TEST(heapInt.peek() == 4);
	BOOST_TEST(heapTestObj.peek().val == 4);
	heapInt.pop();
	heapTestObj.pop();
	BOOST_TEST(heapInt.peek() == 3);
	BOOST_TEST(heapTestObj.peek().val == 3);
	heapInt.pop();
	heapTestObj.pop();
	BOOST_TEST(heapInt.peek() == 2);
	BOOST_TEST(heapTestObj.peek().val == 2);
	heapInt.pop();
	heapTestObj.pop();
}

BOOST_AUTO_TEST_CASE(Heap_Destructor) {
	ext::heap<int> heapInt;
	ext::heap<int> heapIntSize(EXT_HEAP_SIZE(5));
	ext::heap<int> heapIntFunction(EXT_MAX_HEAP(int));
	ext::heap<testObj> heapTestObjFunction([](const testObj &t1, const testObj &t2) { return t1.val > t2.val; });
	ext::heap<int> heapIntSizeFunction(5, EXT_MAX_HEAP(int));
	ext::heap<testObj> heapTestObjSizeFunction(5, [](const testObj &t1, const testObj &t2) { return t1.val > t2.val; });
	ext::heap<int> heapIntInitializerList({2, 5, 3, 4});
	ext::heap<int> heapIntInitializerListFunction({2, 5, 3, 4}, EXT_MAX_HEAP(int));
	ext::heap<testObj> heapTestObjInitializerListFunction({testObj(2), testObj(5), testObj(3), testObj(4)},
	                                                      [](const testObj &t1, const testObj &t2) {
		                                                      return t1.val > t2.val;
	                                                      });

	BOOST_CHECK_NO_THROW(heapInt.~heap());
	BOOST_CHECK_NO_THROW(heapIntSize.~heap());
	BOOST_CHECK_NO_THROW(heapIntFunction.~heap());
	BOOST_CHECK_NO_THROW(heapTestObjFunction.~heap());
	BOOST_CHECK_NO_THROW(heapIntSizeFunction.~heap());
	BOOST_CHECK_NO_THROW(heapTestObjSizeFunction.~heap());
	BOOST_CHECK_NO_THROW(heapIntInitializerList.~heap());
	BOOST_CHECK_NO_THROW(heapIntInitializerListFunction.~heap());
	BOOST_CHECK_NO_THROW(heapTestObjInitializerListFunction.~heap());
}

BOOST_AUTO_TEST_CASE(Heap_Initializer_List_Assignment) {
	ext::heap<int> heapInt = {2, 5, 3, 4};

	BOOST_TEST(heapInt.peek() == 2);
	heapInt.pop();
	BOOST_TEST(heapInt.peek() == 3);
	heapInt.pop();
	BOOST_TEST(heapInt.peek() == 4);
	heapInt.pop();
	BOOST_TEST(heapInt.peek() == 5);
	heapInt.pop();
}

BOOST_AUTO_TEST_CASE(Heap_Peek_Method) {
	ext::heap<int> heapInt = {3, 2, 1};
	ext::heap<testObj> heapTestObj({testObj(2), testObj(5), testObj(3), testObj(4)},
	                               [](const testObj &t1, const testObj &t2) {
		                               return t1.val > t2.val;
	                               });

	BOOST_TEST(heapInt.peek() == 1);
	BOOST_TEST(heapInt.size() == 3);
	BOOST_TEST(heapTestObj.peek().val == 5);
	BOOST_TEST(heapTestObj.size() == 4);
}

BOOST_AUTO_TEST_CASE(Heap_Empty_Method) {
	ext::heap<int> heapInt = {1};

	BOOST_TEST(heapInt.empty() == false);
	heapInt.pop();
	BOOST_TEST(heapInt.empty() == true);
}

BOOST_AUTO_TEST_CASE(Heap_Size_Method) {
	ext::heap<int> heapInt = {1, 2, 3};

	BOOST_TEST(heapInt.size() == 3);
	heapInt.pop();
	BOOST_TEST(heapInt.size() == 2);
}

BOOST_AUTO_TEST_CASE(Heap_Capacity_Method) {
	ext::heap<int> heapIntEmpty;
	ext::heap<int> heapInt = {1};
	ext::heap<int> heapIntSize(EXT_HEAP_SIZE(5));

	BOOST_TEST(heapIntEmpty.capacity() == 2);
	BOOST_TEST(heapInt.capacity() == 2);
	BOOST_TEST(heapIntSize.capacity() == 5);
}

BOOST_AUTO_TEST_CASE(Heap_Max_Capacity_Method) {
	ext::heap<int> heapIntEmpty;
	ext::heap<int> heapInt = {1};

	BOOST_TEST(heapIntEmpty.max_capacity() == SIZE_MAX);
	BOOST_TEST(heapInt.max_capacity() == SIZE_MAX);
}

BOOST_AUTO_TEST_CASE(Heap_Clear_Method) {
	ext::heap<int> heapInt = {1, 2, 3};
	heapInt.clear();
	BOOST_TEST(heapInt.size() == 0);
	BOOST_TEST(heapInt.capacity() == 3);
}

BOOST_AUTO_TEST_CASE(Heap_Push_Copy_Method) {
	ext::heap<testObj> heapTestObj([](const testObj &t1, const testObj &t2) {
		return t1.val > t2.val;
	});
	testObj t1(2);
	heapTestObj.push(t1);
	BOOST_TEST(heapTestObj.peek().val == 2);

	testObj t2(6);
	testObj t3(4);
	heapTestObj.push(t2);
	heapTestObj.push(t3);
	BOOST_TEST(heapTestObj.peek().val == 6);
}

BOOST_AUTO_TEST_CASE(Heap_Push_Move_Method) {
	ext::heap<testObj> heapTestObj([](const testObj &t1, const testObj &t2) {
		return t1.val > t2.val;
	});
	heapTestObj.push(testObj(2));
	BOOST_TEST(heapTestObj.peek().val == 2);

	heapTestObj.push(testObj(6));
	heapTestObj.push(testObj(4));
	BOOST_TEST(heapTestObj.peek().val == 6);
}

BOOST_AUTO_TEST_CASE(Heap_Emplace_Method) {
	ext::heap<testObj> heapTestObj([](const testObj &t1, const testObj &t2) {
		return t1.val > t2.val;
	});
	heapTestObj.emplace(2);
	BOOST_TEST(heapTestObj.peek().val == 2);

	heapTestObj.emplace(6);
	heapTestObj.emplace(4);
	BOOST_TEST(heapTestObj.peek().val == 6);
}

BOOST_AUTO_TEST_CASE(Heap_Pop_Method) {
	ext::heap<int> heapInt = {1, 2, 3};
	ext::heap<testObj> heapTestObj({testObj(1), testObj(2), testObj(3)}, [](const testObj &t1, const testObj &t2) {
		return t1.val > t2.val;
	});

	BOOST_TEST(heapInt.peek() == 1);
	heapInt.pop();
	BOOST_TEST(heapInt.peek() == 2);
	BOOST_TEST(heapInt.size() = 2);

	BOOST_TEST(heapTestObj.peek().val == 3);
	heapTestObj.pop();
	BOOST_TEST(heapTestObj.peek().val == 2);
	BOOST_TEST(heapTestObj.size() = 2);
}
