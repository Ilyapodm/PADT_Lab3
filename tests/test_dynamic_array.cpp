#include <gtest/gtest.h>
#include "dynamic_array.hpp"

/*******************************************************************
 * Constructors
 *******************************************************************/

TEST(DynamicArray_Ctor, Default_ZeroSizeAndCapacity) {
    DynamicArray<int> arr;
    EXPECT_EQ(arr.get_size(),     0);
    EXPECT_EQ(arr.get_capacity(), 0);
}

TEST(DynamicArray_Ctor, SizeZero_OK) {
    DynamicArray<int> arr(0);
    EXPECT_EQ(arr.get_size(),     0);
    EXPECT_EQ(arr.get_capacity(), 0);
}

TEST(DynamicArray_Ctor, NegativeSize_Throws) {
    EXPECT_THROW(DynamicArray<int>(-1), std::invalid_argument);
}

TEST(DynamicArray_Ctor, SizeN_ZeroInitialized) {
    DynamicArray<int> arr(5);
    EXPECT_EQ(arr.get_size(), 5);
    for (int i = 0; i < 5; i++)
        EXPECT_EQ(arr.get(i), 0);  // new T[n]{} zero-inits
}

TEST(DynamicArray_Ctor, FromItems_Copies) {
    int data[] = {10, 20, 30};
    DynamicArray<int> arr(data, 3);
    EXPECT_EQ(arr.get_size(), 3);
    EXPECT_EQ(arr.get(0), 10);
    EXPECT_EQ(arr.get(2), 30);
}

TEST(DynamicArray_Ctor, FromItems_NullptrSizeZero_OK) {
    // nullptr + size=0 допустимо
    EXPECT_NO_THROW(DynamicArray<int>(nullptr, 0));
}

TEST(DynamicArray_Ctor, FromItems_NullptrSizePositive_Throws) {
    EXPECT_THROW(DynamicArray<int>(nullptr, 3), std::invalid_argument);
}

TEST(DynamicArray_Ctor, FromItems_NegativeSize_Throws) {
    int data[] = {1, 2};
    EXPECT_THROW(DynamicArray<int>(data, -1), std::invalid_argument);
}

TEST(DynamicArray_Ctor, Copy_IndependentMemory) {
    int data[] = {1, 2, 3};
    DynamicArray<int> a(data, 3);
    DynamicArray<int> b(a);

    b.set(0, 999);
    EXPECT_EQ(a.get(0), 1);   // оригинал не тронут
    EXPECT_EQ(b.get(0), 999);
}

/*******************************************************************
 * operator =
 *******************************************************************/

TEST(DynamicArray_Assign, Basic) {
    int data[] = {5, 6, 7};
    DynamicArray<int> a(data, 3);
    DynamicArray<int> b;
    b = a;
    EXPECT_EQ(b.get_size(), 3);
    EXPECT_EQ(b.get(1), 6);
}

TEST(DynamicArray_Assign, SelfAssign_NoCorruption) {
    int data[] = {1, 2, 3};
    DynamicArray<int> a(data, 3);
    a = a;  // не должно упасть или испортить данные
    EXPECT_EQ(a.get_size(), 3);
    EXPECT_EQ(a.get(2), 3);
}

TEST(DynamicArray_Assign, IndependentAfterAssign) {
    int data[] = {1, 2, 3};
    DynamicArray<int> a(data, 3);
    DynamicArray<int> b;
    b = a;
    b.set(0, 42);
    EXPECT_EQ(a.get(0), 1);
}

/*******************************************************************
 * get, set
 *******************************************************************/

TEST(DynamicArray_Access, Get_NegativeIndex_Throws) {
    DynamicArray<int> arr(3);
    EXPECT_THROW(arr.get(-1), std::out_of_range);
}

TEST(DynamicArray_Access, Get_IndexEqualSize_Throws) {
    DynamicArray<int> arr(3);
    EXPECT_THROW(arr.get(3), std::out_of_range);  // valid: 0..2
}

TEST(DynamicArray_Access, Get_LastValidIndex_OK) {
    int data[] = {10, 20, 30};
    DynamicArray<int> arr(data, 3);
    EXPECT_EQ(arr.get(2), 30);
}

TEST(DynamicArray_Access, Set_NegativeIndex_Throws) {
    DynamicArray<int> arr(3);
    EXPECT_THROW(arr.set(-1, 0), std::out_of_range);
}

TEST(DynamicArray_Access, Set_IndexEqualSize_Throws) {
    DynamicArray<int> arr(3);
    EXPECT_THROW(arr.set(3, 0), std::out_of_range);
}

TEST(DynamicArray_Access, Get_OnEmpty_Throws) {
    DynamicArray<int> arr;
    EXPECT_THROW(arr.get(0), std::out_of_range);
}

/*******************************************************************
 * resize
 *******************************************************************/

TEST(DynamicArray_Resize, NegativeSize_Throws) {
    DynamicArray<int> arr(3);
    EXPECT_THROW(arr.resize(-1), std::invalid_argument);
}

TEST(DynamicArray_Resize, ToZero_ClearsBuffer) {
    int data[] = {1, 2, 3};
    DynamicArray<int> arr(data, 3);
    arr.resize(0);
    EXPECT_EQ(arr.get_size(),     0);
    EXPECT_EQ(arr.get_capacity(), 0);
}

TEST(DynamicArray_Resize, WithinCapacity_NoRealloc) {
    // После resize(6) capacity = 6, resize(3) не меняет capacity
    DynamicArray<int> arr(6);
    int cap_before = arr.get_capacity();
    arr.resize(3);
    EXPECT_EQ(arr.get_size(), 3);
    EXPECT_EQ(arr.get_capacity(), cap_before);  // capacity не уменьшается
}

TEST(DynamicArray_Resize, BeyondCapacity_Doubles) {
    DynamicArray<int> arr(4);  // capacity=4
    arr.resize(5);             // new_size > capacity → capacity*2=8
    EXPECT_EQ(arr.get_size(),     5);
    EXPECT_EQ(arr.get_capacity(), 8);
}

TEST(DynamicArray_Resize, FarBeyondCapacity_UsesNewSize) {
    DynamicArray<int> arr(2);  // capacity=2
    arr.resize(100);           // 100 > 2*2=4 → capacity=100
    EXPECT_EQ(arr.get_capacity(), 100);
}

TEST(DynamicArray_Resize, PreservesData) {
    int data[] = {1, 2, 3};
    DynamicArray<int> arr(data, 3);
    arr.resize(5);
    EXPECT_EQ(arr.get(0), 1);
    EXPECT_EQ(arr.get(2), 3);
}

TEST(DynamicArray_Resize, IncreaseDecrease_DataIntact) {
    int data[] = {10, 20, 30, 40, 50};
    DynamicArray<int> arr(data, 5);
    arr.resize(10);
    arr.resize(3);
    EXPECT_EQ(arr.get(0), 10);
    EXPECT_EQ(arr.get(2), 30);
    EXPECT_THROW(arr.get(3), std::out_of_range);  // размер = 3 теперь
}
