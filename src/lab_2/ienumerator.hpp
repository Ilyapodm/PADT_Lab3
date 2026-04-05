#pragma once

// Abstract class for enumerator (interface enumerator)
template <typename T>
class IEnumerator {
public:
    virtual bool move_next() = 0;  // move to next element
    virtual const T& get_current() const = 0;  // get current item
    // virtual void set_current(const T &value) = 0;  // set the value
    virtual void reset() = 0;  // move to the beginning
    virtual ~IEnumerator() {}
};