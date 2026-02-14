#ifndef JSON_TYPING_HPP
#define JSON_TYPING_HPP

#include <iostream>
#include <sstream>
#include "./json_exception.hpp"


namespace json {

enum JsonElementTyping: int {
    ERROR = 0,
    NULL_TYPE = 1,
    STRING = 2,
    NUMBER = 3,
    BOOLEAN = 4,
    OBJECT = 5,
    ARRAY = 6,
};

class NullType {
public:
    NullType() {}
    NullType(const NullType&) {}
    NullType(NullType&&) {}
    NullType& operator = (const NullType&) { return *this; } 
    NullType& operator = (NullType&&) { return *this; } 
    const std::string to_string() const { return std::string("null"); }
    friend std::ostream& operator << (std::ostream& os, const NullType&) { return os << "null"; }
};

using Int64 = long long;
using Float64 = double;

class Number {

private:
    std::string integer_part;
    std::string floating_part;
    std::string exponent_part;
    bool is_negative = false;
    bool is_exp_negative = false;
    bool has_floating = false;
    bool has_exponent = false;

public:
    Number() {}
    Number(const std::string& integer_part, bool is_negative):
        integer_part(integer_part), is_negative(is_negative) {}
    Number(const std::string& integer_part, const std::string& floating_part, bool is_negative):
        integer_part(integer_part), floating_part(floating_part), is_negative(is_negative), has_floating(true) {}
    Number(const std::string& integer_part, const std::string& exponent_part, bool is_negative, bool is_exp_negative):
        integer_part(integer_part), exponent_part(exponent_part), is_negative(is_negative), is_exp_negative(is_exp_negative), has_exponent(true) {}
    Number(const std::string& integer_part, const std::string& floating_part, const std::string& exponent_part, bool is_negative, bool is_exp_negative):
        integer_part(integer_part), floating_part(floating_part), exponent_part(exponent_part), is_negative(is_negative), is_exp_negative(is_exp_negative), has_floating(true), has_exponent(true) {}
    Number(const std::string& integer_part, const std::string& floating_part, const std::string& exponent_part, bool is_negative, bool is_exp_negative, bool has_floating, bool has_exponent):
        integer_part(integer_part), floating_part(floating_part), exponent_part(exponent_part), is_negative(is_negative), is_exp_negative(is_exp_negative), has_floating(has_floating), has_exponent(has_exponent) {}

    Number(const Number& num):
        integer_part(num.integer_part), floating_part(num.floating_part), exponent_part(num.exponent_part), is_negative(num.is_negative), is_exp_negative(num.is_exp_negative), has_floating(num.has_floating), has_exponent(num.has_exponent) {}
    
    Number& operator = (const Number& num) {
        integer_part = num.integer_part;
        floating_part = num.floating_part;
        exponent_part = num.exponent_part;
        is_negative = num.is_negative;
        is_exp_negative = num.is_exp_negative;
        has_floating = num.has_floating;
        has_exponent = num.has_exponent;
        return *this;
    }

    inline bool is_float() const { return has_floating || has_exponent; }
    inline bool is_int() const { return !is_float(); }

    const std::string to_string() const {
        std::string result;
        if (is_negative) result += '-';
        result += integer_part;
        if (has_floating) result += '.' + floating_part;
        if (has_exponent) {
            result += 'e';
            if (is_exp_negative) result += '-';
            result += exponent_part;
        }
        return result;
    }
    Int64 to_int(bool strict = false) const {
        if (strict && is_float()) throw json::JsonTypeError("Cannot convert a json float Number to an int.");
        Int64 result;
        std::stringstream ss;
        ss << to_string();
        ss >> result;
        return result;
    }
    Float64 to_float(bool strict = false) const {
        if (strict && is_int()) throw json::JsonTypeError("Cannot convert a json int Number to a float.");
        Float64 result;
        std::stringstream ss;
        ss << to_string();
        ss >> result;
        return result;
    }

};


} 


#endif