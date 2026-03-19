#include "bit_sequence.hpp"
#include <stdexcept>
#include "dynamic_array.hpp"

/*******************************************************************
 * constructors
 *******************************************************************/

BitSequence::BitSequence() : data{} {}

BitSequence::BitSequence(bool *items, int size) {
    data.resize(size);
    for (int i = 0; i < size; i++)
        data.set(i, Bit(items[i]));
}

BitSequence::BitSequence(const BitSequence &other) : data{other.data} {}

/*******************************************************************
 * getters
 *******************************************************************/

const Bit& BitSequence::get_first() const {
    return data.get(0);
}

const Bit& BitSequence::get_last() const {
    return data.get(data.get_size() - 1);
}

const Bit& BitSequence::get(int index) const  {
    return data.get(index);
}

int BitSequence::get_size() const {
    return data.get_size();
}

/*******************************************************************
 * operations
 *******************************************************************/

BitSequence* BitSequence::append(const Bit &item) {
    BitSequence *res = new BitSequence(*this);  // make a copy

    try {
        res->data.resize(res->data.get_size() + 1);  
        res->data.set(res->data.get_size() - 1,  item);  // set at size-1 because size is increased on previous string   
    } catch (...) { 
        delete res;
        throw;
    }

    return res;
}

BitSequence* BitSequence::prepend(const Bit &item)  {
    BitSequence *res = new BitSequence(*this);

    try{
        res->data.resize(res->data.get_size() + 1);
        
        for (int dst = res->data.get_size() - 1; dst > 0; dst--) {
            // T operator= can fail: for mutable have to delete too
            res->data.set(dst, res->data.get(dst - 1));
        }
        res->data.set(0,  item);
    } catch (...) {
        delete res;
        throw;
    }

    return res;
}

BitSequence* BitSequence::insert_at(const Bit &item, int index) {
    if (index < 0 || index > data.get_size())
        throw std::out_of_range("insert_at: index out of range");

    BitSequence *res = new BitSequence(*this);

    try {
        res->data.resize(res->data.get_size() + 1);
        // T operator= can fail: for mutable have to delete too
        for (int dst = res->data.get_size() - 1; dst > index; dst--) {
            res->data.set(dst, res->data.get(dst - 1));
        }

        res->data.set(index, item);
    } catch (...) {
        delete res;
        throw;
    }
    
    return res;
}

BitSequence* BitSequence::set(const Bit &item, int index) {
    if (index < 0 || index >= get_size())
        throw std::out_of_range("set: index out of range");

    BitSequence *res = new BitSequence(*this);

    try {
        // T operator= can fail: for mutable have to delete too
        res->data.set(index, item);
    } catch (...) {
        delete res;
        throw;
    }
    
    return res;
}

BitSequence* BitSequence::remove_at(int index) {
    if (index < 0 || index >= get_size())
        throw std::out_of_range("remove_at: index out of range");

    BitSequence *res = new BitSequence(*this);

    try {
        res->data.remove_at(index);
    } catch (...) {
        delete res;
        throw;
    }

    return res;
}

BitSequence* BitSequence::get_subsequence(int start_index, int end_index) const {
    if (start_index < 0 || end_index >= data.get_size() || start_index > end_index)  // cover size = 0
        throw std::out_of_range("get_subsequence: index out of range");

    // need this method because we don't know what (mutable/immutable) we are working with
    BitSequence *res = new BitSequence();

    try {
        int total_size = end_index - start_index + 1;  

        res->data.resize(total_size);  // resize all needed space one time

        for (int i = start_index; i <= end_index; i++){
            // do not use append because it can create another copy for immutable
            res->data.set(i - start_index, data.get(i));
        }
    } catch (...) {
        delete res;
        throw;
    }

    return res;
}

BitSequence* BitSequence::concat(const Sequence<Bit> &other) const {
    BitSequence *res = new BitSequence();

    try {
        int total_size = this->get_size() + other.get_size();
        res->data.resize(total_size);  // resize all needed space at one time

        for (int i = 0; i < this->get_size(); i++) 
            // do not use append because it can create another copy for immutable
            res->data.set(i, data.get(i));
        
        for (int i = 0; i < other.get_size(); i++) 
            res->data.set(this->get_size() + i, other.get(i));  // have already placed get_size - 1 spaces
        
    } catch (...) {
        delete res;
        throw;
    }
    
    return res;
}

/*******************************************************************
 * bit operations
 *******************************************************************/

 // 1. the length of result bit sequence is the minimum of "this" and "other" lengths for all operations 
    // we can't write "operator&", because it means, that we return Bit&, but if we use instance()(Mutable/Immutable), 
    // we can get clone (new object in the heap) in the method, so need to use ptr
    // 2. could use enumerate, but it's not so quick (new, delete...)
    // 3. don't use try-catch, because set can't throw, because = for "Bit" do not work with memory

BitSequence* BitSequence::bit_and(const BitSequence &other) const {
    int result_size = (this->get_size() > other.get_size()) ? other.get_size() : this->get_size();

    BitSequence* res = new BitSequence();

    res->data.resize(result_size);  // change size (minimum of "this" and "other")
    for (int i = 0; i < result_size; i++) 
        res->data.set(i, this->get(i) & other.get(i));

    return res;
}

BitSequence* BitSequence::bit_or(const BitSequence &other) const {
    int result_size = (this->get_size() > other.get_size()) ? other.get_size() : this->get_size();

    BitSequence* res = new BitSequence();

    res->data.resize(result_size);  // change size (minimum of "this" and "other")

    for (int i = 0; i < result_size; i++) 
        res->data.set(i, this->get(i) | other.get(i));

    return res;
}

BitSequence* BitSequence::bit_xor(const BitSequence &other) const {
    int result_size = (this->get_size() > other.get_size()) ? other.get_size() : this->get_size();

    BitSequence* res = new BitSequence();

    res->data.resize(result_size);  // change size (minimum of "this" and "other")

    for (int i = 0; i < result_size; i++) 
        res->data.set(i, this->get(i) ^ other.get(i));

    return res;
}

BitSequence* BitSequence::bit_not() const {
    BitSequence* res = new BitSequence();

    res->data.resize(this->get_size());

    for (int i = 0; i < data.get_size(); i++) 
        res->data.set(i, ~data.get(i));

    return res;
}

/*******************************************************************
 * map, where, reduce
 *******************************************************************/

BitSequence*  BitSequence::map(Bit (*mapper)(const Bit& element)) {
    BitSequence *res = new BitSequence(*this);

    try { // mapper can fail
        for (int i = 0; i < res->data.get_size(); i++) {
            res->data.set(i, mapper(res->data.get(i)));
        }
    } catch (...) {
        delete res;
        throw;
    }

    return res;
}

BitSequence*  BitSequence::where(bool (*predicate)(const Bit& element)) {
    BitSequence *res = new BitSequence(*this);

    int src = 0, dst = 0;

    try {
        for (; src < res->data.get_size(); src++) {
            if (predicate(res->data.get(src))) {
                res->data.set(dst, res->get(src));
                dst++;
            }
        }
        res->data.resize(dst);  // size can be changed
    } catch (...) {
        delete res;
        throw;
    }
    
    return res;
}

Bit BitSequence::reduce(Bit (*reduce_func)(const Bit& first_element, const Bit& second_element),
                        const Bit& start_element) {
    Bit result = start_element;

    for (int i = 0; i < get_size(); i++) {
        result = reduce_func(get(i), result);  // usually start_element is the 1 arg
    }

    return result;
}

BitSequence* BitSequence::slice(int index, int count, const Sequence<Bit> &seq) {
    // if "count" go out of the edge, silently but it till this edge
    if (count < 0)
        throw::std::invalid_argument("slice: count cannot be negetive");
    if (index < 0)
        index = get_size() + index;
    if (index < 0 || index >= get_size()) 
        throw std::out_of_range("slice: Index out of range");
        
    //         left part    deleted        right part
    // this: [0 .. index) [index .. end) [end .. get_size())

    //       left part        inserted          right part
    // inst: [0 .. index) [ seq.get_size() ] [end .. get_size())
    int end = get_size() < index + count ? get_size() : index + count;  // the next element after replaced elements

    BitSequence *res = new BitSequence(*this);

    int new_size = index + seq.get_size() + (get_size() - end);

    DynamicArray<Bit> new_array(new_size);

    try {
        int dst = 0;
        for (int i = 0; i < index; i++)
            new_array.set(dst++, get(i));

        for (int i = 0; i < seq.get_size(); i++)
            new_array.set(dst++, seq.get(i));

        for (int i = end; i < get_size(); i++)
            new_array.set(dst++, get(i));
    } catch (...) {
        delete res;
        throw;
    }

    res->data = new_array;  // copy for dyn array
    return res;
}

/*******************************************************************
 * Enumerator
 *******************************************************************/

IEnumerator<Bit>* BitSequence::get_enumerator() const {
    return new BitEnumerator(this);
}

bool BitSequence::BitEnumerator::move_next() {
    index++;
    return index < bit_sequence->get_size();
}

const Bit& BitSequence::BitEnumerator::get_current() const {
    if (index < 0 || index >= bit_sequence->get_size())
        throw std::out_of_range("get_current(): called in invalid state");
    return bit_sequence->get(index);
}

void BitSequence::BitEnumerator::reset() {
    index = -1;
}