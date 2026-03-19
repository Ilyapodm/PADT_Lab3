#include <gtest/gtest.h>
#include "linked_list.hpp"
/*******************************************************************
 * Constructors
 *******************************************************************/

TEST(LinkedList_Ctor, Default_EmptyList) {
    LinkedList<int> list;
    EXPECT_EQ(list.get_length(), 0);
}

TEST(LinkedList_Ctor, NegativeSize_Throws) {
    int data[] = {1, 2};
    EXPECT_THROW(LinkedList<int>(data, -1), std::invalid_argument);
}

TEST(LinkedList_Ctor, NullptrSizeZero_OK) {
    EXPECT_NO_THROW(LinkedList<int>(nullptr, 0));
}

TEST(LinkedList_Ctor, NullptrSizePositive_Throws) {
    EXPECT_THROW(LinkedList<int>(nullptr, 3), std::invalid_argument);
}

TEST(LinkedList_Ctor, FromItems_SizeAndValues) {
    int data[] = {10, 20, 30};
    LinkedList<int> list(data, 3);
    EXPECT_EQ(list.get_length(), 3);
    EXPECT_EQ(list.get(0), 10);
    EXPECT_EQ(list.get(2), 30);
}

TEST(LinkedList_Ctor, Copy_IndependentMemory) {
    int data[] = {1, 2, 3};
    LinkedList<int> a(data, 3);
    LinkedList<int> b(a);
    b.set(0, 999);
    EXPECT_EQ(a.get(0), 1);
    EXPECT_EQ(b.get(0), 999);
}

TEST(LinkedList_Ctor, SingleElement) {
    int data[] = {42};
    LinkedList<int> list(data, 1);
    EXPECT_EQ(list.get_first(), 42);
    EXPECT_EQ(list.get_last(),  42);  // head == tail
}

/*******************************************************************
 * operator =
 *******************************************************************/

TEST(LinkedList_Assign, Basic) {
    int data[] = {1, 2, 3};
    LinkedList<int> a(data, 3);
    LinkedList<int> b;
    b = a;
    EXPECT_EQ(b.get_length(), 3);
    EXPECT_EQ(b.get(1), 2);
}

TEST(LinkedList_Assign, SelfAssign_NoCorruption) {
    int data[] = {7, 8, 9};
    LinkedList<int> a(data, 3);
    a = a;
    EXPECT_EQ(a.get_length(), 3);
    EXPECT_EQ(a.get(0), 7);
}

TEST(LinkedList_Assign, IndependentAfterAssign) {
    int data[] = {1, 2, 3};
    LinkedList<int> a(data, 3);
    LinkedList<int> b;
    b = a;
    b.set(0, 42);
    EXPECT_EQ(a.get(0), 1);
}

/*******************************************************************
 * get_first, get_last
 *******************************************************************/

TEST(LinkedList_Access, GetFirst_OnEmpty_Throws) {
    LinkedList<int> list;
    EXPECT_THROW(list.get_first(), std::out_of_range);
}

TEST(LinkedList_Access, GetLast_OnEmpty_Throws) {
    LinkedList<int> list;
    EXPECT_THROW(list.get_last(), std::out_of_range);
}

TEST(LinkedList_Access, Get_NegativeIndex_Throws) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    EXPECT_THROW(list.get(-1), std::out_of_range);
}

TEST(LinkedList_Access, Get_IndexEqualLength_Throws) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    EXPECT_THROW(list.get(3), std::out_of_range);
}

TEST(LinkedList_Access, Set_OutOfRange_Throws) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    EXPECT_THROW(list.set(3, 99),  std::out_of_range);
    EXPECT_THROW(list.set(-1, 99), std::out_of_range);
}

/*******************************************************************
 * append, prepend
 *******************************************************************/

TEST(LinkedList_Mutate, Append_OnEmpty_HeadEqualsTail) {
    LinkedList<int> list;
    list.append(5);
    EXPECT_EQ(list.get_length(), 1);
    EXPECT_EQ(list.get_first(), 5);
    EXPECT_EQ(list.get_last(),  5);
}

TEST(LinkedList_Mutate, Append_Multiple_OrderPreserved) {
    LinkedList<int> list;
    for (int i = 0; i < 5; i++) list.append(i * 10);
    EXPECT_EQ(list.get_first(), 0);
    EXPECT_EQ(list.get_last(),  40);
    EXPECT_EQ(list.get(2),      20);
}

TEST(LinkedList_Mutate, Prepend_OnEmpty_HeadEqualsTail) {
    LinkedList<int> list;
    list.prepend(7);
    EXPECT_EQ(list.get_first(), 7);
    EXPECT_EQ(list.get_last(),  7);
}

TEST(LinkedList_Mutate, Prepend_Multiple_OrderReversed) {
    LinkedList<int> list;
    list.prepend(3);
    list.prepend(2);
    list.prepend(1);
    EXPECT_EQ(list.get(0), 1);
    EXPECT_EQ(list.get(1), 2);
    EXPECT_EQ(list.get(2), 3);
}

/*******************************************************************
 * insert_at
 *******************************************************************/

TEST(LinkedList_InsertAt, AtZero_EqualsPrepend) {
    int data[] = {2, 3, 4};
    LinkedList<int> list(data, 3);
    list.insert_at(1, 0);
    EXPECT_EQ(list.get_first(), 1);
    EXPECT_EQ(list.get_length(), 4);
}

TEST(LinkedList_InsertAt, AtLength_EqualsAppend) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    list.insert_at(4, 3);
    EXPECT_EQ(list.get_last(), 4);
    EXPECT_EQ(list.get_length(), 4);
}

TEST(LinkedList_InsertAt, AtMiddle_ShiftsCorrectly) {
    int data[] = {1, 3, 4};
    LinkedList<int> list(data, 3);
    list.insert_at(2, 1);
    EXPECT_EQ(list.get(0), 1);
    EXPECT_EQ(list.get(1), 2);
    EXPECT_EQ(list.get(2), 3);
    EXPECT_EQ(list.get(3), 4);
}

TEST(LinkedList_InsertAt, NegativeIndex_Throws) {
    LinkedList<int> list;
    EXPECT_THROW(list.insert_at(1, -1), std::out_of_range);
}

TEST(LinkedList_InsertAt, BeyondLength_Throws) {
    int data[] = {1, 2};
    LinkedList<int> list(data, 2);
    EXPECT_THROW(list.insert_at(5, 3), std::out_of_range);  // length=2, valid: 0..2
}

/*******************************************************************
 * remove_at
 *******************************************************************/

TEST(LinkedList_RemoveAt, RemoveFirst_UpdatesHead) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    int val = list.remove_at(0);
    EXPECT_EQ(val, 1);
    EXPECT_EQ(list.get_first(), 2);
    EXPECT_EQ(list.get_length(), 2);
}

TEST(LinkedList_RemoveAt, RemoveLast_UpdatesTail) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    int val = list.remove_at(2);
    EXPECT_EQ(val, 3);
    EXPECT_EQ(list.get_last(), 2);
}

TEST(LinkedList_RemoveAt, RemoveMiddle) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    list.remove_at(1);
    EXPECT_EQ(list.get(0), 1);
    EXPECT_EQ(list.get(1), 3);
}

TEST(LinkedList_RemoveAt, RemoveSingleElement_EmptyList) {
    LinkedList<int> list;
    list.append(42);
    list.remove_at(0);
    EXPECT_EQ(list.get_length(), 0);
    // После удаления последнего элемента get_first/get_last должны бросать
    EXPECT_THROW(list.get_first(), std::out_of_range);
    EXPECT_THROW(list.get_last(),  std::out_of_range);
}

TEST(LinkedList_RemoveAt, NegativeIndex_Throws) {
    LinkedList<int> list;
    list.append(1);
    EXPECT_THROW(list.remove_at(-1), std::out_of_range);
}

TEST(LinkedList_RemoveAt, IndexEqualLength_Throws) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    EXPECT_THROW(list.remove_at(3), std::out_of_range);
}

TEST(LinkedList_RemoveAt, OnEmpty_Throws) {
    LinkedList<int> list;
    EXPECT_THROW(list.remove_at(0), std::out_of_range);
}

/*******************************************************************
 * get_sublist
 *******************************************************************/

TEST(LinkedList_Sublist, FullRange) {
    int data[] = {1, 2, 3, 4, 5};
    LinkedList<int> list(data, 5);
    LinkedList<int>* sub = list.get_sublist(0, 4);
    EXPECT_EQ(sub->get_length(), 5);
    EXPECT_EQ(sub->get(0), 1);
    EXPECT_EQ(sub->get(4), 5);
    delete sub;
}

TEST(LinkedList_Sublist, SingleElement_StartEqualsEnd) {
    int data[] = {10, 20, 30};
    LinkedList<int> list(data, 3);
    LinkedList<int>* sub = list.get_sublist(1, 1);
    EXPECT_EQ(sub->get_length(), 1);
    EXPECT_EQ(sub->get(0), 20);
    delete sub;
}

TEST(LinkedList_Sublist, InvalidRange_StartGreaterEnd_Throws) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    EXPECT_THROW(list.get_sublist(2, 1), std::out_of_range);
}

TEST(LinkedList_Sublist, NegativeStart_Throws) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    EXPECT_THROW(list.get_sublist(-1, 2), std::out_of_range);
}

TEST(LinkedList_Sublist, EndBeyondLength_Throws) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    EXPECT_THROW(list.get_sublist(0, 3), std::out_of_range);
}

TEST(LinkedList_Sublist, OnEmpty_Throws) {
    LinkedList<int> list;
    EXPECT_THROW(list.get_sublist(0, 0), std::out_of_range);
}

/*******************************************************************
 * concat
 *******************************************************************/

TEST(LinkedList_Concat, Basic) {
    int d1[] = {1, 2}, d2[] = {3, 4, 5};
    LinkedList<int> a(d1, 2), b(d2, 3);
    LinkedList<int>* res = a.concat(&b);
    EXPECT_EQ(res->get_length(), 5);
    EXPECT_EQ(res->get(0), 1);
    EXPECT_EQ(res->get(4), 5);
    delete res;
}

TEST(LinkedList_Concat, WithEmptyList) {
    int d[] = {1, 2, 3};
    LinkedList<int> a(d, 3), b;
    LinkedList<int>* res = a.concat(&b);
    EXPECT_EQ(res->get_length(), 3);
    delete res;
}

TEST(LinkedList_Concat, EmptyWithNonEmpty) {
    LinkedList<int> a;
    int d[] = {4, 5};
    LinkedList<int> b(d, 2);
    LinkedList<int>* res = a.concat(&b);
    EXPECT_EQ(res->get_length(), 2);
    delete res;
}

TEST(LinkedList_Concat, NullptrOther_Throws) {
    LinkedList<int> list;
    EXPECT_THROW(list.concat(nullptr), std::invalid_argument);
}

/*******************************************************************
 * filter
 *******************************************************************/
//TODO добавить transform, set
TEST(LinkedList_Filter, KeepAll) {
    int data[] = {2, 4, 6};
    LinkedList<int> list(data, 3);
    list.filter([](const int& x) { return x % 2 == 0; });
    EXPECT_EQ(list.get_length(), 3);
}

TEST(LinkedList_Filter, RemoveAll) {
    int data[] = {1, 3, 5};
    LinkedList<int> list(data, 3);
    list.filter([](const int& x) { return x % 2 == 0; });
    EXPECT_EQ(list.get_length(), 0);
}

TEST(LinkedList_Filter, Mixed) {
    int data[] = {1, 2, 3, 4, 5};
    LinkedList<int> list(data, 5);
    list.filter([](const int& x) { return x % 2 == 0; });
    EXPECT_EQ(list.get_length(), 2);
    EXPECT_EQ(list.get(0), 2);
    EXPECT_EQ(list.get(1), 4);
}

TEST(LinkedList_Filter, RemoveFirst_HeadUpdated) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    list.filter([](const int& x) { return x > 1; });
    EXPECT_EQ(list.get_first(), 2);
}

TEST(LinkedList_Filter, RemoveLast_TailUpdated) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    list.filter([](const int& x) { return x < 3; });
    EXPECT_EQ(list.get_last(), 2);
}

/*******************************************************************
 * enumerator
 *******************************************************************/

TEST(LinkedList_Enumerator, IteratesAll) {
    int data[] = {10, 20, 30};
    LinkedList<int> list(data, 3);

    IEnumerator<int>* it = list.get_enumerator();
}