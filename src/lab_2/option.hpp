#pragma once

#include <stdexcept>

template <class T>
class Option {
public:
    Option() : has_value_(false) {}
    Option(const T& val) : value_(val), has_value_(true) {}

    bool has_value() const { 
        return has_value_;
    }

    const T& get_value() const {
        if (!has_value_) {
            throw std::runtime_error("get_value: Option has no value");
        }

        return value_;
    }

    // "Фабричные методы" - used for more explicity
    static Option<T> None() { return Option<T>(); }  // no value, T must have a default constructor 
    static Option<T> Some(const T& val) { return Option<T>(val); }  // there is a value
private:
    T value_;
    bool has_value_;
};
