#include <gtest/gtest.h>
#include "bit_sequence.hpp"

/*******************************************************************
* Helpers
*******************************************************************/

// bit_seq from str
static BitSequence* from_str(const char* s) {
    int n = 0;
    while (s[n]) n++;
    bool* bits = new bool[n];
    for (int i = 0; i < n; i++) bits[i] = (s[i] == '1');
    auto* seq = new BitSequence(bits, n);
    delete[] bits;
    return seq;
}

// check str == bit_seq
static void expect_bits(const BitSequence* seq, const char* expected) {
    int n = 0;
    while (expected[n]) n++;
    ASSERT_EQ(seq->get_size(), n);
    for (int i = 0; i < n; i++)
        EXPECT_EQ(seq->get(i).get(), expected[i] == '1') << "  at index " << i;
}

/*******************************************************************
* bit_and
*******************************************************************/

TEST(BitSequence_And, BasicAnd) {
    BitSequence* a = from_str("1100");
    BitSequence* b = from_str("1010");
    BitSequence* res = a->bit_and(*b);
    expect_bits(res, "1000");
    delete a; delete b; delete res;
}

TEST(BitSequence_And, AllZeros) {
    BitSequence* a = from_str("1111");
    BitSequence* b = from_str("0000");
    BitSequence* res = a->bit_and(*b);
    expect_bits(res, "0000");
    delete a; delete b; delete res;
}

TEST(BitSequence_And, AllOnes) {
    BitSequence* a = from_str("1111");
    BitSequence* b = from_str("1111");
    BitSequence* res = a->bit_and(*b);
    expect_bits(res, "1111");
    delete a; delete b; delete res;
}

TEST(BitSequence_And, DifferentLengths_ResultIsMin) {
    BitSequence* a = from_str("11001");   // длина 5
    BitSequence* b = from_str("110");     // длина 3
    BitSequence* res = a->bit_and(*b);
    EXPECT_EQ(res->get_size(), 3);        // min(5, 3)
    expect_bits(res, "110");
    delete a; delete b; delete res;
}

TEST(BitSequence_And, OriginalUnchanged) {
    BitSequence* a = from_str("1010");
    BitSequence* b = from_str("1100");
    BitSequence* res = a->bit_and(*b);
    expect_bits(a, "1010");  // a не изменился
    expect_bits(b, "1100");  // b не изменился
    delete a; delete b; delete res;
}

TEST(BitSequence_And, ReturnsNewObject) {
    BitSequence* a = from_str("1010");
    BitSequence* b = from_str("1100");
    BitSequence* res = a->bit_and(*b);
    EXPECT_NE(res, a);
    EXPECT_NE(res, b);
    delete a; delete b; delete res;
}

/*******************************************************************
* bit_or
*******************************************************************/

TEST(BitSequence_Or, BasicOr) {
    BitSequence* a = from_str("1100");
    BitSequence* b = from_str("1010");
    BitSequence* res = a->bit_or(*b);
    expect_bits(res, "1110");
    delete a; delete b; delete res;
}

TEST(BitSequence_Or, AllZeros) {
    BitSequence* a = from_str("0000");
    BitSequence* b = from_str("0000");
    BitSequence* res = a->bit_or(*b);
    expect_bits(res, "0000");
    delete a; delete b; delete res;
}

TEST(BitSequence_Or, WithAllOnes_GivesAllOnes) {
    BitSequence* a = from_str("0000");
    BitSequence* b = from_str("1111");
    BitSequence* res = a->bit_or(*b);
    expect_bits(res, "1111");
    delete a; delete b; delete res;
}

TEST(BitSequence_Or, DifferentLengths_ResultIsMin) {
    BitSequence* a = from_str("10101");
    BitSequence* b = from_str("011");
    BitSequence* res = a->bit_or(*b);
    EXPECT_EQ(res->get_size(), 3);
    expect_bits(res, "111");
    delete a; delete b; delete res;
}

TEST(BitSequence_Or, OriginalUnchanged) {
    BitSequence* a = from_str("1010");
    BitSequence* b = from_str("0101");
    BitSequence* res = a->bit_or(*b);
    expect_bits(a, "1010");
    expect_bits(b, "0101");
    delete a; delete b; delete res;
}

/*******************************************************************
* bit_xor
*******************************************************************/
TEST(BitSequence_Xor, BasicXor) {
    BitSequence* a = from_str("1100");
    BitSequence* b = from_str("1010");
    BitSequence* res = a->bit_xor(*b);
    expect_bits(res, "0110");
    delete a; delete b; delete res;
}

TEST(BitSequence_Xor, SameInput_GivesAllZeros) {
    BitSequence* a = from_str("1011");
    BitSequence* b = from_str("1011");
    BitSequence* res = a->bit_xor(*b);
    expect_bits(res, "0000");
    delete a; delete b; delete res;
}

TEST(BitSequence_Xor, WithAllZeros_GivesSelf) {
    BitSequence* a = from_str("1011");
    BitSequence* b = from_str("0000");
    BitSequence* res = a->bit_xor(*b);
    expect_bits(res, "1011");
    delete a; delete b; delete res;
}

TEST(BitSequence_Xor, DifferentLengths_ResultIsMin) {
    BitSequence* a = from_str("10110");
    BitSequence* b = from_str("101");
    BitSequence* res = a->bit_xor(*b);
    EXPECT_EQ(res->get_size(), 3);
    expect_bits(res, "000");
    delete a; delete b; delete res;
}

TEST(BitSequence_Xor, OriginalUnchanged) {
    BitSequence* a = from_str("1100");
    BitSequence* b = from_str("1010");
    BitSequence* res = a->bit_xor(*b);
    expect_bits(a, "1100");
    expect_bits(b, "1010");
    
    delete a; delete b; delete res;
}

/*******************************************************************
* bit_not
*******************************************************************/

TEST(BitSequence_Not, BasicNot) {
    BitSequence* a = from_str("1010");
    BitSequence* res = a->bit_not();
    expect_bits(res, "0101");
    delete a; delete res;
}

TEST(BitSequence_Not, AllZeros_GivesAllOnes) {
    BitSequence* a = from_str("0000");
    BitSequence* res = a->bit_not();
    expect_bits(res, "1111");
    delete a; delete res;
}

TEST(BitSequence_Not, AllOnes_GivesAllZeros) {
    BitSequence* a = from_str("1111");
    BitSequence* res = a->bit_not();
    expect_bits(res, "0000");
    delete a; delete res;
}

TEST(BitSequence_Not, DoubleNot_GivesSelf) {
    BitSequence* a    = from_str("10110100");
    BitSequence* not1 = a->bit_not();
    BitSequence* not2 = not1->bit_not();
    expect_bits(not2, "10110100");
    delete a; delete not1; delete not2;
}

TEST(BitSequence_Not, SizePreserved) {
    BitSequence* a = from_str("10110");
    BitSequence* res = a->bit_not();
    EXPECT_EQ(res->get_size(), 5);
    delete a; delete res;
}

TEST(BitSequence_Not, OriginalUnchanged) {
    BitSequence* a = from_str("1010");
    BitSequence* res = a->bit_not();
    expect_bits(a, "1010");
    delete a; delete res;
}

TEST(BitSequence_Not, OnEmpty_ReturnsEmpty) {
    BitSequence* a = new BitSequence();
    BitSequence* res = a->bit_not();
    EXPECT_EQ(res->get_size(), 0);
    delete a; delete res;
}

/*******************************************************************
* Комбинированные
*******************************************************************/

TEST(BitSequence_Combined, AndWithNot_GivesAndNot) {
    // a AND (NOT b) — 
    BitSequence* a    = from_str("1111");
    BitSequence* b    = from_str("1010");
    BitSequence* notb = b->bit_not();
    BitSequence* res  = a->bit_and(*notb);
    expect_bits(res, "0101");
    delete a; delete b; delete notb; delete res;
}

TEST(BitSequence_Combined, XorSelfAfterAnd) {
    // (a AND b) XOR a — 
    BitSequence* a   = from_str("1110");
    BitSequence* b   = from_str("1100");
    BitSequence* ab  = a->bit_and(*b);   // 1100
    BitSequence* res = ab->bit_xor(*a);  // 1100 XOR 1110 = 0010
    expect_bits(res, "0010");
    delete a; delete b; delete ab; delete res;
}