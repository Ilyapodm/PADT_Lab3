#pragma once

#include "bit.hpp"
#include "dynamic_array.hpp"
#include "sequence.hpp"

class BitSequence : public Sequence<Bit> {
public:
    // constructors
    BitSequence();
    BitSequence(bool *items, int size);
    BitSequence(const BitSequence &other);
    ~BitSequence() {}   

    // getters
    const Bit& get_first() const override;
    const Bit& get_last() const override;
    const Bit& get(int index) const override;

    int get_size() const override;

    // operations
    // always creates new bitsequence, but can't put const here, because sequence doesn't have const
    BitSequence* append(const Bit &item) override;
    BitSequence* prepend(const Bit &item) override;
    BitSequence* insert_at(const Bit &item, int index) override;
    BitSequence* set(const Bit &item, int index) override;

    BitSequence* remove_at(int index) override;

    BitSequence* get_subsequence(int start_index, int end_index) const override;

    BitSequence* concat(const Sequence<Bit> &other) const override;

    // bit operations
    // 1. the length of result bit sequence is the minimum of "this" and "other" lengths for all operations 
    // we can't write "operator&", because it means, that we return Bit&, but if we use instance()(Mutable/Immutable), 
    // we can get clone (new object in the heap) in the method, so need to use ptr
    // 2. could use enumerate, but it's not so quick (new, delete...)
    // 3. don't use try-catch, because set can't throw, because = for "Bit" do not work with memory

    BitSequence* bit_and(const BitSequence &other) const;
    BitSequence* bit_or(const BitSequence &other) const;
    BitSequence* bit_xor(const BitSequence &other) const;
    BitSequence* bit_not() const;

    // map, where, reduce
    BitSequence*  map(Bit (*mapper)(const Bit& element)) override;
    BitSequence*  where(bool (*predicate)(const Bit& element)) override;
    Bit reduce(Bit (*reduce_func)(const Bit& first_element, const Bit& second_element), const Bit& start_element) override;
    BitSequence*  slice(int index, int count, const Sequence<Bit> &seq) override;

    // nested class enumerator
    class BitEnumerator : public IEnumerator<Bit> {
    public:
        BitEnumerator(const BitSequence *bit_sequence) : bit_sequence{bit_sequence}, index{-1} {}

        bool move_next() override;  // move to next element
        const Bit& get_current() const override;  // get current item
        void reset() override;  // move to the beginning

    private:
        int index;
        const BitSequence *bit_sequence;
    };

    IEnumerator<Bit>* get_enumerator() const override;

private:
    DynamicArray<Bit> data;
};
