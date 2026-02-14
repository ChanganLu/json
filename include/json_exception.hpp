#ifndef JSON_EXCEPTION_HPP
#define JSON_EXCEPTION_HPP

#include <string>
#include <exception>


namespace json {

class JsonException: public std::exception {
protected:
    std::string _head;
    std::string _text;
public:
    JsonException(const char* message, const char* prefill = "Json exception"): _head(prefill), _text(message) {}
    JsonException(const std::string& message, const std::string& prefill = std::string("Json exception")): _head(prefill), _text(message) {}
    virtual const char* what() const noexcept override { return (_head + ": " + _text).c_str(); }
};

class JsonIstreamFailure: public JsonException {
public:
    JsonIstreamFailure(const char* message, const char* prefill = "Json istream failed"): JsonException(message, prefill) {}
    JsonIstreamFailure(const std::string& message, const std::string& prefill = std::string("Json istream failed")): JsonException(message, prefill) {}
};

class JsonTypeError: public JsonException {
public:
    JsonTypeError(const char* message, const char* prefill = "Json wrong type"): JsonException(message, prefill) {}
    JsonTypeError(const std::string& message, const std::string& prefill = std::string("Json wrong type")): JsonException(message, prefill) {}
};



}

#endif