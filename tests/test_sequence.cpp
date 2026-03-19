#include <gtest/gtest.h>
#include "array_sequence.hpp" // конкретная реализация для инстанцирования Sequence<T>

// try_get тестируем через MutableArraySequence — логика в Sequence<T>::tpp,
// одинакова для всех потомков

TEST(Sequence_TryGet, ValidIndex_HasValue) {
    int d[] = {10, 20, 30};
    MutableArraySequence<int> s(d, 3);
    auto opt = s.try_get(1);
    EXPECT_TRUE(opt.has_value());
    EXPECT_EQ(opt.get_value(), 20);
}

TEST(Sequence_TryGet, InvalidIndex_NoValue) {
    int d[] = {1, 2, 3};
    MutableArraySequence<int> s(d, 3);
    EXPECT_FALSE(s.try_get(-1).has_value());
    EXPECT_FALSE(s.try_get(3).has_value());
}

TEST(Sequence_TryGetFirst, NonEmpty_HasValue) {
    int d[] = {42, 1, 2};
    MutableArraySequence<int> s(d, 3);
    auto opt = s.try_get_first();
    EXPECT_TRUE(opt.has_value());
    EXPECT_EQ(opt.get_value(), 42);
}

TEST(Sequence_TryGetFirst, OnEmpty_NoValue) {
    MutableArraySequence<int> s;
    EXPECT_FALSE(s.try_get_first().has_value());
}

TEST(Sequence_TryGetLast, NonEmpty_HasValue) {
    int d[] = {1, 2, 99};
    MutableArraySequence<int> s(d, 3);
    auto opt = s.try_get_last();
    EXPECT_TRUE(opt.has_value());
    EXPECT_EQ(opt.get_value(), 99);
}

TEST(Sequence_TryGetLast, OnEmpty_NoValue) {
    MutableArraySequence<int> s;
    EXPECT_FALSE(s.try_get_last().has_value());
}

TEST(Sequence_OperatorBracket, SameAsGet) {
    int d[] = {5, 10, 15};
    MutableArraySequence<int> s(d, 3);
    for (int i = 0; i < 3; i++)
        EXPECT_EQ(s[i], s.get(i));
}