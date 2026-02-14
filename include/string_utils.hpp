#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <iostream>
#include <sstream>
#include <string>
#include "./char_utils.hpp"
#include "./json_exception.hpp"


namespace json {

namespace utils {


// 在已经获得 \u 时读取 4 个 16进制数字, 解析为 2 个字符
inline const std::string parse_unicode_escape(std::istream& is) {
    int c;
    std::string result("\\u");
    for (int i = 0; i < 2; ++i) {
        // int buf = 0;
        for (int j = 0; j < 2; ++j) {
            c = is.get();
            if (is_eof(c) || !is_hex_digit(c)) throw json::JsonIstreamFailure(std::string("Unicode character requires 4 hex digits, but got `") + (is_eof(c) ? std::string("EOF") : std::string({(char)c})) + std::string("`."));
            ///buf = buf * 0x10 + to_hex_digit(c);
            result += (char)c;
        }
        // result += (char)buf;
    }
    return result;
}

// 在已经获得词首的双引号后读取一个字符串, 并添加前后双引号
inline const std::string get_string_from_istream(std::istream& is) {
    bool escaping = false;
    int c;
    std::string result({'"'});
    while (true) {
        c = is.get();
        if (is_eof(c) || is_breakline(c)) throw json::JsonIstreamFailure("Unclosed quatation mark is encountered when getting a String.");
        // 处理转义字符
        else if (escaping) {
            if (c == (int)'"') result += "\\\"";
            else if (c == (int)'\\') result += "\\\\";
            else if (c == (int)'/') result += "\\/";
            else if (c == (int)'b') result += "\\b";
            else if (c == (int)'f') result += "\\f";
            else if (c == (int)'n') result += "\\n";
            else if (c == (int)'r') result += "\\r";
            else if (c == (int)'t') result += "\\t";
            else if (c == (int)'u') result += parse_unicode_escape(is);
            else { result += '\\'; result += (char)c; }
            escaping = false;
        }
        // 处理非转义状态
        else {
            if (c == (int)'\\') escaping = true;
            else if (c == (int)'"') { result += '"'; break; }
            else result += (char)c;
        }
    }
    return result;
}

// 将字符串转义, 该方法暂未实现
inline const std::string parse_string_escape(const std::string& s) {
    return s;
}


}

}


#endif